/*
 * 20Q for DM42 - portable game engine. See q20.h.
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "q20.h"
#include "kb_default.h"

#define W_INIT       6      /* weight for a learned or firm yes/no      */
#define PRIOR_K      0.25f  /* how much popular objects are favoured    */
#define EARLY_GUESS  0.92f  /* guess before Q20 when this sure          */
#define CAND_MIN     1e-5f  /* ignore objects below this posterior      */
#define CAT_K        3.0f   /* the category answer counts this much     */

static const int8_t code_w[4] = { -4, 6, 0, -6 };  /* weak no, yes, ?, no */

/* ---------- probability tables ---------- */

static float logL[5][2*Q20_WMAX+1];   /* [answer][w+WMAX] */
static float pyes[2*Q20_WMAX+1];

static void init_tables(void) {
  for (int i = 0; i <= 2*Q20_WMAX; i++) {
    int   w = i - Q20_WMAX;
    float p = 0.5f + 0.45f * (float)w / Q20_WMAX;   /* 0.05 .. 0.95 */
    pyes[i] = p;
    logL[A_NONE][i]    = 0.0f;
    logL[A_YES][i]     = logf(p);
    logL[A_NO][i]      = logf(1.0f - p);
    logL[A_MAYBE][i]   = logf(0.25f + 0.75f * (1.0f - fabsf(2.0f*p - 1.0f)));
    logL[A_UNKNOWN][i] = 0.0f;
  }
}

static int clampw(int w) {
  return w > Q20_WMAX ? Q20_WMAX : w < -Q20_WMAX ? -Q20_WMAX : w;
}

/* ---------- memory ---------- */

int q20_alloc(q20_kb_t *kb, q20_game_t *g) {
  kb->quest    = malloc(sizeof(*kb->quest) * Q20_MAXQ);
  kb->plays    = malloc(sizeof(uint16_t) * Q20_MAXOBJ);
  kb->col      = malloc(Q20_MAXOBJ);
  g->rejected  = malloc(Q20_MAXOBJ);
  g->score     = malloc(sizeof(float) * Q20_MAXOBJ);
  g->post      = malloc(sizeof(float) * Q20_MAXOBJ);
  g->eh        = malloc(sizeof(float) * Q20_MAXQ);
  if (!kb->quest || !kb->plays || !kb->col || !g->rejected ||
      !g->score || !g->post || !g->eh)
    return -1;
  kb->nobj = kb->nq = 0;
  init_tables();
  return 0;
}

/* ---------- low level file access ---------- */

static int read_col(const q20_kb_t *kb, int q) {
  return kb->rd(kb->ctx, Q20_OFF_W + (uint32_t)q * Q20_MAXOBJ, kb->col, kb->nobj);
}

static int write_header(q20_kb_t *kb) {
  uint8_t h[16] = { 'Q', '2', '0', 'D' };
  h[4] = kb->nobj & 0xFF;  h[5] = kb->nobj >> 8;
  h[6] = kb->nq & 0xFF;    h[7] = kb->nq >> 8;
  h[8] = Q20_MAXOBJ & 0xFF; h[9] = Q20_MAXOBJ >> 8;
  h[10] = Q20_MAXQ & 0xFF;  h[11] = Q20_MAXQ >> 8;
  return kb->wr(kb->ctx, 0, h, 16);
}

static int write_plays(q20_kb_t *kb, int o) {
  uint8_t b[2] = { kb->plays[o] & 0xFF, kb->plays[o] >> 8 };
  return kb->wr(kb->ctx, Q20_OFF_PLAYS + 2u * o, b, 2);
}

static int8_t get_w(const q20_kb_t *kb, int o, int q) {
  int8_t w = 0;
  kb->rd(kb->ctx, Q20_OFF_W + (uint32_t)q * Q20_MAXOBJ + o, &w, 1);
  return w;
}

int q20_set_w(q20_kb_t *kb, int o, int q, int w) {
  int8_t v = clampw(w);
  return kb->wr(kb->ctx, Q20_OFF_W + (uint32_t)q * Q20_MAXOBJ + o, &v, 1);
}

/* ---------- knowledge base ---------- */

