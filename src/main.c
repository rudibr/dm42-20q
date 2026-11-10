/*
 * 20Q for SwissMicros DM42 - DMCP user interface (built against DMCP 3.14,
 * runs on DMCP 3.14 and later, e.g. 3.16).
 *
 * Keys during a game:
 *   F1 / 1  Yes         F4 / 4  Unknown
 *   F2 / 2  No          F5 / <- Undo last answer
 *   F3 / 3  Maybe       F6      Guess now
 *   EXIT    Quit the game
 *
 * The knowledge base lives in /20Q.DAT on the calculator's flash disk
 * (about 280 KB) and grows every time you play.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <main.h>
#include <dmcp.h>

#include "q20.h"

#define KB_FILE   "/20Q.DAT"
#define K_REDRAW  (-2)

static q20_kb_t   kb;
static q20_game_t game;
static FIL       *fil;
static int        fil_open, fil_rw;
static int        shift;             /* SHIFT pressed, waiting for key */
static int        last_shift;        /* was the returned key shifted?  */
static int        last_added = -1;   /* object created this game */

/* ======================================================================
 *  Keyboard with power management (same logic as the SDK demo loop)
 * ====================================================================== */

static int get_key(void) {
  for (;;) {
    if ((ST(STAT_PGM_END) && ST(STAT_SUSPENDED)) ||
        (!ST(STAT_PGM_END) && key_empty())) {
      CLR_ST(STAT_RUNNING);
      sys_sleep();
    }
    if (ST(STAT_PGM_END) || ST(STAT_SUSPENDED)) {
      if (!ST(STAT_SUSPENDED)) {
        lcd_set_buf_cleared(0);
        draw_power_off_image(1);
        LCD_power_off(0);
        SET_ST(STAT_SUSPENDED);
        SET_ST(STAT_OFF);
      }
      continue;
    }
    SET_ST(STAT_RUNNING);
    CLR_ST(STAT_SUSPENDED);

    if (ST(STAT_OFF)) {
      LCD_power_on();
      rtc_wakeup_delay();
      CLR_ST(STAT_OFF);
      return K_REDRAW;              /* caller redraws its screen */
    }

    if (!key_empty()) reset_auto_off();
    int k = key_pop();
    if (k <= 0 || k == KEY_DOUBLE_RELEASE) continue;
    if (k == KEY_SCREENSHOT) { create_screenshot(1); continue; }
    if (k == KEY_SHIFT) { shift = !shift; return K_REDRAW; }  /* show [SHIFT] */
    if (shift && k == KEY_EXIT) {       /* SHIFT+EXIT = OFF, on every screen */
      shift = 0;
      SET_ST(STAT_PGM_END);
      continue;                         /* the loop above powers off */
    }
    last_shift = shift;
    shift = 0;
    return k;
  }
}

/* ======================================================================
 *  Drawing helpers
 * ====================================================================== */

static void header(const char *left, const char *right) {
  int h = lcd_lineHeight(t20);
  lcd_fill_rect(0, 0, LCD_X, h, 1);   /* dark title bar */
  t20->lnfill = 0;                    /* don't wipe the line per write */
  t20->newln = 0;
  t20->inv = 1;
  lcd_setXY(t20, 4, 0);
  lcd_puts(t20, left);
  if (right && *right) {
    lcd_setXY(t20, LCD_X - lcd_textWidth(t20, right) - 4, 0);
    lcd_puts(t20, right);
  }
  if (shift) {
    lcd_setXY(t20, LCD_X / 2 - 30, 0);
    lcd_puts(t20, "[SHIFT]");
  }
  t20->inv = 0;
  t20->newln = 1;
  t20->lnfill = 1;
}

static void center_at(disp_stat_t *ds, int y, const char *s) {
  int w = lcd_textWidth(ds, s);
  int x = (LCD_X - w) / 2;
  lcd_setXY(ds, x < 0 ? 0 : x, y);
  ds->newln = 0;
  ds->lnfill = 0;
  lcd_puts(ds, s);
  ds->lnfill = 1;
  ds->newln = 1;
}

