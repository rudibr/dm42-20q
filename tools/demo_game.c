/* Plays one scripted game with the real engine and prints a transcript,
 * used by tools/make_gif.py to render the demo animation.
 *   gcc -O2 -Isrc tools/demo_game.c src/q20.c -lm -o demo && ./demo penguin [seed]
 * Output lines:  INFO nobj nq | CAT c | Q n answer text | G n result name | END ...
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "q20.h"
#include "kb_default.h"

static uint8_t *disk;
static int drd(void *c, uint32_t off, void *b, unsigned n) { memcpy(b, disk + off, n); return 0; }
static int dwr(void *c, uint32_t off, void *b, unsigned n) { memcpy(disk + off, b, n); return 0; }

static int truth(int o, int q) {
  int c = (default_cells[o][q >> 2] >> ((q & 3) * 2)) & 3;
  return c == 1 ? A_YES : c == 2 ? A_UNKNOWN : A_NO;
}

int main(int argc, char **argv) {
  q20_kb_t kb; q20_game_t g;
  disk = calloc(1, Q20_FILE_SIZE);
  q20_alloc(&kb, &g); kb.rd = drd; kb.wr = dwr;
  q20_format(&kb); q20_open(&kb);
  srand(argc > 2 ? atoi(argv[2]) : 1);
  int target = q20_find_object(&kb, argc > 1 ? argv[1] : "penguin");
  if (target < 0) { fprintf(stderr, "unknown object\n"); return 1; }
  printf("INFO %d %d\n", kb.nobj, kb.nq);

  q20_new_game(&kb, &g);
  int cat = 0;
  for (int c = 0; c < Q20_NCAT; c++) if (truth(target, c) == A_YES) cat = c;
  printf("CAT %d\n", cat);
  q20_set_category(&kb, &g, cat);
  static const char *an[] = { "", "Yes", "No", "Maybe", "Unknown" };
  for (int step = 0; step < 300; step++) {
    int obj, r = q20_should_guess(&kb, &g, &obj);
    if (r == 2) { printf("END lose %d\n", q20_counted(&g)); return 0; }
    if (r == 1) {
      char nm[Q20_NAMELEN]; q20_name(&kb, obj, nm);
      int ok = (obj == target);
      printf("G %d %s %s\n", q20_counted(&g), ok ? "right" : "wrong", nm);
      if (ok) { printf("END win %d\n", q20_counted(&g)); return 0; }
      q20_reject(&g, obj);
      continue;
    }
    int q = q20_next_question(&kb, &g);
    int a = truth(target, q);
    printf("Q %d %s %s\n", q20_counted(&g) + 1, an[a], kb.quest[q]);
    q20_answer(&kb, &g, q, a);
  }
  return 0;
}
