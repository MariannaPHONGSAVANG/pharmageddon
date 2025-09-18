#include "gifclip.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

static int ensure_mpg_from_gif(const char *gif, const char *mpg,
                               const char *size, int fps) {
  if (access(mpg, F_OK) == 0) return 0;
  char cmd[1024];
  snprintf(cmd, sizeof(cmd),
    "ffmpeg -y -v error -i '%s' -vf \"fps=%d,scale=%s:flags=lanczos\" "
    "-c:v mpeg1video -q:v 2 -pix_fmt yuv420p -an '%s'", gif, fps, size, mpg);
  return system(cmd);
}

static uint32_t probe_duration_ms(const char *mpg) {
  char cmd[1024];
  snprintf(cmd, sizeof(cmd),
    "ffprobe -v error -show_entries format=duration -of csv=p=0 '%s'", mpg);
  FILE *p = popen(cmd, "r");
  if (!p) return 0;
  char buf[128] = {0};
  if (!fgets(buf, sizeof(buf), p)) { pclose(p); return 0; }
  pclose(p);
  double sec = atof(buf);
  if (sec <= 0.0) return 0;
  long ms = (long)(sec * 1000.0 + 0.5);
  return (uint32_t)(ms < 0 ? 0 : ms);
}

int gifclip_init(gifclip *gc, const char *gif_path,
                 const char *size, int fps, uint32_t loop_ms) {
  if (!gc || !gif_path || !size || fps <= 0) return -1;

  snprintf(gc->mpg_path, sizeof(gc->mpg_path),
           "../assets/%s_%s_%dfps.mpg", "gifclip", size, fps); // simple nom; fais plus sophistiqué si besoin

  if (ensure_mpg_from_gif(gif_path, gc->mpg_path, size, fps) != 0) {
    fprintf(stderr, "[gifclip] ffmpeg failed for: %s\n", gif_path);
    gc->ok = false; return -1;
  }

  FILE *f = fopen(gc->mpg_path, "rb");
  if (!f) { fprintf(stderr, "[gifclip] cannot open: %s (%s)\n", gc->mpg_path, strerror(errno)); gc->ok = false; return -1; }
  fclose(f);

  char path[256]; strncpy(path, gc->mpg_path, sizeof(path)); path[sizeof(path)-1]='\0';
  video_open(path, &gc->v);

  gc->loop_ms = loop_ms ? loop_ms : probe_duration_ms(gc->mpg_path);
  gc->ok = true;
  return 0;
}

void gifclip_frame(gifclip *gc, uint32_t *pixels, uint32_t t_ms) {
  if (!gc || !gc->ok) { gfx_cls(pixels, 0x00000000); return; }
  if (gc->loop_ms > 0) t_ms %= gc->loop_ms;
  video_frame(&gc->v, pixels, t_ms);
}

uint32_t gifclip_duration_ms(const gifclip *gc) {
  return (gc && gc->ok) ? gc->loop_ms : 0;
}