static void left_at(disp_stat_t *ds, int x, int y, const char *s) {
  lcd_setXY(ds, x, y);
  ds->newln = 0;
  ds->lnfill = 0;
  lcd_puts(ds, s);
  ds->lnfill = 1;
  ds->newln = 1;
}

/* Word-wraps text, centered, starting at y. Returns next y. */
static int wrap_at(disp_stat_t *ds, int y, const char *text) {
  char s[160];
  q20_to_ascii(text, s, sizeof(s));
  const int maxw = LCD_X - 16;
  char line[100];
  int  ll = 0;
  const char *p = s;

  while (*p) {
    while (*p == ' ') p++;
    const char *e = p;
    while (*e && *e != ' ') e++;
    int wl = e - p;
    if (!wl) break;
    char trial[100];
    int  tl = 0;
    if (ll) { memcpy(trial, line, ll); trial[ll] = ' '; tl = ll + 1; }
    if (tl + wl >= (int)sizeof(trial)) wl = sizeof(trial) - tl - 1;
    memcpy(trial + tl, p, wl);
    trial[tl + wl] = 0;
    if (ll && lcd_textWidth(ds, trial) > maxw) {
      line[ll] = 0;
      center_at(ds, y, line);
      y += lcd_lineHeight(ds);
      ll = 0;
      continue;                       /* retry this word on a new line */
    }
    memcpy(line, trial, tl + wl + 1);
    ll = tl + wl;
    p = e;
  }
  if (ll) {
    line[ll] = 0;
    center_at(ds, y, line);
    y += lcd_lineHeight(ds);
  }
  return y;
}

static void menu(const char *k1, const char *k2, const char *k3,
                 const char *k4, const char *k5, const char *k6) {
  const char *keys[6] = { k1 ? k1 : "", k2 ? k2 : "", k3 ? k3 : "",
                          k4 ? k4 : "", k5 ? k5 : "", k6 ? k6 : "" };
  lcd_draw_menu_keys(keys);
}

static int body_top(void) { return lcd_lineHeight(t20) + 10; }
static int hint_y(void)   { return LCD_Y - LCD_MENU_LINES - lcd_lineHeight(t20) - 2; }

/* Simple full screen message. Waits for a key, returns it. */
static int message(const char *title, const char *l1, const char *l2, const char *l3) {
  for (;;) {
    lcd_clear_buf();
    header(title, "");
    int y = body_top() + 20;
    if (l1) y = wrap_at(t24, y, l1) + 8;
    if (l2) y = wrap_at(t24, y, l2) + 8;
    if (l3) wrap_at(t20, y, l3);
    center_at(t20, hint_y(), "Press any key");
    lcd_refresh();
    int k = get_key();
    if (k != K_REDRAW) return k;
  }
}

/* Message without waiting (for "please wait") */
static void busy(const char *title, const char *l1) {
  lcd_clear_buf();
  header(title, "");
  wrap_at(t24, body_top() + 50, l1);
  lcd_refresh();
}

/* Yes/no screen. Returns 1 = yes, 0 = no, -1 = EXIT. */
static int ask_yes_no(const char *title, const char *l1, const char *l2) {
  for (;;) {
    lcd_clear_buf();
    header(title, "");
    int y = body_top() + 20;
    y = wrap_at(t24, y, l1) + 8;
    if (l2) wrap_at(t24, y, l2);
    menu("Yes", "No", "", "", "", "");
    lcd_refresh();
    switch (get_key()) {
      case KEY_F1: case KEY_1: return 1;
      case KEY_F2: case KEY_2: return 0;
      case KEY_EXIT:           return -1;
    }
  }
}

/* ======================================================================
 *  Text entry using the letters printed on the DM42 keys
 * ====================================================================== */

