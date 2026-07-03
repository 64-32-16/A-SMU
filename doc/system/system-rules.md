# A-SMU System Rules

## State Ownership

- `SystemClass` is the single source of truth for domain values and settings.
- GUI controls and panels read current domain values from `SystemClass` when displaying them.
- GUI actions write changed domain values through `SystemClass` setters.
- System changes may be triggered by different frontends, such as GUI, SCPI, hardware controls, startup defaults, or automation.
- All frontends must route domain changes through `SystemClass` or system-level APIs that enforce the same rules.
- Domain rules must not live only in GUI callbacks.
- GUI code must not add redundant mirror or pending variables for domain values.
- Local UI state is allowed only for presentation and interaction mechanics, such as formatting, focus, touch handling, text entry, temporary text buffers, and display format state.

## Modes And Ranges

- Source mode is changed through `System.SetSourceMode(...)`.
- Measure mode is changed through `System.SetMeasureMode(...)`.
- Voltage range, current range, and resistance range are owned by `SystemClass`.
- Range buttons display values read from `SystemClass`.
- Range dialogs write changes through the matching `SystemClass` setters.

## Auto Range Mode

- Voltage and current range each have a range mode: `AUTO` or `MANUAL`.
- Voltage and current range each have an actual range value.
- Voltage range values are `5 V` and `30 V`.
- Current range values are `100 mA` and `1 A`.
- When voltage range mode is `AUTO`, `SystemClass` may switch the voltage range value up or down to fit a requested source, limit, or measure value.
- When current range mode is `AUTO`, `SystemClass` may switch the current range value up or down to fit a requested source, limit, or measure value.
- This automatic range switch happens only while the corresponding range mode is `AUTO`.
- In `MANUAL` range mode, requested source, limit, or measure values must not change the selected range.
- Number pad validation and `SystemClass` setters must reject source or limit values outside the currently allowed range.
- In `AUTO` range mode, the allowed source or limit input range is the largest available range for that quantity.
- In `MANUAL` range mode, the allowed source or limit input range is the selected fixed range.

## GUI Dialog Boundaries

- Source mode and measure mode are separate dialogs.
- The source mode dialog changes only source mode.
- The measure mode dialog changes only measure mode.
- Range dialogs change only the range they are responsible for.

## Display Formatting

- Panels may keep local buffers for rendered text.
- Panels may keep local formatting state for stable display formatting.
- These buffers and formatting states are not domain state and must not become an alternate source of truth.
