// #include "credits.h"
// #include "ambulance.h"
// #include "planes.h"
// #include "boards.h"
// #include "video1.h"
// #include "scroller.h"
// #include "title.h"
// #include "patarty.h"
// #include "jarig.h"
// #include "physics.h"
// #include "badapple.h"
// #include "twister.h"
// #include "stniccc.h"
// #include "prescription.h"
// #include "nyancat.h"
// #include "amigaball.h"
// #include "farjan.h"
// #include "tomato.h"
// #include "greets.h"
// #include "gfx.h"

// void demo_init(void) {
//     credits_init();
//     ambulance_init();
//     planes_init();
//     boards_init();
//     video1_init();
//     scroller_init();
//     title_init();
//     patarty_init();
//     jarig_init();
//     physics_init();
//     badapple_init();
//     twister_init();
//     stniccc_init();
//     prescription_init();
//     nyancat_init();
//     amigaball_init();
//     farjan_init();
//     tomato_init();
//     greets_init();
// }

// #define BAR 3840

// #define SCROLLER_END BAR * 7
// #define BOARDS_END BAR * 9
// #define CREDITS_END BAR * 13
// #define TITLE_END BAR * 17
// #define AMBULANCE_END BAR * 22
// #define EFFECT1_END BAR * 26
// #define MEMES 118000
// #define MEMES_BAR 3200
// #define MEME1_END (MEMES + MEMES_BAR * 1)
// #define MEME2_END (MEMES + MEMES_BAR * 2)
// #define MEME3_END (MEMES + MEMES_BAR * 3)
// #define MEME4_END (MEMES + MEMES_BAR * 4)
// #define MEME5_END (MEMES + MEMES_BAR * 5)
// #define MEME6_END (MEMES + MEMES_BAR * 6)
// #define MEME7_END (MEMES + MEMES_BAR * 7)
// #define MEME8_END (MEMES + MEMES_BAR * 8)
// #define PLANES_END 153000
// #define VIDEO_END 168874
// #define MUSIC_END 212000

// void demo_frame(uint32_t *pixels, uint32_t time) {
//     if (time < SCROLLER_END) {
//         scroller_frame(pixels, time);
//     // } else if (time < BOARDS_END) {
//     //     boards_frame(pixels, time);
//     // } else if (time < CREDITS_END) {
//     //     credits_frame(pixels, time - BOARDS_END);
//     // } else if (time < TITLE_END) {
//     //     title_frame(pixels, time - CREDITS_END);
//     // } else if (time < AMBULANCE_END) {
//     //     ambulance_frame(pixels, time - TITLE_END);
//     // } else if (time < EFFECT1_END) {
//     //     twister_frame(pixels, time - AMBULANCE_END);
//     } else if (time < MEMES) {
//         prescription_frame(pixels, time - EFFECT1_END);
//     } else if (time < MEME8_END) {
//          if (time < MEME1_END) {
//         //     if ((time - MEMES) % 400 < 30) {
//         //         gfx_cls(pixels, 0xffffff00);
//         //     } else {
//         //         patarty_frame(pixels, time - MEMES);
//         //     }
//         // } else if (time < MEME2_END) {
//         //     if ((time - MEMES) % 400 < 30) {
//         //         gfx_cls(pixels, 0xffffff00);
//         //     } else {
//         //         nyancat_frame(pixels, time);
//         //     }
//         // } else if (time < MEME3_END) {
//         //     jarig_frame(pixels, time - MEME2_END);
//         } else if (time < MEME4_END) {
//             badapple_frame(pixels, time);
//         } //else if (time < MEME5_END) {
//         //     amigaball_frame(pixels, time - MEME4_END);
//         // } else if (time < MEME6_END) {
//         //     tomato_frame(pixels, time - MEME5_END);
//         // } else if (time < MEME7_END) {
//         //     if ((time - MEMES) % 400 < 30) {
//         //         gfx_cls(pixels, 0xffffff00);
//         //     } else {
//         //         farjan_frame(pixels, time - MEME6_END);
//         //     }
//         // } else {
//         //     stniccc_frame(pixels, time - MEME7_END);
//          //}
//     // } else if (time < PLANES_END) {
//     //     planes_frame(pixels, time);
//     // } else if (time < VIDEO_END) {
//     //     video1_frame(pixels, time);
//     // } else if (time < MUSIC_END) {
//     //     greets_frame(pixels, time - VIDEO_END);
//     // } else {
//     //     gfx_cls(pixels, 0x00000000);
//     // }
// }


