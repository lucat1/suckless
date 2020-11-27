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

char *tzrome = "Europe/Rome";

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

void settz(char *tzname) { setenv("TZ", tzname, 1); }

char *mktimes(char *fmt, char *tzname) {
  char buf[129];
  time_t tim;
  struct tm *timtm;

  settz(tzname);
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

char *loadavg(void) {
  double avgs[3];

  if (getloadavg(avgs, 1) < 0)
    return smprintf("");

  return smprintf("%.2f ", avgs[0]);
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
  int descap, remcap;

  descap = -1;
  remcap = -1;

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

  if (remcap < 0 || descap < 0)
    return smprintf("invalid");

  return smprintf("%.0f", ((float)remcap / (float)descap) * 100);
}

char *gettemperature(char *base, char *sensor) {
  char *co;

  co = readfile(base, sensor);
  if (co == NULL)
    return smprintf("");
  return smprintf("%02.0f°C", atof(co) / 1000);
}

int main(void) {
  char *status;
  char *avgs;
  char *bat;
  char *tmbln;

  if (!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "dwmstatus: cannot open display.\n");
    return 1;
  }

  for (;; sleep(60)) {
    avgs = loadavg();
    bat = getbattery("/sys/class/power_supply/BAT1");
    tmbln = mktimes("%d %b %H:%M", tzrome);

    status = smprintf("L:%s B:%s %s", avgs, bat, tmbln);
    setstatus(status);

    free(avgs);
    free(bat);
    free(tmbln);
    free(status);
  }

  XCloseDisplay(dpy);

  return 0;
}
