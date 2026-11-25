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


## RAM sparen / Measurement Buffers - 2026-11-24

- RP2040 RAM is limited: keep one authoritative measurement ring buffer in SystemClass.
- Live graphs, histograms and other live views must read through a non-owning chronological BufferView. Do not add full measurement-buffer copies to panels/pages, heap storage or temporary stack arrays.
- BufferView owns no samples and is valid only until the source buffer changes. Consume it synchronously. The acquisition worker writes only its bounded transport FIFO; SystemClass publishes into the history on the main loop before rendering. Never write the history from the worker.
- Frozen UI data must have an explicit, small bound. The Reading Table may keep one 50-sample page cache containing its visible rows; no full-history HOLD snapshot.
- Preserve shared graphics work buffers. Check the Pico build's RAM use after changes involving buffers or persistent UI state, and report meaningful changes.


## Keine redundanten Daten / Single Source of Truth - 2026-11-24

- Define each setting, capacity and limit once at its owning layer. Derive labels, validation, allocation sizes and dependent values from that source; do not repeat numeric literals or maintain redundant copies.
- Measurement capacity is defined only by BufferClass::Capacity in src/Core/Buffer.h. UI descriptions use GetCapacity()/Capacity; tests derive ring-fill counts, wraparound expectations and offsets from Capacity.
- The table page cache and XY display limit have separate purposes and remain independently bounded; they are not duplicate definitions of the measurement capacity.
- Historical build measurements in documentation are dated observations, not configuration. After a capacity change, run host tests and the Pico build and check RAM headroom and rendering cost.


## Page Navigation Placement - 2026-11-25

- User preference: place page navigation buttons (such as BACK and HOME) at the top whenever the layout permits. Apply this convention to new or revised pages; keep the title and content clear of the buttons.


## Menu Page Style Guide - 2026-11-25

- Read and follow [StyleGuide.md](StyleGuide.md) when creating or revising menu destination pages. System Settings is the user-selected visual reference, with navigation at the top.
- Reuse src/UI/Pages/MenuPageStyle.h for the shared header, navigation and form layout; Info follows the same template. Labels and values use the same font size.


## Reuse Existing Dialogs - 2026-11-25

- Before adding an editor/dialog, reuse the existing implementation and shared instance where possible. No page-specific duplicate range selection logic.
- Measure Settings and the main page open the same SourceRangePopup instance via application wiring; only the NPLC editor belongs to Measure Settings.
