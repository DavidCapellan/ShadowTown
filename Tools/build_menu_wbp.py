"""
Crea WBP_InteractionMenu con botones anclados a pantalla vía ue-mcp bridge handlers.
Usa add_widget en secuencia.
"""
import unreal
import json
import subprocess
import sys

BRIDGE = r"C:\UE4\ShadowTown\Tools\ue_bridge_call.py"
WBP = "/Game/_Game/UI/WBP_InteractionMenu.WBP_InteractionMenu"


def call(method: str, params: dict) -> dict:
    import tempfile
    import os
    fd, path = tempfile.mkstemp(suffix=".json")
    os.close(fd)
    with open(path, "w", encoding="utf-8") as f:
        json.dump(params, f)
    result = subprocess.run(
        [sys.executable, BRIDGE, method, path],
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    os.unlink(path)
    if result.returncode != 0:
        unreal.log_error(result.stderr)
        return {}
    try:
        return json.loads(result.stdout)
    except json.JSONDecodeError:
        unreal.log_error(result.stdout)
        return {}


def log(msg: str) -> None:
    unreal.log(f"[BuildMenu] {msg}")


def run() -> None:
    log("Creando WBP_InteractionMenu...")

    if unreal.EditorAssetLibrary.does_asset_exist(WBP.rsplit(".", 1)[0]):
        unreal.EditorAssetLibrary.delete_asset(WBP.rsplit(".", 1)[0])

    r = call(
        "create_widget_blueprint",
        {
            "packagePath": "/Game/_Game/UI",
            "name": "WBP_InteractionMenu",
            "parentClass": "UserWidget",
        },
    )
    if not r.get("result", {}).get("success"):
        log(f"create failed: {r}")
        return

  # Root canvas
    call("add_widget", {"assetPath": WBP, "widgetClass": "CanvasPanel", "widgetName": "RootCanvas"})
    call("set_root", {"assetPath": WBP, "widgetName": "RootCanvas"})

    call(
        "add_widget",
        {
            "assetPath": WBP,
            "widgetClass": "Overlay",
            "widgetName": "ScreenOverlay",
            "parentWidgetName": "RootCanvas",
        },
    )
    call(
        "add_widget",
        {
            "assetPath": WBP,
            "widgetClass": "Border",
            "widgetName": "Dim",
            "parentWidgetName": "ScreenOverlay",
        },
    )
    call(
        "set_widget_property",
        {
            "assetPath": WBP,
            "widgetName": "Dim",
            "propertyName": "BrushColor",
            "value": {"r": 0, "g": 0, "b": 0, "a": 0.45},
        },
    )

    call(
        "add_widget",
        {
            "assetPath": WBP,
            "widgetClass": "CanvasPanel",
            "widgetName": "MenuCanvas",
            "parentWidgetName": "ScreenOverlay",
        },
    )

    buttons = [
        ("BtnResponder", "Responder", 0, -220),
        ("BtnCallarse", "Callarse", -130, -150),
        ("BtnIgnorar", "Ignorar", 130, -150),
        ("BtnSalir", "Salir", 0, -80),
    ]
    for name, label, ox, oy in buttons:
        call(
            "add_widget",
            {
                "assetPath": WBP,
                "widgetClass": "Button",
                "widgetName": name,
                "parentWidgetName": "MenuCanvas",
            },
        )
        call(
            "add_widget",
            {
                "assetPath": WBP,
                "widgetClass": "TextBlock",
                "widgetName": name + "_Text",
                "parentWidgetName": name,
            },
        )
        call(
            "set_widget_property",
            {
                "assetPath": WBP,
                "widgetName": name + "_Text",
                "propertyName": "Text",
                "value": label,
            },
        )
        call(
            "set_widget_property",
            {
                "assetPath": WBP,
                "widgetName": name,
                "propertyName": "Slot.Anchors",
                "value": {"minimum": {"x": 0.5, "y": 1}, "maximum": {"x": 0.5, "y": 1}},
            },
        )

    call("compile_blueprint", {"assetPath": WBP})
    log(f"Widget creado: {WBP}")


run()
