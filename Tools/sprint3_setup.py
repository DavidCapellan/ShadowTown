"""
Sprint 3 — Configura diálogos, widget, NPCs y decoración del parque en el editor.
Ejecutar con: execute_python (ruta a este archivo) desde ue-mcp o consola UE.
"""
import unreal

PROJECT = "/Game/_Game"
CSV_PATH = "C:/UE4/ShadowTown/Content/_Game/DialogueData.csv"
DT_PATH = f"{PROJECT}/Data/DT_DialogueData"
WIDGET_PATH = f"{PROJECT}/UI/WBP_DialogueHUD"
WIDGET_CLASS = "/Script/ShadowTown.ShadowDialogueWidget"

BP_JOAQUIN = f"{PROJECT}/Blueprints/BP_Joaquin_Player"
BP_SOMBRA = f"{PROJECT}/Blueprints/BP_Sombra_NPC"
BP_GM = f"{PROJECT}/Blueprints/GM_ShadowTown"
MAP_PATH = f"{PROJECT}/Maps/Map_Circuito_Final"

TALK_FEMALE = f"{PROJECT}/Characters/NPC/Mesh/Lina2Armature_Speak.Lina2Armature_Speak"
TALK_MALE = f"{PROJECT}/Characters/Joaquín/Exra3Armature_Talk.Exra3Armature_Talk"
IDLE_FEMALE = f"{PROJECT}/Characters/NPC/Mesh/Lina2Armature_Idle-Stay.Lina2Armature_Idle-Stay"


def log(msg: str) -> None:
    unreal.log(f"[Sprint3] {msg}")


def ensure_datatable() -> unreal.DataTable:
    if unreal.EditorAssetLibrary.does_asset_exist(DT_PATH):
        log(f"DataTable exists: {DT_PATH}")
        return unreal.load_asset(DT_PATH)

    row_struct = unreal.load_object(None, "/Script/ShadowTown.ShadowDialogueRow")
    if not row_struct:
        raise RuntimeError("Compile ShadowTown module first (ShadowDialogueRow missing).")

    factory = unreal.DataTableFactory()
    factory.struct = row_struct
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    dt = tools.create_asset("DT_DialogueData", f"{PROJECT}/Data", unreal.DataTable, factory)

    # Import CSV
    task = unreal.AssetImportTask()
    task.filename = CSV_PATH
    task.destination_path = f"{PROJECT}/Data"
    task.destination_name = "DT_DialogueData"
    task.replace_existing = True
    task.automated = True
    task.save = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])

    if unreal.EditorAssetLibrary.does_asset_exist(DT_PATH):
        unreal.EditorAssetLibrary.save_asset(DT_PATH)
        log("Imported DialogueData.csv into DT_DialogueData")
        return unreal.load_asset(DT_PATH)

    raise RuntimeError("Failed to create DT_DialogueData")


def ensure_dialogue_widget() -> str:
    if unreal.EditorAssetLibrary.does_asset_exist(WIDGET_PATH):
        log(f"Widget exists: {WIDGET_PATH}")
        return WIDGET_PATH

    parent = unreal.load_class(None, WIDGET_CLASS)
    factory = unreal.WidgetBlueprintFactory()
    factory.parent_class = parent
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    wbp = tools.create_asset("WBP_DialogueHUD", f"{PROJECT}/UI", unreal.WidgetBlueprint, factory)
    if not wbp:
        raise RuntimeError("Could not create WBP_DialogueHUD")

    unreal.EditorAssetLibrary.save_asset(wbp.get_path_name())
    log("Created WBP_DialogueHUD (add Text blocks named DialogueText / SpeakerText in editor)")
    return WIDGET_PATH


def add_component_to_bp(bp_path: str, comp_class_path: str, comp_name: str) -> None:
    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"Missing blueprint: {bp_path}")
        return
    comp_class = unreal.load_class(None, comp_class_path)
    if not comp_class:
        log(f"Missing component class: {comp_class_path}")
        return
    unreal.BlueprintEditorLibrary.add_component_to_blueprint(bp, comp_class, comp_name)
    unreal.EditorAssetLibrary.save_asset(bp_path)
    log(f"Added {comp_name} to {bp_path}")


