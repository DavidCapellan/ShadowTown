import unreal

ar = unreal.AssetRegistryHelpers.get_asset_registry()
for data in ar.get_assets_by_path("/Game", recursive=True):
    if str(data.asset_class_path.asset_name) not in ("Blueprint", "WidgetBlueprint", "BlueprintMacroLibrary"):
        continue
    path = f"{data.package_name}.{data.asset_name}"
    bp = unreal.load_asset(path)
    if not bp:
        continue
    hits = []
    for graph in bp.get_editor_property("ubergraph_pages") or []:
        for node in graph.nodes:
            try:
                title = node.get_node_title(unreal.NodeTitleType.FULL_TITLE).to_string().lower()
            except Exception:
                title = node.get_class().get_name().lower()
            if any(k in title for k in ("dialog", "dialogue", "text block", "subtitles", "line", "speak", "talk", "open level", "combat", "datatable", "csv", "widget")):
                hits.append(f"{graph.get_name()}: {title[:80]}")
    if hits:
        unreal.log(f"HIT {path}")
        for h in hits[:12]:
            unreal.log(f"  {h}")