/* Letters as printed on the DM42 keys (to the right of each key) */
static char alpha_of(int key) {
  switch (key) {
    case KEY_SIGMA: return 'a';  case KEY_INV:  return 'b';
    case KEY_SQRT:  return 'c';  case KEY_LOG:  return 'd';
    case KEY_LN:    return 'e';  case KEY_XEQ:  return 'f';
    case KEY_STO:   return 'g';  case KEY_RCL:  return 'h';
    case KEY_RDN:   return 'i';  case KEY_SIN:  return 'j';
    case KEY_COS:   return 'k';  case KEY_TAN:  return 'l';
    case KEY_SWAP:  return 'm';  case KEY_CHS:  return 'n';
    case KEY_E:     return 'o';
    case KEY_7:     return 'p';  case KEY_8:    return 'q';
    case KEY_9:     return 'r';  case KEY_DIV:  return 's';
    case KEY_4:     return 't';  case KEY_5:    return 'u';
    case KEY_6:     return 'v';  case KEY_MUL:  return 'w';
    case KEY_1:     return 'x';  case KEY_2:    return 'y';
    case KEY_3:     return 'z';  case KEY_SUB:  return '-';
    case KEY_0:     return ' ';  case KEY_DOT:  return '.';
    case KEY_RUN:   return '?';  case KEY_ADD:  return ' ';
  }
  return 0;
}

static char digit_of(int key) {
  switch (key) {
    case KEY_0: return '0'; case KEY_1: return '1'; case KEY_2: return '2';
    case KEY_3: return '3'; case KEY_4: return '4'; case KEY_5: return '5';
    case KEY_6: return '6'; case KEY_7: return '7'; case KEY_8: return '8';
    case KEY_9: return '9';
  }
  return 0;
}

static void str_addc(char *buf, int max, char c) {
  int l = strlen(buf);
  if (l + 1 < max) { buf[l] = c; buf[l + 1] = 0; }
}

/* Returns 1 = OK, 0 = cancelled. */
static int text_entry(const char *title, const char *prompt, char *buf, int max) {
  buf[0] = 0;
  shift = 0;
  for (;;) {
    lcd_clear_buf();
    header(title, "");
    int y = wrap_at(t20, body_top(), prompt) + 14;
    char shown[100];
    snprintf(shown, sizeof(shown), "%s_", buf);
    const char *v = shown;                 /* keep the end visible */
    while (lcd_textWidth(t24, v) > LCD_X - 16) v++;
    center_at(t24, y, v);
    center_at(t20, hint_y() - lcd_lineHeight(t20), "Letters as printed on the keys, + = space");
    center_at(t20, hint_y(), "SHIFT+key = digit  R/S = ?  EXIT = cancel");
    menu("Space", "-", "'", "", "Del", "OK");
    lcd_refresh();

    int k = get_key();
    if (k == K_REDRAW) continue;
    int was_shift = last_shift;
    switch (k) {
      case KEY_EXIT: return 0;
      case KEY_F1:   str_addc(buf, max, ' ');  continue;
      case KEY_F2:   str_addc(buf, max, '-');  continue;
      case KEY_F3:   str_addc(buf, max, '\''); continue;
      case KEY_F5:
      case KEY_BSP:  { int l = strlen(buf); if (l) buf[l - 1] = 0; } continue;
      case KEY_F6: {
        int l = strlen(buf);
        while (l && buf[l-1] == ' ') buf[--l] = 0;
        char *s = buf;
        while (*s == ' ') s++;
        memmove(buf, s, strlen(s) + 1);
        if (buf[0]) return 1;
        continue;
      }
    }
    char c = was_shift ? digit_of(k) : 0;
    if (!c) c = alpha_of(k);
    if (c) str_addc(buf, max, c);
  }
}

/* ======================================================================
 *  Knowledge base file
 *  Normally open read-only. Writes reopen it read/write and close it
 *  again afterwards, so the file is always complete on disk (DMCP 3.14
 *  has no f_sync).
 * ====================================================================== */

