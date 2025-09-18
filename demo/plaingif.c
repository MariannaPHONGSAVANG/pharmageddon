#include "gifclip.h"
#include "plaingif.h"

static gifclip g;

void plaingif_init(void) {
  // Paramètres **reconfigurables** via variables d’environnement :
  const char *gif   = getenv("PLAINGIF_PATH") ?: "../assets/pattern-14425_512.gif";
  const char *size  = getenv("PLAINGIF_SIZE") ?: "192:192"; // pour ta croix
  int fps          = getenv("PLAINGIF_FPS") ? atoi(getenv("PLAINGIF_FPS")) : 25;
  int loop_ms_env  = getenv("PLAINGIF_LOOP_MS") ? atoi(getenv("PLAINGIF_LOOP_MS")) : 0;

  gifclip_init(&g, gif, size, fps, (uint32_t)loop_ms_env);
}

void plaingif_frame(uint32_t *pixels, uint32_t t_ms) {
  gifclip_frame(&g, pixels, t_ms);
}
