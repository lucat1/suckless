/*
 * Copy me if you can.
 * by 20h
 */

#define _BSD_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>

static Display *dpy;

char *smprintf(char *fmt, ...) {
  va_list fmtargs;
  char *ret;
  int len;

  va_start(fmtargs, fmt);
  len = vsnprintf(NULL, 0, fmt, fmtargs);
  va_end(fmtargs);

  ret = malloc(++len);
  if (ret == NULL) {
    perror("malloc");
    exit(1);
  }

  va_start(fmtargs, fmt);
  vsnprintf(ret, len, fmt, fmtargs);
  va_end(fmtargs);

  return ret;
}

char *mktimes(char *fmt) {
  char buf[129];
  time_t tim;
  struct tm *timtm;

  tim = time(NULL);
  timtm = localtime(&tim);
  if (timtm == NULL)
    return smprintf("");

  if (!strftime(buf, sizeof(buf) - 1, fmt, timtm)) {
    fprintf(stderr, "strftime == 0\n");
    return smprintf("");
  }

  return smprintf("%s", buf);
}

void setstatus(char *str) {
  XStoreName(dpy, DefaultRootWindow(dpy), str);
  XSync(dpy, False);
}

char *readfile(char *base, char *file) {
  char *path, line[513];
  FILE *fd;

  memset(line, 0, sizeof(line));

  path = smprintf("%s/%s", base, file);
  fd = fopen(path, "r");
  if (fd == NULL) {
    printf("could not open file: %s", path);
    perror("fopen");
    exit(1);
  }
  free(path);

  if (fgets(line, sizeof(line) - 1, fd) == NULL) {
    perror("fgets");
    exit(1);
  }
  fclose(fd);

  return smprintf("%s", line);
}

char *getbattery(char *base) {
  char *co;
  int descap, remcap, cap;

  descap = -1;
  remcap = -1;
  cap = -1;

  co = readfile(base, "present");
  if (co[0] != '1') {
    free(co);
    return smprintf("not present");
  }
  free(co);

  co = readfile(base, "energy_full_design");
  sscanf(co, "%d", &descap);
  free(co);

  co = readfile(base, "energy_now");
  sscanf(co, "%d", &remcap);
  free(co);

  co = readfile(base, "capacity");
  sscanf(co, "%d", &cap);
  free(co);

  if (remcap < 0 || descap < 0)
    return smprintf("invalid");

  return smprintf("%d%% [%.0f%%]", cap, ((float)remcap / (float)descap) * 100);
}

int main(void) {
  char *status;
  char *bat;
  char *tz;

  if (!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "dwmstatus: cannot open display.\n");
    return 1;
  }

  for (;; sleep(60)) {
    bat = getbattery("/sys/class/power_supply/BAT0");
    tz = mktimes("%H:%M"); // %d of %b

    status = smprintf("%s    %s ", bat, tz);
    setstatus(status);

    free(bat);
    free(tz);
    free(status);
  }

  XCloseDisplay(dpy);

  return 0;
}