static int fs_rd(void *ctx, uint32_t off, void *b, unsigned n) {
  UINT br;
  if (!fil_open || f_lseek(fil, off) != FR_OK) return -1;
  return (f_read(fil, b, n, &br) != FR_OK || br != n) ? -1 : 0;
}
static int fs_wr(void *ctx, uint32_t off, void *b, unsigned n) {
  UINT bw;
  if (!fil_open || !fil_rw || f_lseek(fil, off) != FR_OK) return -1;
  return (f_write(fil, b, n, &bw) != FR_OK || bw != n) ? -1 : 0;
}

static void kb_close(void) {
  if (fil_open) f_close(fil);
  if (fil_rw) sys_disk_write_enable(0);
  fil_open = fil_rw = 0;
}

static int kb_open_ro(void) {
  kb_close();
  if (!sys_disk_ok()) return -1;
  if (f_open(fil, KB_FILE, FA_READ) != FR_OK) return -1;
  fil_open = 1;
  return 0;
}

static int kb_begin_write(int create) {
  kb_close();
  if (!sys_disk_ok()) return -1;
  sys_disk_write_enable(1);
  BYTE mode = FA_READ | FA_WRITE | (create ? FA_CREATE_ALWAYS : FA_OPEN_EXISTING);
  if (f_open(fil, KB_FILE, mode) != FR_OK) { sys_disk_write_enable(0); return -1; }
  fil_open = fil_rw = 1;
  return 0;
}

static int kb_end_write(void) {
  int err = 0;
  if (fil_open && f_close(fil) != FR_OK) err = -1;
  fil_open = 0;
  if (fil_rw) sys_disk_write_enable(0);
  fil_rw = 0;
  if (kb_open_ro()) err = -1;
  return err;
}

static void write_failed(void) {
  message("20Q", "Could not write to the disk!",
          "Is the calculator connected as a USB disk?", NULL);
}

/* Build /20Q.DAT from the built-in knowledge base. */
static int kb_create(void) {
  busy("20Q", "Building the knowledge base... please wait.");
  int err = kb_begin_write(1);
  if (!err) err = q20_format(&kb);
  if (kb_end_write()) err = -1;
  if (!err) err = q20_open(&kb);
  return err;
}

/* Opens the file, or creates it. Returns 0 when the KB is usable. */
static int kb_startup(void) {
  if (!kb_open_ro() && !q20_open(&kb)) return 0;
  return kb_create();
}

/* ======================================================================
 *  Setup menu (SHIFT+0, or F6 on the title screen)
 * ====================================================================== */

#define MI_ABOUT_20Q  11
#define MI_RESET_KB   12

static const uint8_t mid_menu[] = {
  MI_RESET_KB, MI_MSC, MI_SYSTEM_ENTER, MI_ABOUT_20Q, 0 };
static const smenu_t MID_MENU = { "Setup", mid_menu, NULL, NULL };

static void about(void) {
  char l3[64];
  snprintf(l3, sizeof(l3), "%d things, %d questions, %d bytes free",
           kb.nobj, kb.nq, sys_free_mem());
  message("About 20Q", "20Q v" PROGRAM_VERSION " for the DM42",
          "Think of something - I will guess it in 20 questions or less. "
          "I learn every time you play.", l3);
}

static int run_menu_item(uint8_t line_id) {
  switch (line_id) {
    case MI_ABOUT_20Q:
      about();
      return 0;
    case MI_RESET_KB:
      if (ask_yes_no("Reset", "Forget everything I have learned?",
                     "The built-in knowledge base is restored.") == 1) {
        if (kb_create()) write_failed();
      }
      return 0;
  }
  return MRET_UNIMPL;
}

static const char *menu_line_str(uint8_t line_id, char *s, const int slen) {
  switch (line_id) {
    case MI_ABOUT_20Q: return "About 20Q >";
    case MI_RESET_KB:  return "Reset knowledge base >";
  }
  return NULL;
}

