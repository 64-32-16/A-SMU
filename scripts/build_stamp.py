"""Generate the firmware build timestamp for the Info page on each build."""
from datetime import datetime, timezone
from pathlib import Path

Import("env")
generated = Path(env.subst("$PROJECT_DIR")) / "tmp" / "generated"
generated.mkdir(parents=True, exist_ok=True)
stamp = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC")
(generated / "BuildInfo.h").write_text(
    '#pragma once\n#define ASMU_BUILD_TIMESTAMP "' + stamp + '"\n', encoding="ascii")
env.Append(CPPPATH=[str(generated)])
