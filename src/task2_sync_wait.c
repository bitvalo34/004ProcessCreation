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

  printf("[task2] Parent starting. PID=%d\n", (int)getpid());

  pid_t pid = fork();
  if (pid < 0) die("fork");

  if (pid == 0) {
    printf("[task2] Child working... PID=%d\n", (int)getpid());
    sleep_ms(600);
    printf("[task2] Child exiting with code 42.\n");
    return 42;
  }

  printf("[task2] Parent waiting for child PID=%d ...\n", (int)pid);

  int status = 0;
  pid_t w = waitpid(pid, &status, 0);
  if (w < 0) die("waitpid");

  if (WIFEXITED(status)) {
    int code = WEXITSTATUS(status);
    printf("[task2] Child finished normally. exit_code=%d\n", code);
  } else {
    printf("[task2] Child did not exit normally.\n");
  }

  printf("[task2] Parent done.\n");
  return 0;
}