static void open_setup(void) {
  kb_close();                         /* the menu may switch on USB disk */
  SET_ST(STAT_MENU);
  handle_menu(&MID_MENU, MENU_RESET, 0);
  CLR_ST(STAT_MENU);
  wait_for_key_release(-1);
  if (kb_startup())
    message("20Q", "The knowledge base is not available.",
            "Switch off USB disk mode and try again.", NULL);
}

/* ======================================================================
 *  Game screens
 * ====================================================================== */

enum { ACT_YES = 1, ACT_NO, ACT_MAYBE, ACT_UNKNOWN, ACT_UNDO, ACT_GUESS, ACT_QUIT };

static void q_title(char *buf, int len) {
  snprintf(buf, len, "Question %d", q20_counted(&game) + 1);
}

/* Returns 0..3 for the category, 4 = unsure, -1 = quit */
static int category_screen(void) {
  char right[24];
  q_title(right, sizeof(right));
  for (;;) {
    lcd_clear_buf();
    header("20Q", right);
    int y = body_top() + 30;
    center_at(t24, y, "Is it Animal, Vegetable,");
    center_at(t24, y + lcd_lineHeight(t24), "Thing or Other?");
    center_at(t20, hint_y(), "Vegetable = any plant.  Other = food, place, ...");
    menu("Animal", "Plant", "Thing", "Other", "Unsure", "");
    lcd_refresh();
    switch (get_key()) {
      case KEY_F1: case KEY_1: return 0;
      case KEY_F2: case KEY_2: return 1;
      case KEY_F3: case KEY_3: return 2;
      case KEY_F4: case KEY_4: return 3;
      case KEY_F5: case KEY_5: return 4;
      case KEY_EXIT:           return -1;
    }
  }
}

static int question_screen(int q) {
  char right[24];
  q_title(right, sizeof(right));
  for (;;) {
    lcd_clear_buf();
    header("20Q", right);
    int lines = 1, wpx = lcd_textWidth(t24, kb.quest[q]);
    if (wpx > LCD_X - 16) lines = 2 + (wpx > 2 * (LCD_X - 40));
    int y = (body_top() + hint_y()) / 2 - lines * lcd_lineHeight(t24) / 2 - 6;
    wrap_at(t24, y, kb.quest[q]);
    menu("Yes", "No", "Maybe", "Unknown", "Undo", "Guess!");
    lcd_refresh();
    switch (get_key()) {
      case KEY_F1: case KEY_1:   return ACT_YES;
      case KEY_F2: case KEY_2:   return ACT_NO;
      case KEY_F3: case KEY_3:   return ACT_MAYBE;
      case KEY_F4: case KEY_4:   return ACT_UNKNOWN;
      case KEY_F5: case KEY_BSP: return ACT_UNDO;
      case KEY_F6:               return ACT_GUESS;
      case KEY_EXIT:             return ACT_QUIT;
    }
  }
}

/* Returns 1 = right, 0 = wrong, -1 = quit */
static int guess_screen(int obj) {
  char right[32], nm[Q20_NAMELEN], up[Q20_NAMELEN + 2];
  snprintf(right, sizeof(right), "After %d questions", q20_counted(&game));
  q20_name(&kb, obj, nm);
  snprintf(up, sizeof(up), "%s?", nm);
  for (char *p = up; *p; p++) if (*p >= 'a' && *p <= 'z') *p -= 'a' - 'A';
  for (;;) {
    lcd_clear_buf();
    header("20Q", right);
    int y = body_top() + 18;
    center_at(t24, y, "I am guessing that it is...");
    y += lcd_lineHeight(t24) + 16;
    lcd_switchFont(fReg, 3);
    if (lcd_textWidth(fReg, up) > LCD_X - 8) lcd_switchFont(fReg, 1);
    center_at(fReg, y, up);
    menu("Right", "Wrong", "", "", "", "");
    lcd_refresh();
    switch (get_key()) {
      case KEY_F1: case KEY_1: return 1;
      case KEY_F2: case KEY_2: return 0;
      case KEY_EXIT:           return -1;
    }
  }
}