int q20_format(q20_kb_t *kb) {
  int8_t *buf = kb->col;               /* Q20_MAXOBJ bytes of scratch */
  kb->nobj = DEFAULT_NOBJ;
  kb->nq   = DEFAULT_NQ;
  if (write_header(kb)) return -1;

  /* plays */
  memset(kb->plays, 0, 2 * Q20_MAXOBJ);
  memset(buf, 0, Q20_MAXOBJ);
  for (uint32_t off = 0; off < 2u * Q20_MAXOBJ; off += Q20_MAXOBJ)
    if (kb->wr(kb->ctx, Q20_OFF_PLAYS + off, buf, Q20_MAXOBJ)) return -1;

  /* questions */
  for (int q = 0; q < Q20_MAXQ; q++) {
    memset(kb->quest[q], 0, Q20_QLEN);
    if (q < DEFAULT_NQ) strncpy(kb->quest[q], default_questions[q], Q20_QLEN - 1);
    if (kb->wr(kb->ctx, Q20_OFF_QUEST + (uint32_t)q * Q20_QLEN, kb->quest[q], Q20_QLEN))
      return -1;
  }

  /* names, written in blocks of whole names */
  const int per = Q20_MAXOBJ / Q20_NAMELEN;
  const char *np = default_names;
  for (int o0 = 0; o0 < Q20_MAXOBJ; o0 += per) {
    int n = (o0 + per <= Q20_MAXOBJ) ? per : Q20_MAXOBJ - o0;
    memset(buf, 0, n * Q20_NAMELEN);
    for (int i = 0; i < n; i++) {
      if (o0 + i >= DEFAULT_NOBJ) break;
      strncpy((char *)buf + i * Q20_NAMELEN, np, Q20_NAMELEN - 1);
      np += strlen(np) + 1;
    }
    if (kb->wr(kb->ctx, Q20_OFF_NAMES + (uint32_t)o0 * Q20_NAMELEN, buf, n * Q20_NAMELEN))
      return -1;
  }

  /* weights, one column at a time */
  for (int q = 0; q < Q20_MAXQ; q++) {
    memset(buf, 0, Q20_MAXOBJ);
    if (q < DEFAULT_NQ)
      for (int o = 0; o < DEFAULT_NOBJ; o++)
        buf[o] = code_w[(default_cells[o][q >> 2] >> ((q & 3) * 2)) & 3];
    if (kb->wr(kb->ctx, Q20_OFF_W + (uint32_t)q * Q20_MAXOBJ, buf, Q20_MAXOBJ))
      return -1;
  }
  return 0;
}

int q20_open(q20_kb_t *kb) {
  uint8_t h[16];
  if (kb->rd(kb->ctx, 0, h, 16) || memcmp(h, "Q20D", 4)) return -1;
  int nobj = h[4] | (h[5] << 8), nq = h[6] | (h[7] << 8);
  int mo = h[8] | (h[9] << 8),   mq = h[10] | (h[11] << 8);
  if (mo != Q20_MAXOBJ || mq != Q20_MAXQ) return -1;
  if (nobj < 1 || nobj > Q20_MAXOBJ || nq < Q20_NCAT || nq > Q20_MAXQ) return -1;
  uint8_t *b = (uint8_t *)kb->col;
  for (int o0 = 0; o0 < nobj; o0 += Q20_MAXOBJ / 2) {
    int n = nobj - o0 < Q20_MAXOBJ / 2 ? nobj - o0 : Q20_MAXOBJ / 2;
    if (kb->rd(kb->ctx, Q20_OFF_PLAYS + 2u * o0, b, 2 * n)) return -1;
    for (int i = 0; i < n; i++) kb->plays[o0 + i] = b[2*i] | (b[2*i+1] << 8);
  }
  for (int q = 0; q < nq; q++) {
    if (kb->rd(kb->ctx, Q20_OFF_QUEST + (uint32_t)q * Q20_QLEN, kb->quest[q], Q20_QLEN))
      return -1;
    kb->quest[q][Q20_QLEN - 1] = 0;
  }
  kb->nobj = nobj;
  kb->nq = nq;
  return 0;
}

int q20_name(const q20_kb_t *kb, int o, char *out) {
  out[0] = 0;
  if (o < 0 || o >= kb->nobj) return -1;
  if (kb->rd(kb->ctx, Q20_OFF_NAMES + (uint32_t)o * Q20_NAMELEN, out, Q20_NAMELEN)) {
    out[0] = 0;
    return -1;
  }
  out[Q20_NAMELEN - 1] = 0;
  return 0;
}

