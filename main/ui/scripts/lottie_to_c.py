#!/usr/bin/env python3
"""Compile the Lottie JSON files of custom/lottie_assets into C arrays.

    python3 scripts/lottie_to_c.py

Writes custom/lottie_assets/<name>.c next to each <name>.json. The arrays are
NUL terminated, as LVGL's scripts/filetohex.py --null-terminate makes them, and
the size includes the terminator. Register a new animation in lottie_assets.c.
"""

from pathlib import Path

ASSETS = Path(__file__).resolve().parent.parent / "custom" / "lottie_assets"

for json_path in sorted(ASSETS.glob("*.json")):
    name = json_path.stem
    data = json_path.read_bytes() + b"\0"
    rows = ",\n".join(
        "    " + ", ".join(f"0x{b:02x}" for b in data[i:i + 16]) for i in range(0, len(data), 16)
    )
    json_path.with_suffix(".c").write_text(
        f"/* Generated from {json_path.name} by scripts/lottie_to_c.py; do not edit. */\n\n"
        "#include <stddef.h>\n"
        "#include <stdint.h>\n\n"
        f"const uint8_t {name}_json[] = {{\n{rows}\n}};\n\n"
        f"const size_t {name}_json_size = sizeof({name}_json);\n"
    )
    print(f"{json_path.name} -> {name}.c ({len(data)} bytes)")
