// gifclip.c
#include "gifclip.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
// --- utils ---
// gifclip.c
static int parse_size(const char *size, int *w, int *h) {
  if (!size || !w || !h) return -1;
  int ww = 0, hh = 0;

  // 1) "192:192"
  if (sscanf(size, "%d:%d", &ww, &hh) == 2) {
    if (ww > 0 && hh > 0) { *w = ww; *h = hh; return 0; }
  }
  // 2) "192x192" (éventuellement avec espaces)
  if (sscanf(size, "%d x %d", &ww, &hh) == 2 ||
      sscanf(size, "%dx%d",     &ww, &hh) == 2) {
    if (ww > 0 && hh > 0) { *w = ww; *h = hh; return 0; }
  }
  return -1;
}


static void basename_noext(const char *path, char *out, size_t outsz) {
  const char *b = strrchr(path, '/');
  b = b ? b + 1 : path;
  snprintf(out, outsz, "%s", b);        // <-- au lieu de strncpy
  char *dot = strrchr(out, '.');
  if (dot) *dot = '\0';
}


static void sanitize_for_fname(const char *in, char *out, size_t outsz) {
  // remplace caractères gênants (":/ \") par '_'
  size_t i = 0;
  for (; in[i] && i + 1 < outsz; ++i) {
    char c = in[i];
    if (c == ':' || c == '/' || c == '\\' || c == ' ') c = '_';
    out[i] = c;
  }
  out[i] = '\0';
}

static int ensure_mpg_from_gif(const char *gif, const char *mpg,
                               const char *size, int fps) {
  if (access(mpg, F_OK) == 0) return 0;

  int W = 192, H = 192;
  parse_size(size, &W, &H);  // si parse échoue, W/H restent 192

  // Fill: garder ratio, agrandir jusqu'à couvrir W×H, puis cropper centré
  // Equivalent: scale=WxH:force_original_aspect_ratio=increase,crop=W:H
  char vf[256];
  snprintf(vf, sizeof(vf),
           "fps=%d,"
           "scale=%d:%d:force_original_aspect_ratio=increase,"
           "crop=%d:%d",
           fps, W, H, W, H);

  char cmd[1024];
  snprintf(cmd, sizeof(cmd),
    "ffmpeg -y -v error -i '%s' -vf \"%s\" "
    "-c:v mpeg1video -q:v 2 -pix_fmt yuv420p -an '%s'",
    gif, vf, mpg);

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

  char base[128], size_clean[64];
basename_noext(gif_path, base, sizeof(base));
sanitize_for_fname(size, size_clean, sizeof(size_clean));

snprintf(gc->mpg_path, sizeof(gc->mpg_path),
         "../assets/gifclip_%s_%s_%dfps.mpg", base, size_clean, fps);

  if (ensure_mpg_from_gif(gif_path, gc->mpg_path, size, fps) != 0) {
    fprintf(stderr, "[gifclip] ffmpeg failed for: %s\n", gif_path);
    gc->ok = false; return -1;
    

  }

  FILE *f = fopen(gc->mpg_path, "rb");
  if (!f) { fprintf(stderr, "[gifclip] cannot open: %s (%s)\n", gc->mpg_path, strerror(errno)); gc->ok = false; return -1; }
  fclose(f);

  char path[256]; strncpy(path, gc->mpg_path, sizeof(path)); path[sizeof(path)-1] = '\0';
  video_open(path, &gc->v);

  // Durée de boucle : paramètre explicite ou détection ffprobe
  uint32_t detected = probe_duration_ms(gc->mpg_path);
  gc->loop_ms = (loop_ms > 0) ? loop_ms : detected;

  // Fallback si toujours 0 (ffprobe manquant/raté)
  if (gc->loop_ms == 0) {
    gc->loop_ms = 4000; // boucle par défaut toutes les 4s
    fprintf(stderr, "[gifclip] duration unknown, falling back to %u ms\n", gc->loop_ms);
  } else {
    fprintf(stderr, "[gifclip] detected duration = %u ms (loop)\n", gc->loop_ms);
  }

  gc->ok = true;
  gc->last_cycle = (uint32_t)(-1);
  return 0;
}

void gifclip_frame(gifclip *gc, uint32_t *pixels, uint32_t t_ms) {
  if (!gc || !gc->ok || gc->loop_ms == 0) {
    gfx_cls(pixels, 0x00000000);
    return;
  }

  // Temps global -> (cycle, temps local)
  uint32_t cycle = t_ms / gc->loop_ms;
  uint32_t local = t_ms % gc->loop_ms;

  // Si on passe à un nouveau cycle, on réinitialise le décodeur
  if (cycle != gc->last_cycle) {
    // Ré-ouvre depuis le début (plus robuste qu’un seek arrière)
    video_open(gc->mpg_path, &gc->v);
    gc->last_cycle = cycle;
  }

  // Rendu de la frame pour ce temps local
  video_frame(&gc->v, pixels, local);
}
uint32_t gifclip_duration_ms(const gifclip *gc) {
  return (gc && gc->ok) ? gc->loop_ms : 0u;
}
