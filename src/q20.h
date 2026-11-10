/*
 * 20Q for DM42 - portable game engine (no DMCP dependencies).
 *
 * Like the original 20Q, the "neural network" is a matrix of learned
 * weights between every object and every question. A weight says how
 * strongly the object tends to answer "yes" (positive) or "no" (negative).
 * Answers are combined as probabilities, so a few inconsistent answers do
 * not send the game down a dead end.
 *
 * The matrix is too big for the calculator's RAM, so it lives in a file
 * that is read through caller-supplied read/write functions. The weights
 * are stored column by column (one column per question), so applying an
 * answer is one contiguous read.
 *
 * File layout (all little endian):
 *   0          header: "Q20D", u16 nobj, u16 nq, u16 maxobj, u16 maxq, 4 x 0
 *   OFF_PLAYS  u16 plays[maxobj]
 *   OFF_QUEST  char quest[maxq][Q20_QLEN]
 *   OFF_NAMES  char name[maxobj][Q20_NAMELEN]
 *   OFF_W      int8 w[maxq][maxobj]          (column-major)
 */
#ifndef Q20_H
#define Q20_H

#include <stdint.h>

#define Q20_MAXOBJ   1200   /* objects the KB can hold            */
#define Q20_MAXQ      200   /* questions the KB can hold          */
#define Q20_NAMELEN    24   /* bytes incl. NUL                    */
#define Q20_QLEN       64   /* bytes incl. NUL                    */
#define Q20_WMAX        8   /* weights are in -WMAX..+WMAX         */

#define Q20_MAXASK     25   /* hard limit on questions in a game  */
#define Q20_GUESS_AT   20   /* must guess after this many         */
#define Q20_NCAT        4   /* columns 0..3: animal/plant/thing/other */

#define Q20_OFF_PLAYS  16u
#define Q20_OFF_QUEST  (Q20_OFF_PLAYS + 2u * Q20_MAXOBJ)
#define Q20_OFF_NAMES  (Q20_OFF_QUEST + (uint32_t)Q20_QLEN * Q20_MAXQ)
#define Q20_OFF_W      (Q20_OFF_NAMES + (uint32_t)Q20_NAMELEN * Q20_MAXOBJ)
#define Q20_FILE_SIZE  (Q20_OFF_W + (uint32_t)Q20_MAXQ * Q20_MAXOBJ)

/* Answers */
enum { A_NONE = 0, A_YES, A_NO, A_MAYBE, A_UNKNOWN };

/* Storage access: return 0 on success */
typedef int q20_rw_fn(void *ctx, uint32_t off, void *buf, unsigned len);

typedef struct {
  int        nobj, nq;
  char     (*quest)[Q20_QLEN];       /* RAM copy of the questions  */
  uint16_t  *plays;                  /* RAM copy of play counts    */
  int8_t    *col;                    /* one column buffer          */
  q20_rw_fn *rd, *wr;
  void      *ctx;
} q20_kb_t;

typedef struct {
  uint8_t ans[Q20_MAXQ];             /* A_* per question            */
  uint8_t hist[Q20_MAXQ];            /* asked questions in order    */
  int     nasked;                    /* answers recorded            */
  int     nfree;                     /* answers that don't count    */
  int     nguess;
  int     last_guess_at;             /* counted questions at guess  */
  uint8_t *rejected;                 /* [Q20_MAXOBJ]                */
  float  *score;                     /* [Q20_MAXOBJ] log-likelihood */
  float  *post;                      /* [Q20_MAXOBJ] posterior      */
  float  *eh;                        /* [Q20_MAXQ] scratch          */
} q20_game_t;

/* Memory for RAM parts. 0 = OK. */
int  q20_alloc(q20_kb_t *kb, q20_game_t *g);

/* Knowledge base file */
int  q20_format(q20_kb_t *kb);     /* write the built-in default KB  */
int  q20_open(q20_kb_t *kb);       /* read header/plays/questions    */
int  q20_name(const q20_kb_t *kb, int obj, char *out);   /* NAMELEN buf */
int  q20_find_object(const q20_kb_t *kb, const char *name);  /* -1 none */
int  q20_add_object(q20_kb_t *kb, const char *name);         /* -1 full */
int  q20_add_question(q20_kb_t *kb, const char *text);       /* -1 full */
int  q20_set_w(q20_kb_t *kb, int obj, int q, int w);

/* Game */
void q20_new_game(const q20_kb_t *kb, q20_game_t *g);
int  q20_counted(const q20_game_t *g);                    /* questions asked */
int  q20_answer(const q20_kb_t *kb, q20_game_t *g, int q, int a);
int  q20_set_category(const q20_kb_t *kb, q20_game_t *g, int cat); /* 0..3 */
int  q20_undo(const q20_kb_t *kb, q20_game_t *g);          /* 0 = nothing */
int  q20_posterior(const q20_kb_t *kb, q20_game_t *g, float *pbest);
int  q20_next_question(const q20_kb_t *kb, q20_game_t *g); /* -1 = none */
int  q20_should_guess(const q20_kb_t *kb, q20_game_t *g, int *obj);
void q20_reject(q20_game_t *g, int obj);
int  q20_top(const q20_kb_t *kb, q20_game_t *g, int *out, int n);

/* Learning: move the target object's weights toward this game's answers */
int  q20_learn(q20_kb_t *kb, const q20_game_t *g, int obj);

/* Text helper: UTF-8 -> ASCII (æ -> ae, ...) */
void q20_to_ascii(const char *in, char *out, int outlen);

#endif