/* ---------- text helpers ---------- */

void q20_to_ascii(const char *in, char *out, int outlen) {
  int o = 0;
  const unsigned char *s = (const unsigned char *)in;
  while (*s && o < outlen - 3) {
    if (*s < 0x80) { out[o++] = *s++; continue; }
    if (s[0] == 0xC3 && s[1]) {
      const char *r = "?";
      switch (s[1]) {
        case 0xA6: r = "ae"; break;  case 0x86: r = "AE"; break;
        case 0xB8: r = "oe"; break;  case 0x98: r = "OE"; break;
        case 0xA5: r = "aa"; break;  case 0x85: r = "AA"; break;
        case 0xA9: case 0xA8: r = "e"; break;
        case 0xB3: case 0xB2: case 0xB6: r = "o"; break;
        case 0xBC: r = "u"; break;   case 0xA4: r = "a"; break;
      }
      while (*r && o < outlen - 1) out[o++] = *r++;
      s += 2;
      continue;
    }
    s++;
    while ((*s & 0xC0) == 0x80) s++;
    out[o++] = '?';
  }
  out[o] = 0;
}

/* lower case, no spaces/hyphens, so "Polar Bear" == "polarbear" */
static void norm_name(const char *in, char *out, int outlen) {
  char tmp[Q20_NAMELEN * 2];
  q20_to_ascii(in, tmp, sizeof(tmp));
  int o = 0;
  for (char *p = tmp; *p && o < outlen - 1; p++) {
    char c = *p;
    if (c == ' ' || c == '-' || c == '\'') continue;
    if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    out[o++] = c;
  }
  out[o] = 0;
}

int q20_find_object(const q20_kb_t *kb, const char *name) {
  char a[Q20_NAMELEN * 2], b[Q20_NAMELEN * 2];
  norm_name(name, a, sizeof(a));
  if (!a[0]) return -1;
  const int per = Q20_MAXOBJ / Q20_NAMELEN;
  char *buf = (char *)kb->col;
  for (int o0 = 0; o0 < kb->nobj; o0 += per) {
    int n = kb->nobj - o0 < per ? kb->nobj - o0 : per;
    if (kb->rd(kb->ctx, Q20_OFF_NAMES + (uint32_t)o0 * Q20_NAMELEN, buf, n * Q20_NAMELEN))
      return -1;
    for (int i = 0; i < n; i++) {
      char *nm = buf + i * Q20_NAMELEN;
      nm[Q20_NAMELEN - 1] = 0;
      norm_name(nm, b, sizeof(b));
      if (!strcmp(a, b)) return o0 + i;
    }
  }
  return -1;
}

int q20_add_object(q20_kb_t *kb, const char *name) {
  if (kb->nobj >= Q20_MAXOBJ) return -1;
  int o = kb->nobj;
  char nm[Q20_NAMELEN];
  memset(nm, 0, sizeof(nm));
  strncpy(nm, name, Q20_NAMELEN - 1);
  if (kb->wr(kb->ctx, Q20_OFF_NAMES + (uint32_t)o * Q20_NAMELEN, nm, Q20_NAMELEN)) return -1;
  /* its weights must start at 0 (unknown) in every column */
  for (int q = 0; q < kb->nq; q++)
    if (q20_set_w(kb, o, q, 0)) return -1;
  kb->plays[o] = 0;
  if (write_plays(kb, o)) return -1;
  kb->nobj++;
  if (write_header(kb)) { kb->nobj--; return -1; }
  return o;
}

int q20_add_question(q20_kb_t *kb, const char *text) {
  if (kb->nq >= Q20_MAXQ) return -1;
  int q = kb->nq;
  memset(kb->quest[q], 0, Q20_QLEN);
  strncpy(kb->quest[q], text, Q20_QLEN - 1);
  if (kb->wr(kb->ctx, Q20_OFF_QUEST + (uint32_t)q * Q20_QLEN, kb->quest[q], Q20_QLEN))
    return -1;
  memset(kb->col, 0, Q20_MAXOBJ);   /* unknown for everybody */
  if (kb->wr(kb->ctx, Q20_OFF_W + (uint32_t)q * Q20_MAXOBJ, kb->col, Q20_MAXOBJ))
    return -1;
  kb->nq++;
  if (write_header(kb)) { kb->nq--; return -1; }
  return q;
}

