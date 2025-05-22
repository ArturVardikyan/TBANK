#!/usr/bin/env bash
set -euo pipefail

# Определяем корень проекта и папку сборки
ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"

SHM_NAME="/TBANK_TEST"
N=3
MAX=1000

# 1) Удаляем старый сегмент (на случай предыдущего падения)
"$BUILD_DIR/deinitializer" "$SHM_NAME" || true

# 2) Инициализируем
"$BUILD_DIR/initializer" "$SHM_NAME" "$N" "$MAX"

# 3) Прогоняем client (shared-memory) и сохраняем вывод
"$BUILD_DIR/client" "$SHM_NAME" "$N" <<EOF > shared_out.txt
show_account_list
transfer 0 1 500
show_balance 0
show_balance 1
mass_update -100
show_account_list
exit
EOF

# 4) Удаляем сегмент
"$BUILD_DIR/deinitializer" "$SHM_NAME"

# 5) Сравниваем с эталоном
diff -u shared_expected.txt shared_out.txt
