# Lab004 — Process Creation and Management (WSL / Linux)

This lab implements small C programs to practice:
- Process creation
- Parent/child synchronization
- IPC using pipes
- Multiple child management
- Shared memory (System V IPC)

## Objective / Expected Behavior

- Task 1: Create a child process and print parent/child IDs.
- Task 2: Synchronize parent and child (parent waits for child).
- Task 3: IPC with a pipe (parent writes, child reads).
- Task 4: Spawn 3 children and properly wait/reap them.
- Task 5: Shared memory: parent writes, child reads, then cleanup.

## Project Structure

- `src/`
  - `task1_create_process.c`
  - `task2_sync_wait.c`
  - `task3_pipe_ipc.c`
  - `task4_multiple_children.c`
  - `task5_shared_memory.c`
- `Makefile`  
  Build, run, smoke tests, stress tests, and log capture.
- `bin/`  
  Compiled binaries (generated).
- `logs/`  
  Execution logs (generated with `make capture-logs`).

## How It Works

### Task 1 — Process creation (fork)

- `fork()` duplicates the calling process.
- After `fork()`, both parent and child continue execution in separate processes.
- We print:
  - parent PID
  - child PID
  - child PPID (should match parent PID)

### Task 2 — Synchronization (waitpid)

- Parent calls `waitpid(child_pid, &status, 0)` to block until the child finishes.
- We use `WIFEXITED(status)` and `WEXITSTATUS(status)` to read the child exit code.
- Expected behavior: "Parent done" prints after the child exits.

### Task 3 — IPC with a pipe (parent -> child)

- `pipe(pipefd)` gives two file descriptors:
  - `pipefd[0]` read end
  - `pipefd[1]` write end
- Parent closes the read end and writes a message to the write end.
- Child closes the write end and reads from the read end.
- Closing the unused ends is important so the reader can detect EOF correctly.

### Task 4 — Multiple children + reaping

- Parent spawns 3 children in a loop.
- Each child prints its index and exits with a unique code.
- Parent waits for each PID with `waitpid` and prints the exit codes.
- Expected behavior: exactly 3 "Reaped child ..." lines.

### Task 5 — Shared memory (System V IPC)

- Parent creates shared memory with `shmget`.
- Parent attaches (`shmat`), writes a string, then waits.
- Child attaches and reads the same bytes.
- Parent detaches and calls `shmctl(..., IPC_RMID, ...)` to mark the segment for deletion.

## Build (WSL)

Requirements:
- WSL Ubuntu
- `gcc`, `make`, and `timeout` (coreutils)

Build all:
```bash
make all
```
### Run

Run any task:

```bash
make run1
make run2
make run3
make run4
make run5
```

### Tests

Smoke tests (fast checks):

```bash
make test
```

Stress test example (run Task 4 multiple times):

```bash
make stress4 N=5
```

### Execution Logs (Deliverable)

Generate logs:

```bash
make capture-logs
```

Outputs:

- `logs/task1.txt`
- `logs/task2.txt`
- `logs/task3.txt`
- `logs/task4.txt`
- `logs/task5.txt`

### Notes / Troubleshooting

- If a pipe program ever hangs, it's usually because one process forgot to close the unused pipe ends.
- If shared memory remains after a crash, you can inspect with `ipcs -m` and remove with `ipcrm -m <shmid>`.

