#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

static void die(const char *msg) {
  perror(msg);
  exit(1);
}

static void sleep_ms(long ms) {
  struct timespec ts = { ms / 1000, (ms % 1000) * 1000000L };
  nanosleep(&ts, NULL);
}

int main(void) {
  setbuf(stdout, NULL);

  const size_t SIZE = 256;

  /*
   * IPC_PRIVATE gives us a unique key — works fine since
   * only our fork'd child needs access to this segment.
   */
  int shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | 0600);
  if (shmid < 0) die("shmget");

  printf("[task5] Created shared memory. shmid=%d\n", shmid);

  pid_t pid = fork();
  if (pid < 0) die("fork");

  if (pid == 0) {
    /* child attaches to the same shared memory segment */
    void *addr = shmat(shmid, NULL, 0);
    if (addr == (void *)-1) die("shmat(child)");

    /* give the parent a moment to write first (not bulletproof, but fine for a demo) */
    sleep_ms(200);

    char local[256];
    strncpy(local, (const char *)addr, sizeof(local) - 1);
    local[sizeof(local) - 1] = '\0';

    printf("[task5] Child read: \"%s\"\n", local);

    if (shmdt(addr) < 0) die("shmdt(child)");
    return 0;
  }

  /* parent attaches and writes the message into shared memory */
  void *addr = shmat(shmid, NULL, 0);
  if (addr == (void *)-1) die("shmat(parent)");

  const char *msg = "Shared hello from parent (task5).";
  snprintf((char *)addr, SIZE, "%s", msg);
  printf("[task5] Parent wrote message.\n");

  /* wait for the child to finish reading before we clean up */
  int status = 0;
  waitpid(pid, &status, 0);

  if (shmdt(addr) < 0) die("shmdt(parent)");

  /* IPC_RMID marks it for deletion — the kernel frees it after the last detach */
  if (shmctl(shmid, IPC_RMID, NULL) < 0) die("shmctl(IPC_RMID)");
  printf("[task5] Parent marked segment for deletion.\n");

  return 0;
}
