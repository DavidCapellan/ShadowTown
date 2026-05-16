"""
Conecta el menú C++ de interacción y el bloqueo WASD.
Ejecutar en el editor con execute_python tras compilar ShadowTown.
"""
import unreal

BP_JOAQUIN = "/Game/_Game/Blueprints/BP_Joaquin_Player"
BP_SOMBRA = "/Game/_Game/Blueprints/BP_Sombra_NPC"


def log(msg: str) -> None:
    unreal.log(f"[WireMenu] {msg}")


def add_lock_component(bp_path: str) -> None:
    comp_class = unreal.load_class(None, "/Script/ShadowTown.ShadowMovementLockComponent")
    if not comp_class:
        log("Compila el módulo ShadowTown primero.")
        return
    bp = unreal.load_asset(bp_path)
    if not bp:
        return
    unreal.BlueprintEditorLibrary.add_component_to_blueprint(bp, comp_class, "MovementLock")
    unreal.EditorAssetLibrary.save_asset(bp_path)
    log(f"MovementLockComponent añadido a {bp_path}")


def run() -> None:
    log("Cableado de menú de interacción...")
    add_lock_component(BP_JOAQUIN)
    log(
        "En BP_Sombra_NPC, al abrir el menú usa:\n"
        "  ShadowPlayerInteractionLibrary -> Show Interaction Menu (Player, Self)\n"
        "Y elimina/oculta el widget 3D antiguo.\n"
        "Al cerrar (Salir):\n"
        "  ShadowPlayerInteractionLibrary -> Hide Interaction Menu"
    )


run()