def configure_sombra_npc(dt: unreal.DataTable) -> None:
    bp = unreal.load_asset(BP_SOMBRA)
    if not bp:
        return
    gen = bp.generated_class()
    cdo = gen.get_default_object() if gen else None
    comps = cdo.get_components_by_class(unreal.ShadowDialogueNpcComponent) if cdo else []
    for c in comps:
        c.set_editor_property("dialogue_table", dt)
        c.set_editor_property("npc_gender", "Female")
        c.set_editor_property("npc_display_name", "Sombra")
        c.set_editor_property("dialogue_type", unreal.ShadowDialogueLineType.PRE_COMBATE)
        talk = unreal.load_asset(TALK_FEMALE)
        idle = unreal.load_asset(IDLE_FEMALE)
        if talk:
            c.set_editor_property("talk_animation", talk)
        if idle:
            c.set_editor_property("idle_animation", idle)
    unreal.EditorAssetLibrary.save_asset(BP_SOMBRA)
    log("Configured BP_Sombra_NPC dialogue component")


def configure_game_mode(widget_path: str) -> None:
    gm = unreal.load_asset(BP_GM)
    if not gm:
        return
    wbp_class = unreal.EditorAssetLibrary.load_blueprint_class(widget_path)
    subsystem_class = unreal.ShadowDialogueSubsystem
    log("Set GM_ShadowTown to assign DialogueWidgetClass on BeginPlay via Blueprint")
    unreal.EditorAssetLibrary.save_asset(BP_GM)


def decorate_dialogue_plaza() -> None:
    editor_sub = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_sub.get_editor_world()
    if not world:
        return

    # Plaza de diálogo cerca del NPC existente
    center = unreal.Vector(800, 0, 120)
    lights = [
        (unreal.Vector(600, -300, 350), unreal.Rotator(-35, 30, 0)),
        (unreal.Vector(1000, 300, 350), unreal.Rotator(-35, -140, 0)),
        (unreal.Vector(800, 0, 500), unreal.Rotator(-50, 0, 0)),
    ]
    for loc, rot in lights:
        la = unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.PointLight, loc, rot)
        if la:
            comp = la.get_component_by_class(unreal.PointLightComponent)
            if comp:
                comp.set_intensity(4500)
                comp.set_light_color(unreal.LinearColor(1.0, 0.92, 0.75, 1.0))
                comp.set_attenuation_radius(1800)

    # Bancos / pedestales decorativos
    cube_mesh = unreal.load_asset("/Engine/BasicShapes/Cube.Cube")
    for i, offset in enumerate([(-200, -200), (200, -200), (-200, 200), (200, 200)]):
        loc = center + unreal.Vector(offset[0], offset[1], 40)
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, loc, unreal.Rotator(0, i * 45, 0))
        if actor and cube_mesh:
            sm = actor.static_mesh_component
            sm.set_static_mesh(cube_mesh)
            sm.set_world_scale3d(unreal.Vector(1.2, 0.5, 0.35))

    # Segundo NPC masculino
    sombra_class = unreal.EditorAssetLibrary.load_blueprint_class(BP_SOMBRA)
    if sombra_class:
        male_loc = center + unreal.Vector(400, 350, 0)
        male_npc = unreal.EditorLevelLibrary.spawn_actor_from_class(sombra_class, male_loc, unreal.Rotator(0, -120, 0))
        if male_npc:
            for c in male_npc.get_components_by_class(unreal.ShadowDialogueNpcComponent):
                c.set_editor_property("npc_gender", "Male")
                talk = unreal.load_asset(TALK_MALE)
                if talk:
                    c.set_editor_property("talk_animation", talk)

    # Jugador
    joaquin_class = unreal.EditorAssetLibrary.load_blueprint_class(BP_JOAQUIN)
    if joaquin_class:
        player_start = center + unreal.Vector(-350, 0, 0)
        unreal.EditorLevelLibrary.spawn_actor_from_class(joaquin_class, player_start, unreal.Rotator(0, 90, 0))

    unreal.EditorLevelLibrary.save_current_level()
    log("Decorated dialogue plaza and placed player/NPCs")


def set_default_gamemode() -> None:
    unreal.EditorAssetLibrary.save_directory(PROJECT)


def run() -> None:
    log("Starting Sprint 3 setup...")
    dt = ensure_datatable()
    widget_path = ensure_dialogue_widget()
    add_component_to_bp(BP_SOMBRA, "/Script/ShadowTown.ShadowDialogueNpcComponent", "DialogueTrigger")
    add_component_to_bp(BP_JOAQUIN, "/Script/ShadowTown.ShadowDialoguePlayerComponent", "DialoguePlayer")
    configure_sombra_npc(dt)
    configure_game_mode(widget_path)
    decorate_dialogue_plaza()
    log("Sprint 3 setup complete. Compile C++, then open WBP_DialogueHUD and bind UI.")


run()
