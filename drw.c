/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <cairo-xlib.h>
#include <pango/pangocairo.h>

#include "drw.h"
#include "util.h"

Drw *
drw_create(Display *dpy, int screen, Window root, unsigned int w, unsigned int h) {
	Drw *drw = (Drw *)calloc(1, sizeof(Drw));

	if(!drw)
		return NULL;
	drw->dpy = dpy;
	drw->screen = screen;
	drw->w = w;
	drw->h = h;
	drw->buf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
	drw->ctx = cairo_create(drw->buf);
	cairo_set_line_width(drw->ctx, 1);

	/* buffering */
	drw->bar = cairo_xlib_surface_create(dpy, root, DefaultVisual(dpy, screen), w, h);
	drw->foo = cairo_create(drw->bar);
	cairo_surface_destroy(drw->bar);

	return drw;
}

void
drw_resize(Drw *drw, unsigned int w, unsigned int h) {
	if(!drw)
		return;
	drw->w = w;
	drw->h = h;
}

void
drw_free(Drw *drw) {
	cairo_surface_finish(drw->buf);
	cairo_destroy(drw->ctx);
	cairo_destroy(drw->foo);
	free(drw);
}

Fnt *
drw_font_create(Drw *drw, const char *fontname) {
	Fnt *font;
	PangoFontDescription *desc;

	font = (Fnt *)calloc(1, sizeof(Fnt));
	if(!font)
		return NULL;

	font->layout = pango_cairo_create_layout(drw->ctx);
	desc = pango_font_description_from_string(fontname);
	pango_layout_set_font_description(font->layout, desc);
	pango_font_description_free(desc);

	pango_layout_get_pixel_size(font->layout, NULL, &font->h);
	return font;
}

void
drw_font_free(Display *dpy, Fnt *font) {
	if(!font)
		return;
	g_object_unref(font->layout);
	free(font);
}

Clr *
drw_clr_create(Drw *drw, const char *clrname) {
	Clr *clr;
	Colormap cmap;
	XColor color;
	char *dup, *str;
	int alpha;

	if(!drw)
		return NULL;
	clr = (Clr *)calloc(1, sizeof(Clr));
	if(!clr)
		return NULL;
	cmap = DefaultColormap(drw->dpy, drw->screen);

	dup = strdup(clrname);
	str = strtok(dup, " ");
	if(!XAllocNamedColor(drw->dpy, cmap, str, &color, &color))
		die("error, cannot allocate color '%s'\n", str);
	clr->r = ((color.pixel >> 16) & 0xFF) / 255.0;
	clr->g = ((color.pixel >> 8) & 0xFF) / 255.0;
	clr->b = (color.pixel & 0xFF) / 255.0;
	clr->rgb = color.pixel;

	str = strtok(NULL, " ");
	alpha = strtol(str, NULL, 10);
	clr->a = (alpha & 0xFF) / 255.0;
	free(dup);
	return clr;
}

void
drw_clr_free(Clr *clr) {
	if(clr)
		free(clr);
}

void
drw_setfont(Drw *drw, Fnt *font) {
	if(drw)
		drw->font = font;
}

void
drw_setscheme(Drw *drw, ClrScheme *scheme) {
	if(drw && scheme)
		drw->scheme = scheme;
}

void
drw_rect(Drw *drw, int x, int y, int filled, int empty) {
	int dx;

	if(!drw || !drw->font || !drw->scheme)
		return;
	dx = (drw->font->h + 2) / 4;
	cairo_set_source_rgb(drw->ctx, drw->scheme->fg->r, drw->scheme->fg->g, drw->scheme->fg->b);
	if(filled) {
		cairo_rectangle(drw->ctx, x+1, y+1, dx+1, dx+1);
		cairo_fill(drw->ctx);
	}
	else if(empty) {
		cairo_rectangle(drw->ctx, x+1.5, y+1.5, dx, dx);
		cairo_stroke(drw->ctx);
	}
}

void
drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, const char *text) {
	//cairo_pattern_t *pat;
	char buf[256];
	int i, tx, ty, th, len, olen;
	Extnts tex;

	if(!drw || !drw->scheme)
		return;
	/*pat = cairo_pattern_create_linear(x, y,  x, h);
	cairo_pattern_add_color_stop_rgba(pat, 1, 0, 0, 0, 1);
	cairo_pattern_add_color_stop_rgba(pat, 0, drw->scheme->bg->r, drw->scheme->bg->g, drw->scheme->bg->b, 0.1);
	cairo_rectangle(drw->ctx, x, y, w, h);
	cairo_set_source(drw->ctx, pat);
	cairo_fill(drw->ctx);
	cairo_pattern_destroy(pat);*/
	cairo_set_source_rgba(drw->ctx, drw->scheme->bg->r, drw->scheme->bg->g, drw->scheme->bg->b, 0.1);
	cairo_rectangle(drw->ctx, x, y, w, h);
	cairo_fill(drw->ctx);
	if(!text || !drw->font)
		return;
	olen = strlen(text);
	drw_font_getexts(drw->font, text, olen, &tex);
	th = drw->font->h;
	ty = y + (h / 2) - (th / 2);
	tx = x + (h / 2);
	/* shorten text if necessary */
	for(len = MIN(olen, sizeof buf); len && (tex.w > w - tex.h || w < tex.h); len--)
		drw_font_getexts(drw->font, text, len, &tex);
	if(!len)
		return;
	memcpy(buf, text, len);
	if(len < olen)
		for(i = len; i && i > len - 3; buf[--i] = '.');
	pango_layout_set_text(drw->font->layout, buf, len);
	cairo_move_to(drw->ctx, tx, ty);
	cairo_set_source_rgb(drw->ctx, drw->scheme->fg->r, drw->scheme->fg->g, drw->scheme->fg->b);
	pango_cairo_update_layout(drw->ctx, drw->font->layout);
	pango_cairo_show_layout(drw->ctx, drw->font->layout);
}

void
drw_clear(Drw *drw) {
	if(!drw)
		return;
	cairo_save(drw->ctx);
	cairo_set_operator(drw->ctx, CAIRO_OPERATOR_CLEAR);
	cairo_paint(drw->ctx);
	cairo_restore(drw->ctx);
}

void
drw_map(Drw *drw, Window barwin, int x, int y, unsigned int w, unsigned int h) {
	if(!drw)
		return;
	cairo_xlib_surface_set_drawable(drw->bar, barwin, w, h);
	cairo_set_source_surface(drw->foo, drw->buf, x, y);
	cairo_paint(drw->foo);
	XSync(drw->dpy, False); //FIXME needed?
}

void
drw_font_getexts(Fnt *font, const char *text, unsigned int len, Extnts *tex) {
	if(!font || !text)
		return;
	pango_layout_set_text(font->layout, text, len);
	pango_layout_get_pixel_size(font->layout, &tex->w, &tex->h);
}

int
drw_font_getexts_width(Fnt *font, const char *text, unsigned int len) {
	Extnts tex;

	if(!font)
		return -1;
	drw_font_getexts(font, text, len, &tex);
	return tex.w;
}

Cur *
drw_cur_create(Drw *drw, int shape) {
	Cur *cur = (Cur *)calloc(1, sizeof(Cur));

	if(!drw || !cur)
		return NULL;
	cur->cursor = XCreateFontCursor(drw->dpy, shape);
	return cur;
}

void
drw_cur_free(Drw *drw, Cur *cursor) {
	if(!drw || !cursor)
		return;
	XFreeCursor(drw->dpy, cursor->cursor);
	free(cursor);
}
