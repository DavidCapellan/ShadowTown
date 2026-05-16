import unreal

KEYWORDS = ("dialog", "dialogue", "talk", "speak", "interact", "move", "input", "lock", "freeze", "combat", "hud", "widget", "text", "line", "npc", "sombra")

def log(msg: str) -> None:
    unreal.log(msg)

ar = unreal.AssetRegistryHelpers.get_asset_registry()

log("--- Blueprints under /Game/_Game ---")
for data in ar.get_assets_by_path("/Game/_Game", recursive=True):
    name = str(data.asset_name)
    cls = str(data.asset_class_path.asset_name)
    pkg = str(data.package_name)
    blob = f"{name} {cls} {pkg}".lower()
    if cls in ("Blueprint", "WidgetBlueprint") or "wbp" in name.lower():
        if any(k in blob for k in KEYWORDS) or "wbp" in name.lower() or "dialog" in name.lower():
            log(f"ASSET: {pkg} [{cls}]")

log("--- Functions in Joaquin / Sombra / GM ---")
for path in [
    "/Game/_Game/Blueprints/BP_Joaquin_Player.BP_Joaquin_Player",
    "/Game/_Game/Blueprints/BP_Sombra_NPC.BP_Sombra_NPC",
    "/Game/_Game/Blueprints/GM_ShadowTown.GM_ShadowTown",
]:
    bp = unreal.load_asset(path)
    if not bp:
        log(f"MISSING {path}")
        continue
    log(f"== {path} ==")
    for graph in bp.get_editor_property("ubergraph_pages") or []:
        gname = graph.get_name()
        for node in graph.nodes:
            title = ""
            try:
                title = node.get_node_title(unreal.NodeTitleType.FULL_TITLE).to_string()
            except Exception:
                title = node.get_class().get_name()
            tlow = title.lower()
            if any(k in tlow for k in KEYWORDS):
                log(f"  [{gname}] {node.get_class().get_name()}: {title}")
