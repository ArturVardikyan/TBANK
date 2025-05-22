#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"

PORT=23456

# 1) Запускаем сервер в фоне
"$BUILD_DIR/server" 3 1000 "$PORT" &
SERVER_PID=$!

# Даем серверу подняться
sleep 1

# 2) Прогоняем socket_client и сохраняем вывод
"$BUILD_DIR/socket_client" localhost "$PORT" <<EOF > socket_out.txt
help
transfer 0 1 200
show_balance 0
show_balance 1
freeze 2
show_account_list
shutdown
EOF

# 3) Ожидаем завершения сервера
wait $SERVER_PID

# 4) Сравниваем с эталоном
diff -u socket_expected.txt socket_out.txt
