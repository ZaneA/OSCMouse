/*
 * OSC Mouse Movement Server
 * Designed to be used with TouchOSC
 * By Zane Ashby
 */

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include <lo/lo.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

int x = 0;
int y = 0;
int width = 100;
int height = 100;

int main(int argc, char **argv)
{
  Display *dpy;

  if (argc < 3) {
    fprintf(stderr, "Usage: %s <screen-width> <screen-height>\n", argv[0]);
    exit(1);
  }

  width = atoi(argv[1]);
  height = atoi(argv[2]);

  if (!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "Could not open display %s", getenv("DISPLAY"));
  }

  lo_server_thread st = lo_server_thread_new("8000", [](int num, const char *msg, const char *path) {
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
  });

  lo_server_thread_add_method(st, "/4/xy", "ff", [](const char *path, const char *types, lo_arg **argv, int argc, void *data, void *dpy) -> int {
    x = (int)(argv[1]->f * width);
    y = height - (int)(argv[0]->f * height);

    XWarpPointer((Display*)dpy, None, RootWindow((Display*)dpy, DefaultScreen((Display*)dpy)), 0, 0, 0, 0, x, y);
    XFlush((Display*)dpy);

    return 0; // Handled
  }, dpy);

  lo_server_thread_add_method(st, "/4/toggle5", "f", [](const char *path, const char *types, lo_arg **argv, int argc, void *data, void *dpy) -> int {
    XTestFakeButtonEvent((Display*)dpy, 1, 1, 0);
    XTestFakeButtonEvent((Display*)dpy, 1, 0, 0);
    XFlush((Display*)dpy);

    return 0; // Handled
  }, dpy);

  lo_server_thread_start(st);

  for (;;) usleep(1000);

  XCloseDisplay(dpy);

  return EXIT_SUCCESS;
}