/* "You win" - let the player tell us what it was. Returns object or -1. */
static int which_was_it(void) {
  int  top[6];
  char names[6][Q20_NAMELEN];
  int  n = q20_top(&kb, &game, top, 6);
  for (int i = 0; i < n; i++) q20_name(&kb, top[i], names[i]);
  for (;;) {
    lcd_clear_buf();
    header("You win!", "");
    int y = body_top();
    center_at(t24, y, "What were you thinking of?");
    y += lcd_lineHeight(t24) + 4;
    for (int i = 0; i < n; i++) {
      char s[Q20_NAMELEN + 6];
      snprintf(s, sizeof(s), "%d  %s", i + 1, names[i]);
      left_at(t20, 30 + (i % 2) * 190, y + (i / 2) * lcd_lineHeight(t20), s);
    }
    y += ((n + 1) / 2) * lcd_lineHeight(t20) + 6;
    center_at(t20, y, "0 = something else (type it)   EXIT = skip");
    menu("", "", "", "", "", "Type");
    lcd_refresh();

    int k = get_key(), d = -1;
    switch (k) {
      case KEY_1: d = 0; break; case KEY_2: d = 1; break;
      case KEY_3: d = 2; break; case KEY_4: d = 3; break;
      case KEY_5: d = 4; break; case KEY_6: d = 5; break;
      case KEY_EXIT: return -1;
      case KEY_0: case KEY_F6: {
        char name[Q20_NAMELEN];
        if (!text_entry("New thing", "What were you thinking of? (e.g. okapi)",
                        name, sizeof(name)))
          continue;
        int o = q20_find_object(&kb, name);
        if (o >= 0) return o;
        if (kb_begin_write(0)) { kb_end_write(); write_failed(); return -1; }
        o = q20_add_object(&kb, name);
        if (kb_end_write()) o = -1;
        last_added = o;
        if (o < 0) {
          if (kb.nobj >= Q20_MAXOBJ)
            message("20Q", "My memory is full.", "Reset the knowledge base in Setup.", NULL);
          else
            write_failed();
        }
        return o;
      }
    }
    if (d >= 0 && d < n) return top[d];
  }
}

/* Offer to teach a new question that describes obj. */
static void teach_question(int obj) {
  if (kb.nq >= Q20_MAXQ) return;
  char nm[Q20_NAMELEN], l1[96];
  q20_name(&kb, obj, nm);
  snprintf(l1, sizeof(l1), "Teach me a new question about %s?", nm);
  if (ask_yes_no("New question", l1, "It helps me tell things apart.") != 1) return;

  char text[Q20_QLEN];
  if (!text_entry("New question", "Type a yes/no question, e.g. does it have a long neck",
                  text, sizeof(text) - 2))
    return;
  if (text[0] >= 'a' && text[0] <= 'z') text[0] -= 'a' - 'A';
  int l = strlen(text);
  if (text[l - 1] != '?') { text[l] = '?'; text[l + 1] = 0; }

  snprintf(l1, sizeof(l1), "What is the answer for %s?", nm);
  int a = ask_yes_no("New question", text, l1);
  if (a < 0) return;
  int err = kb_begin_write(0);
  if (!err) {
    int q = q20_add_question(&kb, text);
    err = (q < 0) || q20_set_w(&kb, obj, q, a ? 6 : -6);
  }
  if (kb_end_write() || err) write_failed();
}

static void learn(int obj) {
  int err = kb_begin_write(0);
  if (!err) err = q20_learn(&kb, &game, obj);
  if (kb_end_write() || err) write_failed();
}