/* ---------- game ---------- */

void q20_new_game(const q20_kb_t *kb, q20_game_t *g) {
  memset(g->ans, 0, sizeof(g->ans));
  memset(g->rejected, 0, Q20_MAXOBJ);
  g->nasked = 0;
  g->nfree = 0;
  g->nguess = 0;
  g->last_guess_at = -1;
  for (int o = 0; o < kb->nobj; o++)
    g->score[o] = PRIOR_K * logf(1.0f + kb->plays[o]);
}

int q20_counted(const q20_game_t *g) { return g->nasked - g->nfree; }

static int apply(const q20_kb_t *kb, q20_game_t *g, int q, int a, float sign) {
  if (a == A_UNKNOWN || a == A_NONE) return 0;
  if (read_col(kb, q)) return -1;
  if (q < Q20_NCAT) sign *= CAT_K;
  for (int o = 0; o < kb->nobj; o++)
    g->score[o] += sign * logL[a][kb->col[o] + Q20_WMAX];
  return 0;
}

int q20_answer(const q20_kb_t *kb, q20_game_t *g, int q, int a) {
  if (q < 0 || q >= kb->nq || g->ans[q] != A_NONE) return -1;
  if (apply(kb, g, q, a, 1.0f)) return -1;
  g->ans[q] = a;
  g->hist[g->nasked++] = q;
  return 0;
}

int q20_set_category(const q20_kb_t *kb, q20_game_t *g, int cat) {
  for (int c = 0; c < Q20_NCAT; c++)
    if (q20_answer(kb, g, c, c == cat ? A_YES : A_NO)) return -1;
  g->nfree = Q20_NCAT - 1;   /* the four answers count as one question */
  return 0;
}

static int undo_one(const q20_kb_t *kb, q20_game_t *g) {
  int q = g->hist[--g->nasked];
  apply(kb, g, q, g->ans[q], -1.0f);
  g->ans[q] = A_NONE;
  return 1;
}

int q20_undo(const q20_kb_t *kb, q20_game_t *g) {
  if (g->nasked == 0) return 0;
  if (g->nfree && g->nasked <= Q20_NCAT) {        /* back to the start */
    while (g->nasked) undo_one(kb, g);
    g->nfree = 0;
  } else {
    undo_one(kb, g);
  }
  if (g->last_guess_at > q20_counted(g)) g->last_guess_at = -1;
  return 1;
}

void q20_reject(q20_game_t *g, int obj) {
  if (obj >= 0 && obj < Q20_MAXOBJ) g->rejected[obj] = 1;
  g->nguess++;
  g->last_guess_at = q20_counted(g);
}

int q20_posterior(const q20_kb_t *kb, q20_game_t *g, float *pbest) {
  float mx = -1e30f;
  int best = -1;
  for (int o = 0; o < kb->nobj; o++)
    if (!g->rejected[o] && g->score[o] > mx) { mx = g->score[o]; best = o; }
  float sum = 0;
  for (int o = 0; o < kb->nobj; o++) {
    float p = g->rejected[o] ? 0.0f : expf(g->score[o] - mx);
    g->post[o] = p;
    sum += p;
  }
  if (sum > 0)
    for (int o = 0; o < kb->nobj; o++) g->post[o] /= sum;
  if (pbest) *pbest = (best >= 0) ? g->post[best] : 0.0f;
  return best;
}

/* Fast natural log (about 1e-4 relative error), plenty for entropy. */
static float fast_logf(float x) {
  union { float f; uint32_t i; } u = { x };
  float e = (float)(int)((u.i >> 23) & 0xFF) - 127.0f;
  u.i = (u.i & 0x007FFFFF) | 0x3F800000;          /* mantissa in [1,2) */
  float m = u.f;
  float p = -1.7417939f + (2.8212026f + (-1.4699568f + (0.44717955f
            - 0.056570851f * m) * m) * m) * m;     /* log2(m) */
  return (p + e) * 0.69314718f;
}

static float plogp(float p) { return p > 1e-12f ? p * fast_logf(p) : 0.0f; }

/* Picks the question with the lowest expected entropy afterwards.
   Call after q20_posterior(). */
