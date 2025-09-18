#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "gfx.h"
#include "video.h"
#include "badapple.h"

static video badapple;
static bool badapple_ok = false;

void badapple_init(void) {
  const char *path = "../assets/BadApple_192x192_audio.mpg";
  FILE *f = fopen(path, "rb");
  if (!f) {
    fprintf(stderr, "[badapple] not found: %s\n", path);
    return;
  }
  fclose(f);

  // video_open attend un char* non-const
  char pbuf[256];
  strncpy(pbuf, path, sizeof(pbuf));
  pbuf[sizeof(pbuf) - 1] = '\0';

  video_open(pbuf, &badapple);
  badapple_ok = true;
}

void badapple_frame(uint32_t *pixels, uint32_t time_ms) {
  if (!badapple_ok) {
    gfx_cls(pixels, 0x00000000);
    return;
  }
  video_frame(&badapple, pixels, time_ms);
}
