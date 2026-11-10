/* Host-side test of the 20Q engine:
 *   gcc -O2 -Isrc test/host_test.c src/q20.c -lm -o q20test && ./q20test
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "q20.h"
#include "kb_default.h"

static uint8_t *disk;
static unsigned long bytes_read, reads;
static int drd(void *c, uint32_t off, void *b, unsigned n) {
  if (off + n > Q20_FILE_SIZE) return -1;
  memcpy(b, disk + off, n); bytes_read += n; reads++; return 0;
}
static int dwr(void *c, uint32_t off, void *b, unsigned n) {
  if (off + n > Q20_FILE_SIZE) return -1;
  memcpy(disk + off, b, n); return 0;
}

static int truth(int o, int q) {                    /* from built-in table */
  if (o >= DEFAULT_NOBJ || q >= DEFAULT_NQ) return A_UNKNOWN;
  int c = (default_cells[o][q >> 2] >> ((q & 3) * 2)) & 3;
  return c == 1 ? A_YES : c == 2 ? A_UNKNOWN : A_NO;
}
static int category(int o) {
  for (int c = 0; c < Q20_NCAT; c++) if (truth(o, c) == A_YES) return c;
  return -1;
}

static float frand(void) { return rand() / (float)RAND_MAX; }

/* play one game; noise = chance that a yes/no answer is flipped */
static int play(q20_kb_t *kb, q20_game_t *g, int target, float noise, int *nq, int *ng) {
  q20_new_game(kb, g);
  q20_set_category(kb, g, category(target));
  for (int step = 0; step < 300; step++) {
    int obj, r = q20_should_guess(kb, g, &obj);
    if (r == 2) break;
    if (r == 1) {
      if (obj == target) { *nq = q20_counted(g); *ng = g->nguess + 1; return 1; }
      q20_reject(g, obj);
      continue;
    }
    int q = q20_next_question(kb, g);
    int a = truth(target, q);
    if (a != A_UNKNOWN && frand() < noise) a = (a == A_YES) ? A_NO : A_YES;
    q20_answer(kb, g, q, a);
  }
  *nq = q20_counted(g); *ng = g->nguess;
  return 0;
}

int main(void) {
  q20_kb_t kb; q20_game_t g;
  disk = calloc(1, Q20_FILE_SIZE);
  if (q20_alloc(&kb, &g)) { puts("alloc failed"); return 1; }
  kb.rd = drd; kb.wr = dwr; kb.ctx = NULL;
  if (q20_format(&kb) || q20_open(&kb)) { puts("format/open failed"); return 1; }

  size_t heap = Q20_MAXQ * Q20_QLEN + 2 * Q20_MAXOBJ + Q20_MAXOBJ + Q20_MAXOBJ
              + 2 * 4 * Q20_MAXOBJ + 4 * Q20_MAXQ;
  printf("objects %d, questions %d, file %u bytes, heap ~%zu bytes\n",
         kb.nobj, kb.nq, (unsigned)Q20_FILE_SIZE, heap);

  float noises[] = { 0.0f, 0.05f, 0.10f };
  for (int ni = 0; ni < 3; ni++) {
    srand(42);
    int win = 0, first = 0, games = 0, sumq = 0, lostp = 0;
    bytes_read = reads = 0;
    for (int o = 0; o < kb.nobj; o++) {
      int nq, ng, w = play(&kb, &g, o, noises[ni], &nq, &ng);
      win += w; games++; sumq += nq; first += (w && ng == 1);
      if (!w && ni == 0 && lostp++ < 15) {
        char nm[Q20_NAMELEN]; q20_name(&kb, o, nm); printf("  lost: %s\n", nm);
      }
    }
    printf("noise %2.0f%%: won %d/%d (%.1f%%), first guess right %.1f%%, "
           "avg %.1f questions, avg %.0f KB read/game\n",
           noises[ni]*100, win, games, 100.0*win/games, 100.0*first/games,
           (float)sumq/games, bytes_read / 1024.0 / games);
  }

  /* learning: a new animal "giraffe-like" object taught from a game */
  q20_new_game(&kb, &g);
  q20_set_category(&kb, &g, 0);
  const char *keys[] = { "Is it a mammal?", "Is it bigger than a car?",
                         "Does it have four legs?", "Is it found in Africa?",
                         "Does it have stripes or spots?" , "Is it found in the sea?"};
  int vals[] = { A_YES, A_YES, A_YES, A_YES, A_NO, A_NO };
  int qs[6];
  for (int i = 0; i < 6; i++) {
    qs[i] = -1;
    for (int q = 0; q < kb.nq; q++) if (!strcmp(kb.quest[q], keys[i])) qs[i] = q;
    q20_answer(&kb, &g, qs[i], vals[i]);
  }
  int o = q20_add_object(&kb, "okapi");
  q20_learn(&kb, &g, o);
  int nq, ng, hits = 0;
  for (int rep = 0; rep < 5; rep++) {
    q20_new_game(&kb, &g);
    q20_set_category(&kb, &g, 0);
    int won = 0;
    for (int s = 0; s < 300; s++) {
      int obj, r = q20_should_guess(&kb, &g, &obj);
      if (r == 2) break;
      if (r == 1) { if (obj == o) { won = 1; break; } q20_reject(&g, obj); continue; }
      int q = q20_next_question(&kb, &g), a = A_UNKNOWN;
      for (int i = 0; i < 6; i++) if (qs[i] == q) a = vals[i];
      q20_answer(&kb, &g, q, a);
    }
    hits += won;
  }
  (void)nq; (void)ng;
  printf("learned 'okapi' (6 answers) found again in %d/5 games\n", hits);

  /* new question */
  int nqid = q20_add_question(&kb, "Does it have stripes on its legs?");
  q20_set_w(&kb, o, nqid, 6);

  /* reopen from "disk" */
  q20_kb_t kb2; q20_game_t g2;
  q20_alloc(&kb2, &g2); kb2.rd = drd; kb2.wr = dwr;
  char nm[Q20_NAMELEN];
  int ok = !q20_open(&kb2) && kb2.nobj == kb.nobj && kb2.nq == kb.nq &&
           !q20_name(&kb2, o, nm) && !strcmp(nm, "okapi") && kb2.plays[o] == 1 &&
           !strcmp(kb2.quest[nqid], "Does it have stripes on its legs?");
  printf("reopen: %s (nobj %d, nq %d)\n", ok ? "OK" : "FAIL", kb2.nobj, kb2.nq);
  printf("find 'Polar-Bear' -> %d, 'OKAPI' -> %d, 'xyz' -> %d\n",
         q20_find_object(&kb2, "Polar-Bear"), q20_find_object(&kb2, "OKAPI"),
         q20_find_object(&kb2, "xyz"));
  return 0;
}
