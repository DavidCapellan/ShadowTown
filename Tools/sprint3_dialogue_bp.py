"""
Sprint 3 — Diálogos sin C++ (funciona antes de compilar ShadowTown).
Bloquea movimiento del jugador, anima NPCs y mejora la plaza de diálogo.
"""
import unreal

PROJECT = "/Game/_Game"
MAP = f"{PROJECT}/Maps/Map_Circuito_Final"
BP_JOAQUIN = f"{PROJECT}/Blueprints/BP_Joaquin_Player"
BP_SOMBRA = f"{PROJECT}/Blueprints/BP_Sombra_NPC"
DT_CSV = "C:/UE4/ShadowTown/Content/_Game/DialogueData.csv"

TALK_FEMALE = f"{PROJECT}/Characters/NPC/Mesh/Lina2Armature_Speak"
TALK_MALE = f"{PROJECT}/Characters/Joaquín/Exra3Armature_Talk"
IDLE_FEMALE = f"{PROJECT}/Characters/NPC/Mesh/Lina2Armature_Idle-Stay"

# Estado global de sesión de diálogo (solo editor PIE / runtime simple)
_DIALOGUE = {
    "active": False,
    "lines": [],
    "index": 0,
    "player": None,
    "npc": None,
    "widget": None,
    "prev_anim": None,
}


def log(msg: str) -> None:
    unreal.log(f"[Sprint3-BP] {msg}")


def load_lines(gender: str, line_type: str = "PreCombate") -> list[dict]:
    lines = []
    try:
        with open(DT_CSV, encoding="utf-8") as f:
            header = f.readline().strip().split(",")
            for row in f:
                parts = [p.strip().strip('"') for p in row.split(",")]
                if len(parts) < 5:
                    continue
                if parts[2] != gender or parts[1] != line_type:
                    continue
                lines.append({"speaker": parts[2], "text": parts[4]})
    except OSError as exc:
        log(f"CSV read failed: {exc}")
    return lines


def get_or_create_widget(player_controller) -> unreal.UserWidget:
    wbp_path = f"{PROJECT}/UI/WBP_DialogueHUD"
    if not unreal.EditorAssetLibrary.does_asset_exist(wbp_path):
        factory = unreal.WidgetBlueprintFactory()
        factory.set_editor_property("parent_class", unreal.UserWidget)
        tools = unreal.AssetToolsHelpers.get_asset_tools()
        tools.create_asset("WBP_DialogueHUD", f"{PROJECT}/UI", unreal.WidgetBlueprint, factory)
        unreal.EditorAssetLibrary.save_asset(wbp_path)
        log("Created WBP_DialogueHUD — añade TextBlock + Border en el editor")

    wbp_class = unreal.EditorAssetLibrary.load_blueprint_class(wbp_path)
    widget = unreal.WidgetBlueprintLibrary.create(player_controller, wbp_class, player_controller)
    if widget and not widget.is_in_viewport():
        widget.add_to_viewport(200)
    return widget


def set_widget_text(widget, speaker: str, text: str) -> None:
    if not widget:
        return
    for name in ("DialogueText", "Txt_Dialogue", "TextBlock_Dialogue"):
        tb = widget.get_widget_from_name(name)
        if tb:
            tb.set_text(unreal.Text.from_string(text))
            break
    for name in ("SpeakerText", "Txt_Speaker", "TextBlock_Speaker"):
        tb = widget.get_widget_from_name(name)
        if tb:
            tb.set_text(unreal.Text.from_string(speaker))
            break


def lock_player(player: unreal.Character, lock: bool) -> None:
    if not player:
        return
    move = player.get_character_movement()
    if move:
        if lock:
            move.stop_movement_immediately()
            move.disable_movement()
        else:
            move.set_movement_mode(unreal.MovementMode.WALKING)
    pc = player.get_controller()
    if pc:
        pc.set_ignore_move_input(lock)


def play_npc_talk(npc: unreal.Actor, gender: str) -> None:
    mesh = npc.get_component_by_class(unreal.SkeletalMeshComponent)
    if not mesh:
        return
    path = TALK_FEMALE if gender == "Female" else TALK_MALE
    talk = unreal.load_asset(path)
    if talk:
        _DIALOGUE["prev_anim"] = mesh.get_editor_property("animation_mode")
        mesh.set_animation_mode(unreal.AnimationMode.ANIMATION_SINGLE_NODE)
        mesh.play_animation(talk, True)


def stop_npc_talk(npc: unreal.Actor) -> None:
    mesh = npc.get_component_by_class(unreal.SkeletalMeshComponent) if npc else None
    if not mesh:
        return
    mesh.stop()
    idle = unreal.load_asset(IDLE_FEMALE)
    if idle:
        mesh.play_animation(idle, True)


def show_line() -> None:
    if not _DIALOGUE["active"]:
        return
    idx = _DIALOGUE["index"]
    lines = _DIALOGUE["lines"]
    if idx >= len(lines):
        end_dialogue()
        return
    line = lines[idx]
    pc = _DIALOGUE["player"].get_controller() if _DIALOGUE["player"] else None
    w = _DIALOGUE["widget"] or (get_or_create_widget(pc) if pc else None)
    _DIALOGUE["widget"] = w
    set_widget_text(w, line["speaker"], line["text"])
    log(f"Line {idx + 1}/{len(lines)}: {line['text'][:60]}...")