static void play_game(void) {
  q20_new_game(&kb, &game);
  int force_guess = 0, cat_asked = 0;
  last_added = -1;

  for (;;) {
    if (!cat_asked) {                     /* always start like the original */
      int c = category_screen();
      if (c < 0) return;
      if (c < Q20_NCAT) q20_set_category(&kb, &game, c);
      cat_asked = 1;                      /* 4 = unsure: just carry on */
    }

    int obj;
    int r = q20_should_guess(&kb, &game, &obj);
    if (force_guess && game.last_guess_at != q20_counted(&game) && obj >= 0) r = 1;
    force_guess = 0;

    if (r == 1) {
      int ok = guess_screen(obj);
      if (ok < 0) return;
      if (ok) {
        char l2[64];
        snprintf(l2, sizeof(l2), "I guessed it in %d questions.", q20_counted(&game));
        message("I win!", "Got it!", l2, NULL);
        learn(obj);
        return;
      }
      q20_reject(&game, obj);
      continue;
    }

    if (r == 2) {
      int o = which_was_it();
      if (o < 0) return;
      int is_new = (o == last_added);
      learn(o);
      char nm[Q20_NAMELEN], l1[64];
      q20_name(&kb, o, nm);
      snprintf(l1, sizeof(l1), "Thanks! Now I know more about %s.", nm);
      message("Thanks", l1, NULL, NULL);
      if (is_new) teach_question(o);
      return;
    }

    int q = q20_next_question(&kb, &game);
    if (q < 0) { force_guess = 1; continue; }
    switch (question_screen(q)) {
      case ACT_YES:     q20_answer(&kb, &game, q, A_YES);     break;
      case ACT_NO:      q20_answer(&kb, &game, q, A_NO);      break;
      case ACT_MAYBE:   q20_answer(&kb, &game, q, A_MAYBE);   break;
      case ACT_UNKNOWN: q20_answer(&kb, &game, q, A_UNKNOWN); break;
      case ACT_UNDO:
        q20_undo(&kb, &game);
        if (game.nasked == 0) cat_asked = 0;   /* back to the first question */
        break;
      case ACT_GUESS:   force_guess = 1;                      break;
      case ACT_QUIT:
        if (ask_yes_no("20Q", "Quit this game?", NULL) == 1) return;
        break;
    }
  }
}

static void title_screen(void) {
  for (;;) {
    lcd_clear_buf();
    header("20Q - Twenty Questions", "v" PROGRAM_VERSION);
    int y = body_top() + 6;
    lcd_switchFont(fReg, 4);
    center_at(fReg, y, "20Q");
    y += lcd_lineHeight(fReg) + 4;
    y = wrap_at(t24, y, "Think of an animal, a plant, a thing or something else.") + 4;
    char s[80];
    snprintf(s, sizeof(s), "I know %d things and %d questions.", kb.nobj, kb.nq);
    center_at(t20, y, s);
    center_at(t20, hint_y(), "Press any key to start");
    menu("Play", "", "", "", "About", "Setup");
    lcd_refresh();

    int k = get_key();
    if (k == K_REDRAW) continue;
    if (last_shift && k == KEY_0) { open_setup(); continue; }   /* SETUP */
    switch (k) {
      case KEY_F5:   about(); break;
      case KEY_F6:   open_setup(); break;
      case KEY_EXIT: break;
      default:
        if (!fil_open && kb_startup()) {
          message("20Q", "The knowledge base is not available.",
                  "Switch off USB disk mode and try again.", NULL);
          break;
        }
        play_game();
        break;
    }
  }
}

/* ======================================================================
 *  Entry point
 * ====================================================================== */

/* newlib's abort() needs this; a DMCP program never really exits */
void _exit(int status) {
  (void)status;
  for (;;) sys_sleep();
}

void program_main() {
  run_menu_item_app = run_menu_item;
  menu_line_str_app = menu_line_str;

  srand(sys_current_ms());
  fil = malloc(sizeof(FIL));
  if (!fil || q20_alloc(&kb, &game)) {
    lcd_clear_buf();
    lcd_putsAt(t24, 2, "20Q: not enough memory");
    lcd_refresh();
    for (;;) get_key();
  }
  kb.rd = fs_rd;
  kb.wr = fs_wr;
  kb.ctx = NULL;
  if (kb_startup())
    message("20Q", "Could not open or create /20Q.DAT.",
            "Is the calculator connected as a USB disk?", NULL);
  title_screen();
}
