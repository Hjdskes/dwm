/* See LICENSE file for copyright and license details. */

#include <cairo-xlib.h>
#include <pango/pango.h>

typedef struct {
	double r;
	double g;
	double b;
	unsigned long rgb;
} Clr;

typedef struct {
	Cursor cursor;
} Cur;

typedef struct {
	int h;
	PangoLayout *layout;
} Fnt;

typedef struct {
	Clr *fg;
	Clr *bg;
	Clr *border;
} ClrScheme;

typedef struct {
	unsigned int w, h;
	cairo_t *ctx;
	cairo_surface_t *sur;
	ClrScheme *scheme;
	Fnt *font;
} Drw;

typedef struct {
	int w;
	int h;
} Extnts;

/* Drawable abstraction */
Drw *drw_create(Display *dpy, int screen, Window root, unsigned int w, unsigned int h);
void drw_resize(Drw *drw, unsigned int w, unsigned int h);
void drw_setdrawable(Drw *drw, Window barwin, unsigned int w, unsigned int h);
void drw_free(Drw *drw);

/* Fnt abstraction */
Fnt *drw_font_create(const char *fontname);
void drw_font_free(Fnt *font);
void drw_font_getexts(Fnt *font, const char *text, unsigned int len, Extnts *extnts);
int drw_font_getexts_width(Fnt *font, const char *text, unsigned int len);

/* Colour abstraction */
Clr *drw_clr_create(Display *dpy, int screen, const char *clrname);
void drw_clr_free(Clr *clr);

/* Cursor abstraction */
Cur *drw_cur_create(Display *dpy, int shape);
void drw_cur_free(Display *dpy, Cur *cursor);

/* Drawing context manipulation */
void drw_setfont(Drw *drw, Fnt *font);
void drw_setscheme(Drw *drw, ClrScheme *scheme);

/* Drawing functions */
void drw_rect(Drw *drw, int x, int y, int filled, int empty);
void drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, const char *text);
