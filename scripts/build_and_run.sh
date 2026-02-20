#!/usr/bin/env bash
set -e

if [ $# -lt 1 ]; then
  echo "Usage: ./build_and_run.sh <task_number 1..5>"
  exit 1
fi

TASK="$1"
CFLAGS="-std=c11 -Wall -Wextra -O0 -g"

case "$TASK" in
  1) SRC="task1_create_process.c"; OUT="task1" ;;
  2) SRC="task2_sync_wait.c";     OUT="task2" ;;
  3) SRC="task3_pipe_ipc.c";      OUT="task3" ;;
  4) SRC="task4_multiple_children.c"; OUT="task4" ;;
  5) SRC="task5_shared_memory.c"; OUT="task5" ;;
  *) echo "Task must be 1..5"; exit 1 ;;
esac

echo "==> Building $SRC -> $OUT"
gcc $CFLAGS "$SRC" -o "$OUT"

echo "==> Running ./$OUT"
./"$OUT"