#include <stdint.h>
#include "gfx.h"
#include "scroller.h"
#include "prescription.h"
#include "badapple.h"
#include "gifclip.h"

// ==== Timeline “BAR” (comme l’original) ====
#define BAR 3840

#define SCROLLER_END       (BAR * 1)
#define PRESCRIPTION_END   (SCROLLER_END + BAR * 1)
#define BADAPPLE_END       (PRESCRIPTION_END + BAR * 1)

// ---- Config GIFs ici : tu AJOUTES tes fichiers ici ----
typedef struct {
  const char *path;     // chemin du GIF
  const char *size;     // "192:192" pour la croix entière
  int fps;              // ex: 25
  uint32_t loop_ms;     // 0 => auto (durée réelle via ffprobe)
  uint32_t window_ms;   // durée à l’écran (en ms) -> le GIF boucle pendant ce temps
} GifConf;

static GifConf GIFS[] = {
  // Boucle à sa durée réelle, mais reste 20 secondes à l’écran
  { "../assets/pattern-14425_512.gif", "192:192", 25, 0, 20000 },
  { "../assets/pattern-purple.gif", "192:192", 25, 0, 15000 },
  { "../assets/Medicine_Pills.gif", "192:192", 25, 0, 15000 },

  


  // Exemple: reste 8 s, même si le GIF fait 3.8 s (il reboucle 2×+)
  // { "../assets/ton_autre.gif", "192:192", 25, 0, 8000 },
};
#define NUM_GIFS (sizeof(GIFS)/sizeof(GIFS[0]))



static gifclip clips[NUM_GIFS];

static uint32_t GIFS_END = 0;
static uint32_t gif_cumul_end[NUM_GIFS];
static uint32_t gif_total_ms = 0;

void demo_init(void) {
  scroller_init();
  prescription_init();
  badapple_init();

  // Init GIFs + cumuls
  uint32_t t = 0;
  for (int i = 0; i < (int)NUM_GIFS; ++i) {
    gifclip_init(&clips[i], GIFS[i].path, GIFS[i].size, GIFS[i].fps, GIFS[i].loop_ms);

    // Combien de temps on veut l’afficher ?
    uint32_t play_ms = GIFS[i].window_ms;
    if (play_ms == 0) {
      // si tu laisses 0, on prend sa durée réelle (il ne jouera qu’1 boucle)
      uint32_t d = gifclip_duration_ms(&clips[i]);
      play_ms = (d > 0) ? d : 5000;
    }

    t += play_ms;
    gif_cumul_end[i] = t;
  }
  gif_total_ms = t;
  GIFS_END = BADAPPLE_END + gif_total_ms;
}

static inline void run_gif_playlist(uint32_t *pixels, uint32_t t_local) {
  // Boucler toute la playlist si tu veux
  if (gif_total_ms > 0) t_local %= gif_total_ms;

  uint32_t start = 0;
  for (int i = 0; i < (int)NUM_GIFS; ++i) {
    uint32_t end = gif_cumul_end[i];
    if (t_local < end) {
      uint32_t local = t_local - start;   // temps écoulé dans la fenêtre
      gifclip_frame(&clips[i], pixels, local); // => s’occupe du modulo sur la durée réelle
      return;
    }
    start = end;
  }
  gfx_cls(pixels, 0x00000000);
}

void demo_frame(uint32_t *pixels, uint32_t time) {
  if (time < SCROLLER_END) {
    scroller_frame(pixels, time);

  } else if (time < PRESCRIPTION_END) {
    prescription_frame(pixels, time - SCROLLER_END);

  } else if (time < BADAPPLE_END) {
    badapple_frame(pixels, time - PRESCRIPTION_END);

  } else if (time < GIFS_END) {
    run_gif_playlist(pixels, time - BADAPPLE_END);

  } else {
    gfx_cls(pixels, 0x00000000);
  }
}
