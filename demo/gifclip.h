#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "video.h"

typedef struct {
  video v;
  bool ok;
  uint32_t loop_ms;     // durée de boucle (ms). 0 => déduite via ffprobe si possible
  char mpg_path[256];   // chemin du .mpg généré
} gifclip;

int  gifclip_init(gifclip *gc, const char *gif_path,
                  const char *size, int fps, uint32_t loop_ms);
void gifclip_frame(gifclip *gc, uint32_t *pixels, uint32_t t_ms);
uint32_t gifclip_duration_ms(const gifclip *gc); // renvoie loop_ms (renseigné)
