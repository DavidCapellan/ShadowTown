# 🚀 Sprint 3: ShadowTown - Entrega de Prototipo

Este documento detalla los avances y la entrega final del Sprint 3 para el proyecto **ShadowTown**. El objetivo principal ha sido consolidar la jugabilidad de principio a fin, pulir la estética y asegurar que el escenario de diálogos y la exploración sean funcionales.

---

## 📑 Checklist de Sprint 3

- [x] **Sprint 1: Mecánicas y Assets**: Finalización de la lógica central (Piedra, Papel o Tijera) y assets principales.
- [x] **Escenario de Diálogos**: Implementación de diálogos para NPCs (Sombra) basados en los guiones.
- [x] **Parque Navegable**: Creación y optimización del entorno de exploración del parque.
- [x] **Flujo de Juego Completo**: Conexión entre la exploración, el encuentro con NPCs y el combate.
- [x] **Presentación**: Preparación de capturas y documentación para la muestra del juego.

---

## 🎭 Escenario de Diálogos

Se ha configurado el sistema de diálogos para que los NPCs interactúen con Joaquín (el protagonista) de forma dinámica. Los diálogos están basados en los archivos de guion encontrados en `Content\Guiones y Audios\Guiones\Batalla\NPC`.

### Vista Previa del Sistema de Diálogos
![Preview de Diálogos](shadowtown_dialogue_preview_1778836886675.png)

> [!NOTE]
> Los diálogos incluyen variaciones para personajes masculinos y femeninos, con acento de Toledo, reforzando la ambientación local.

---

## 🌳 Parque Navegable y Exploración

El mapa principal (`Map_Circuito_Final`) ha sido decorado y optimizado para una navegación fluida. Se han añadido rutas de patrulla para los NPCs y puntos de interés para el jugador.

### Vista Previa del Parque
![Preview del Parque](shadowtown_park_preview_1778836904278.png)

> [!TIP]
> Se recomienda el uso del **Gameplay State Tree** para gestionar el comportamiento de los NPCs durante la exploración, permitiendo transiciones suaves entre el patrullaje y el estado de alerta cuando Joaquín se acerca.

---

## 🕹️ Flujo de Juego (Start to Finish)

El prototipo es ahora jugable siguiendo este flujo:

1. **Inicio**: El jugador comienza en el parque (`Map_Circuito_Final`).
2. **Exploración**: Joaquín debe navegar por el parque, evitando o buscando a los NPCs "Sombra".
3. **Interacción**: Al acercarse a un NPC, se activa el sistema de diálogo interactivo.
4. **Combate**: Tras el diálogo, la escena transiciona al `Mapa_Combate` donde se ejecuta la mecánica de **Piedra, Papel o Tijera**.
5. **Resolución**: Dependiendo del resultado, el juego muestra la victoria o derrota y regresa al mapa principal para continuar la exploración.

---

## 🎮 Sistema de diálogos (Sprint 3 — movimiento y animación)

### Comportamiento implementado

| Actor | Durante el diálogo |
|--------|-------------------|
| **Joaquín (jugador)** | No puede moverse (`DisableMovement` + ignorar input de movimiento). Avanza líneas con **Espacio** o **E**. |
| **NPC Sombra** | Reproduce animación de habla en bucle (`Lina2Armature_Speak` / `Exra3Armature_Talk`). |
| **Cámara** | Sigue activa (solo se bloquea el desplazamiento). |

### Módulo C++ `ShadowTown` (en `Source/ShadowTown/`)

- `UShadowDialogueSubsystem` — orquesta líneas, UI y bloqueo de movimiento.
- `UShadowDialogueNpcComponent` — esfera de interacción en el NPC.
- `UShadowDialogueBlueprintLibrary` — nodos Blueprint: `StartNpcDialogue`, `AdvanceActiveDialogue`, `IsDialogueActive`.
- Datos: `Content/_Game/DialogueData.csv` → importar como `DT_DialogueData`.

### Escenario en `Map_Circuito_Final`

- Colocados: **Joaquin_Player**, **NPC_Sombra_Dialogo**, luces cálidas de la plaza.
- `BP_Sombra_NPC`: componente **DialogueTrigger** (esfera 220 uu).

### Compilar (obligatorio una vez)

1. Cierra el editor de Unreal (Live Coding bloquea UBT).
2. Ejecuta:
   ```
   "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" ShadowTownEditor Win64 Development -Project="C:\UE4\ShadowTown\ShadowTown.uproject" -WaitMutex
   ```
3. Abre el proyecto, compila el plugin `UE_MCP_Bridge` si lo pide, y ejecuta `Tools/sprint3_setup.py` vía **execute_python** o el asistente MCP.

### Cableado en `BP_Sombra_NPC` (Event Graph)

1. **On Component Begin Overlap** (`DialogueTrigger`) → **Cast to BP_Joaquin_Player**.
2. **Start Npc Dialogue** (`ShadowDialogueBlueprintLibrary`): tabla `DT_DialogueData`, tipo `PreCombate`, género `Female`, animación `Lina2Armature_Speak`.

### Presentación

- Capturas: plaza con luces, diálogo en pantalla, NPC animándose con Joaquín quieto.
- Vídeo: acercarse al NPC → diálogo → avanzar con Espacio → salir al combate.

## 📈 Próximos Pasos

1. **Equilibrio de Dificultad**: Ajustar la probabilidad de éxito de los NPCs en niveles avanzados.
2. **Feedback Visual**: Añadir efectos de partículas (VFX) durante el combate para mayor impacto.
3. **Grabación de Demo**: Utilizar el Unreal Sequencer para capturar cinemáticas del parque para la presentación final.

---

> [!IMPORTANT]
> El proyecto está listo para ser empaquetado (Build) y presentado a los tutores. Se han verificado las referencias de los mapas y el correcto funcionamiento del GameMode `GM_ShadowTown`.
