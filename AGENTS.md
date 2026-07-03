# A-SMU GUI Development Rules

## State Ownership

- `SystemClass` is the single source of truth for domain values and settings.
- GUI controls and panels must read current domain values from `SystemClass` when displaying them.
- GUI actions must write changed domain values through `SystemClass` setters.
- Do not add redundant GUI mirror or pending variables for system values, including voltage range (`V-Range`) and current range (`I-Range`).
- Local UI state is allowed only for presentation and interaction mechanics, such as formatting, focus, touch handling, and text entry.
- See `doc/system/system-rules.md` for the project-level system rules.