def start_dialogue(player: unreal.Character, npc: unreal.Actor, gender: str = "Female") -> None:
    if _DIALOGUE["active"]:
        return
    lines = load_lines(gender, "PreCombate")
    if not lines:
        log("No dialogue lines found")
        return
    _DIALOGUE.update({
        "active": True,
        "lines": lines,
        "index": 0,
        "player": player,
        "npc": npc,
        "widget": None,
    })
    lock_player(player, True)
    play_npc_talk(npc, gender)
    show_line()
    log("Dialogue started")


def advance_dialogue() -> None:
    if not _DIALOGUE["active"]:
        return
    _DIALOGUE["index"] += 1
    show_line()


def end_dialogue() -> None:
    if not _DIALOGUE["active"]:
        return
    lock_player(_DIALOGUE["player"], False)
    stop_npc_talk(_DIALOGUE["npc"])
    w = _DIALOGUE["widget"]
    if w:
        w.remove_from_parent()
    _DIALOGUE.update({
        "active": False,
        "lines": [],
        "index": 0,
        "player": None,
        "npc": None,
        "widget": None,
        "prev_anim": None,
    })
    log("Dialogue ended")


def bind_pie_input() -> None:
    """Registra Space/E para avanzar diálogo en PIE."""
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    if not world:
        return
    player = unreal.GameplayStatics.get_player_character(world, 0)
    if not player:
        return
    pc = player.get_controller()
    if not pc:
        return
    input_comp = pc.input_component
    if not input_comp:
        return

    def _advance(*args):
        advance_dialogue()

    input_comp.bind_action("AdvanceDialogue", unreal.InputEvent.IE_PRESSED, _advance, True)
    log("Bound AdvanceDialogue (Space/E) for PIE")


def decorate_plaza() -> None:
    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    if not world:
        return

    center = unreal.Vector(800, 0, 120)

    # Iluminación cálida en la zona de diálogo
    for loc, yaw in [
        (unreal.Vector(550, -280, 380), 25),
        (unreal.Vector(1050, 280, 380), -155),
        (unreal.Vector(800, 0, 520), -50),
    ]:
        light = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.PointLight, loc, unreal.Rotator(-40, yaw, 0))
        if light:
            comp = light.get_component_by_class(unreal.PointLightComponent)
            comp.set_intensity(5200)
            comp.set_light_color(unreal.LinearColor(1.0, 0.9, 0.72, 1.0))
            comp.set_attenuation_radius(2000)
            comp.set_cast_shadows(True)

    cube = unreal.load_asset("/Engine/BasicShapes/Cube")
    for ox, oy, yaw in [(-220, -180, 0), (220, -180, 90), (-220, 180, -90), (220, 180, 180)]:
        bench = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.StaticMeshActor,
            center + unreal.Vector(ox, oy, 45),
            unreal.Rotator(0, yaw, 0))
        if bench and cube:
            sm = bench.static_mesh_component
            sm.set_static_mesh(cube)
            sm.set_world_scale3d(unreal.Vector(1.4, 0.55, 0.4))
            sm.set_material(0, None)

    # Reemplazar mesh suelto por BP NPC si existe
    sombra_cls = unreal.EditorAssetLibrary.load_blueprint_class(BP_SOMBRA)
    joaquin_cls = unreal.EditorAssetLibrary.load_blueprint_class(BP_JOAQUIN)

    if sombra_cls:
        npc = unreal.EditorLevelLibrary.spawn_actor_from_class(
            sombra_cls, center + unreal.Vector(250, 0, 0), unreal.Rotator(0, -90, 0))
        if npc:
            npc.set_actor_label("NPC_Sombra_Dialogo")

        npc2 = unreal.EditorLevelLibrary.spawn_actor_from_class(
            sombra_cls, center + unreal.Vector(420, 320, 0), unreal.Rotator(0, -135, 0))
        if npc2:
            npc2.set_actor_label("NPC_Sombra_Hombre")

    if joaquin_cls:
        pj = unreal.EditorLevelLibrary.spawn_actor_from_class(
            joaquin_cls, center + unreal.Vector(-400, 0, 0), unreal.Rotator(0, 90, 0))
        if pj:
            pj.set_actor_label("Joaquin_PlayerStart")

    unreal.EditorLevelLibrary.save_current_level()
    log("Plaza de diálogo decorada")


def setup_overlap_triggers() -> None:
    """Añade esfera de interacción a BP_Sombra si no tiene lógica."""
    log(
        "Añade en BP_Sombra_NPC: Sphere Overlap -> Cast to BP_Joaquin -> "
        "Call start_dialogue (o usa el componente C++ tras compilar)"
    )


def run() -> None:
    log("Sprint 3 Blueprint setup...")
    decorate_plaza()
    setup_overlap_triggers()
    log("Listo. En PIE, llama start_dialogue(player, npc, 'Female') desde overlap o consola Python.")


run()
