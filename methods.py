import os
import sys
from pathlib import Path

from enum import Enum

# Colors are disabled in non-TTY environments such as pipes. This means
# that if output is redirected to a file, it won't contain color codes.
# Colors are always enabled on continuous integration.
_colorize = bool(sys.stdout.isatty() or os.environ.get("CI"))


class ANSI(Enum):
    """
    Enum class for adding ansi colorcodes directly into strings.
    Automatically converts values to strings representing their internal value,
    or an empty string in a non-colorized scope.
    """

    RESET = "\x1b[0m"
    BOLD = "\x1b[1m"
    ITALIC = "\x1b[3m"
    UNDERLINE = "\x1b[4m"
    STRIKETHROUGH = "\x1b[9m"
    REGULAR = "\x1b[22;23;24;29m"

    BLACK = "\x1b[30m"
    RED = "\x1b[31m"
    GREEN = "\x1b[32m"
    YELLOW = "\x1b[33m"
    BLUE = "\x1b[34m"
    MAGENTA = "\x1b[35m"
    CYAN = "\x1b[36m"
    WHITE = "\x1b[37m"

    PURPLE = "\x1b[38;5;93m"
    PINK = "\x1b[38;5;206m"
    ORANGE = "\x1b[38;5;214m"
    GRAY = "\x1b[38;5;244m"

    def __str__(self) -> str:
        global _colorize
        return str(self.value) if _colorize else ""


def print_warning(*values: object) -> None:
    """Prints a warning message with formatting."""
    print(f"{ANSI.YELLOW}{ANSI.BOLD}WARNING:{ANSI.REGULAR}", *values, ANSI.RESET, file=sys.stderr)


def print_error(*values: object) -> None:
    """Prints an error message with formatting."""
    print(f"{ANSI.RED}{ANSI.BOLD}ERROR:{ANSI.REGULAR}", *values, ANSI.RESET, file=sys.stderr)


# Get the project folder name ahead of time  # [MODIFIED] 参考 Godot：模块顶层定位工程根
base_folder = Path(__file__).resolve().parent
BUILD_DIR_NAME = "build"


def redirect_emitter(target, source, env):
    """
    Emitter to automatically redirect object build files to the `build/` directory,
    retaining subfolder structure (src/foo.cpp -> build/src/foo.obj).
    If `redirect_build_objects` is `False`, or the target already lives under
    `build/` (e.g. flecs objects declared explicitly in their SConscript),
    this emitter does nothing — making it idempotent.
    """
    if not env["redirect_build_objects"]:
        return target, source

    redirected_targets = []
    for item in target:
        path = Path(item.get_abspath()).resolve()
        try:
            rel = path.relative_to(base_folder)
        except ValueError:
            # 项目外路径：无法安全重写，保留原样并提示
            print_warning(f'Failed to redirect "{path}"')
            redirected_targets.append(item)
            continue

        if rel.parts[0] == BUILD_DIR_NAME:
            # [MODIFIED] 幂等保护：已位于 build/ 下的显式 target（如 flecs）不动，
            # 避免 build/obj/build/... 这类二次重定向
            pass
        else:
            # src/example_class.cpp -> build/src/example_class.obj
            item = env.File(f"#build/{rel.as_posix()}")
        redirected_targets.append(item)
    return redirected_targets, source

