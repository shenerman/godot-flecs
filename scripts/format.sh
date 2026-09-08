#!/usr/bin/env bash
# 格式化/检查 src/ 下的自有源文件；thirdparty 和 godot-cpp 永不进入
# 用法: scripts/format.sh          格式化
#       scripts/format.sh --check  只检查(CI/hook 用)
set -euo pipefail
cd "$(dirname "$0")/.."

FILES=$(find src \( -name '*.hpp' -o -name '*.cpp' \) -type f 2>/dev/null || true)
[ -z "$FILES" ] && { echo "no source files"; exit 0; }

if [ "${1:-}" = "--check" ]; then
    echo "$FILES" | xargs clang-format --dry-run --Werror
else
    echo "$FILES" | xargs clang-format -i
    echo "formatted $(echo "$FILES" | wc -l) files"
fi
