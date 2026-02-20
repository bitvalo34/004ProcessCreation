#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void die(const char *msg) {
  perror(msg);
  exit(1);
}

int main(void) {
  setbuf(stdout, NULL);

  /* pipe() gives us two fds: pipefd[0] for reading, pipefd[1] for writing */
  int pipefd[2];
  if (pipe(pipefd) < 0) die("pipe");

  pid_t pid = fork();
  if (pid < 0) die("fork");

  if (pid == 0) {
    /*
     * Child side — we only read here, so close the write end.
     * If we don't close it, read() will never see EOF.
     */
    close(pipefd[1]);

    char buf[256];
    ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);
    if (n < 0) die("read");

    buf[n] = '\0'; /* read() doesn't null-terminate, so we do it ourselves */
    printf("[task3] Child received (%zd bytes): %s\n", n, buf);

    close(pipefd[0]);
    return 0;
  }

  /* Parent side — we only write, so close the read end */
  close(pipefd[0]);

  const char *msg = "Hello from parent (task3).";
  ssize_t sent = write(pipefd[1], msg, (ssize_t)strlen(msg));
  if (sent < 0) die("write");

  printf("[task3] Parent sent %zd bytes.\n", sent);

  close(pipefd[1]); /* done writing — this lets the child see EOF */

  int status = 0;
  waitpid(pid, &status, 0);
  printf("[task3] Parent done.\n");
  return 0;
}
