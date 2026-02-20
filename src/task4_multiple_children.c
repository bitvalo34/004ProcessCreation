#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
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

  pid_t kids[3]; /* save each child's PID so we can wait for them later */

  printf("[task4] Parent PID=%d creating 3 children...\n", (int)getpid());

  for (int i = 0; i < 3; i++) {
    pid_t pid = fork();
    if (pid < 0) die("fork");

    if (pid == 0) {
      printf("[task4] Child #%d started. PID=%d PPID=%d\n",
             i, (int)getpid(), (int)getppid());

      /* stagger the sleeps so they finish at different times */
      sleep_ms(300 + i * 250);

      printf("[task4] Child #%d done.\n", i);
      return 10 + i; /* each child exits with a unique code */
    }

    /* only the parent gets here — children return above */
    kids[i] = pid;
  }

  /* now reap all three children and print their exit codes */
  for (int i = 0; i < 3; i++) {
    int status = 0;
    pid_t w = waitpid(kids[i], &status, 0);
    if (w < 0) die("waitpid");

    if (WIFEXITED(status)) {
      printf("[task4] Reaped child PID=%d exit_code=%d\n", (int)w, WEXITSTATUS(status));
    } else {
      printf("[task4] Reaped child PID=%d (non-normal exit)\n", (int)w);
    }
  }

  printf("[task4] Parent done.\n");
  return 0;
}