int q20_next_question(const q20_kb_t *kb, q20_game_t *g) {
  float best = 1e30f;
  int   nbest = -1;

  /* Objects are stored grouped by category, so the live candidates sit in
     a narrow index range: only that part of each column is read. */
  int lo = -1, hi = -1;
  for (int o = 0; o < kb->nobj; o++)
    if (g->post[o] >= CAND_MIN) { if (lo < 0) lo = o; hi = o; }
  if (lo < 0) return -1;

  for (int q = 0; q < kb->nq; q++) {
    g->eh[q] = 1e30f;
    if (g->ans[q] != A_NONE) continue;
    if (kb->rd(kb->ctx, Q20_OFF_W + (uint32_t)q * Q20_MAXOBJ + lo, kb->col + lo, hi - lo + 1))
      continue;
    float py = 0, sy = 0, sn = 0;
    for (int o = lo; o <= hi; o++) {
      float p = g->post[o];
      if (p < CAND_MIN) continue;
      float y = pyes[kb->col[o] + Q20_WMAX];
      py += p * y;
      sy += plogp(p * y);
      sn += plogp(p * (1.0f - y));
    }
    float pn = 1.0f - py;
    float hy = (py > 1e-9f) ? fast_logf(py) - sy / py : 0.0f;
    float hn = (pn > 1e-9f) ? fast_logf(pn) - sn / pn : 0.0f;
    g->eh[q] = py * hy + pn * hn;
    if (g->eh[q] < best) { best = g->eh[q]; nbest = q; }
  }
  if (nbest < 0) return -1;

  /* A little variety: pick randomly among nearly-as-good questions. */
  int nc = 0, pick;
  for (int q = 0; q < kb->nq; q++) if (g->eh[q] <= best + 0.03f) nc++;
  if (nc <= 1) return nbest;
  pick = rand() % nc;
  for (int q = 0; q < kb->nq; q++)
    if (g->eh[q] <= best + 0.03f && pick-- == 0) return q;
  return nbest;
}

/* Returns 0 = ask a question, 1 = guess *obj, 2 = give up. */
int q20_should_guess(const q20_kb_t *kb, q20_game_t *g, int *obj) {
  float pb;
  int best = q20_posterior(kb, g, &pb);
  int n = q20_counted(g);
  *obj = best;
  if (best < 0) return 2;
  int any_q = 0;
  for (int q = 0; q < kb->nq; q++) if (g->ans[q] == A_NONE) { any_q = 1; break; }

  if (g->last_guess_at == n) {
    if (n >= Q20_MAXASK || !any_q) return 2;
    return 0;
  }
  if (pb >= EARLY_GUESS && n >= 5) return 1;
  if (n >= Q20_GUESS_AT || !any_q)  return 1;
  return 0;
}

int q20_top(const q20_kb_t *kb, q20_game_t *g, int *out, int n) {
  q20_posterior(kb, g, NULL);
  int k = 0;
  for (; k < n; k++) {
    int b = -1;
    for (int o = 0; o < kb->nobj; o++) {
      if (g->rejected[o]) continue;
      int used = 0;
      for (int j = 0; j < k; j++) if (out[j] == o) { used = 1; break; }
      if (used) continue;
      if (b < 0 || g->post[o] > g->post[b]) b = o;
    }
    if (b < 0) break;
    out[k] = b;
  }
  return k;
}

int q20_learn(q20_kb_t *kb, const q20_game_t *g, int obj) {
  if (obj < 0 || obj >= kb->nobj) return -1;
  int fresh = (kb->plays[obj] == 0);
  for (int i = 0; i < g->nasked; i++) {
    int q = g->hist[i];
    int w = get_w(kb, obj, q), nw = w;
    switch (g->ans[q]) {
      case A_YES:   nw = (fresh && w == 0) ?  W_INIT : w + 2; break;
      case A_NO:    nw = (fresh && w == 0) ? -W_INIT : w - 2; break;
      case A_MAYBE: nw += (w > 0) ? -1 : (w < 0) ? 1 : 0;     break;
      default: break;
    }
    if (clampw(nw) != w && q20_set_w(kb, obj, q, nw)) return -1;
  }
  if (kb->plays[obj] < 65535) kb->plays[obj]++;
  return write_plays(kb, obj);
}
