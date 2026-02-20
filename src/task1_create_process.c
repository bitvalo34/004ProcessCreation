#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

static void die(const char *msg) {
  perror(msg);
  exit(1);
}

static void sleep_ms(long ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000L;
  nanosleep(&ts, NULL);
}

int main(void) {
  // Makes output predictable even when piping to files.
  setbuf(stdout, NULL);

  printf("[task1] Parent starting. PID=%d\n", (int)getpid());

  pid_t pid = fork();
  if (pid < 0) die("fork");

  if (pid == 0) {
    // Child
    printf("[task1] Child here.  PID=%d  PPID=%d\n", (int)getpid(), (int)getppid());
    sleep_ms(700);
    printf("[task1] Child done.\n");
    return 0;
  }

  // Parent
  printf("[task1] Parent after fork. MyPID=%d ChildPID=%d\n", (int)getpid(), (int)pid);
  sleep_ms(300);
  printf("[task1] Parent done (not waiting in task1).\n");
  return 0;
}
