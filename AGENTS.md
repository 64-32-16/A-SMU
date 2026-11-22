# A-SMU GUI Development Rules

## State Ownership

- `SystemClass` is the single source of truth for domain values and settings.
- GUI controls and panels must read current domain values from `SystemClass` when displaying them.
- GUI actions must write changed domain values through `SystemClass` setters.
- Do not add redundant GUI mirror or pending variables for system values, including voltage range (`V-Range`) and current range (`I-Range`).
- Local UI state is allowed only for presentation and interaction mechanics, such as formatting, focus, touch handling, and text entry.
- See `system.md` for the project-level system rules.

## Project Documentation

- Keep project Markdown documentation in the repository root.
- See `Hardware.md` for components and the current GUI/simulation setup.
- SPI wiring is defined by the schematic; do not infer new pin assignments.

## Reusable Graphics Library - 2026-11-22

- Build reusable graphics functions through `GDClass`, with the goal of a powerful library for future projects.
- Controls and panels call `GDClass`; keep direct EVE/FT813 rendering calls and hardware-specific graphics details inside its implementation.
- Add general drawing features (such as rounded rectangles, configurable corner radii and gradients) to `GDClass` rather than duplicating them in controls or panels.
- Keep the graphics API independent of A-SMU domain logic, `SystemClass`, and project-specific layouts. Controls provide geometry, colors and other style parameters.

## Dialog Design Reference - 2026-11-22

- User decision: all dialogs follow the existing **SET LIMIT** dialog as the visual and interaction reference.
- Reuse its centered window, palette, readable value field, spacing and native EVE 3D buttons; preserve selected/disabled state colors.
- Resolve graphics-resource limits in GDClass/rendering rather than switching dialogs to a different visual style.

## Shared Graph Configuration - 2026-11-22

- Graph display preferences shared by MainPage and MENU/Graph belong centrally to SystemClass (`GraphSettings`), not to individual panels.
- Read shared preferences through GetGraphSettings and change them through SystemClass setters. Keep the settings representation independent of UI widgets so a versioned save/load format can be added later.
- Touch tracking and viewport gestures remain presentation state; do not reset manual zoom during ordinary redraws.


## Temporary Build Artifacts

- Write build logs to `tmp/logs/`, never to the repository root.
- Store disposable test outputs and temporary generated artifacts under `tmp/`.
- `tmp/` is ignored by Git. Keep maintained source files, tests and documentation outside it.
