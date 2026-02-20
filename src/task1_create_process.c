#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

/* Quick helper so I don't have to repeat error handling everywhere. */
static void die(const char *msg) {
  perror(msg);
  exit(1);
}

/* nanosleep wrapper — I prefer milliseconds over whole seconds for demos. */
static void sleep_ms(long ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000L;
  nanosleep(&ts, NULL);
}

int main(void) {
  setbuf(stdout, NULL); /* disable buffering so prints show up immediately */

  printf("[task1] Parent starting. PID=%d\n", (int)getpid());

  /* fork() returns 0 to the child and the child's PID to the parent */
  pid_t pid = fork();
  if (pid < 0) die("fork");

  if (pid == 0) {
    /* I'm the child — prove it by printing my PID and my parent's PID */
    printf("[task1] Child here.  PID=%d  PPID=%d\n", (int)getpid(), (int)getppid());
    sleep_ms(700);
    printf("[task1] Child done.\n");
    return 0;
  }

  /* Back in the parent — the child is running concurrently at this point */
  printf("[task1] Parent after fork. MyPID=%d ChildPID=%d\n", (int)getpid(), (int)pid);
  sleep_ms(300);
  /* Not calling wait() here on purpose — that's what task2 is for. */
  printf("[task1] Parent done (not waiting in task1).\n");
  return 0;
}
