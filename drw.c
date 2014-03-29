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
	drw->root = root;
	drw->w = w;
	drw->h = h;
	drw->surface = cairo_xlib_surface_create(dpy, root, DefaultVisual(dpy, screen), w, h);
	drw->context = cairo_create(drw->surface);
	cairo_set_line_width(drw->context, 1);
	return drw;
}

void
drw_resize(Drw *drw, unsigned int w, unsigned int h) {
	if(!drw)
		return;
	drw->w = w;
	drw->h = h;
	if(drw->surface)
		cairo_xlib_surface_set_size(drw->surface, w, h);
}

void
drw_free(Drw *drw) {
	cairo_surface_finish(drw->surface);
	cairo_destroy(drw->context);
	free(drw);
}

Fnt * //FIXME: check if this is optimal with cairo
drw_font_create(Drw *drw, const char *fontname) {
	Fnt *font;
	PangoContext *context;
	PangoFontDescription *desc;
	PangoFontMetrics *metrics;

	font = (Fnt *)calloc(1, sizeof(Fnt));
	if(!font)
		return NULL;

	context = pango_cairo_create_context(drw->context);
	desc = pango_font_description_from_string(fontname);
	font->layout = pango_layout_new(context);
	pango_layout_set_font_description(font->layout, desc);

	metrics = pango_context_get_metrics(context, desc, NULL);
	font->ascent = pango_font_metrics_get_ascent(metrics) / PANGO_SCALE;
	font->descent = pango_font_metrics_get_descent(metrics) / PANGO_SCALE;
	font->h = font->ascent + font->descent;

	pango_font_metrics_unref(metrics);
	pango_font_description_free(desc);
	g_object_unref(context);
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

	if(!drw)
		return NULL;
	clr = (Clr *)calloc(1, sizeof(Clr));
	if(!clr)
		return NULL;
	cmap = DefaultColormap(drw->dpy, drw->screen);
	if(!XAllocNamedColor(drw->dpy, cmap, clrname, &color, &color))
		die("error, cannot allocate color '%s'\n", clrname);
	clr->r = ((color.pixel >> 16) & 0xFF) / 255.0;
	clr->g = ((color.pixel >> 8) & 0xFF) / 255.0;
	clr->b = (color.pixel & 0xFF) / 255.0;
	clr->rgb = color.pixel;

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
	cairo_set_source_rgb(drw->context, drw->scheme->fg->r, drw->scheme->fg->g, drw->scheme->fg->b);
	dx = (drw->font->ascent + drw->font->descent + 2) / 4;
	if(filled) {
		cairo_rectangle(drw->context, x+1, y+1, dx+1, dx+1);
		cairo_fill(drw->context);
	}
	else if(empty) {
		cairo_rectangle(drw->context, x+1.5, y+1.5, dx, dx);
		cairo_stroke(drw->context);
	}
}

void //TODO clipping? save/restore?
drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, const char *text) {
	char buf[256];
	int i, tx, ty, th, len, olen;
	Extnts tex;

	if(!drw || !drw->scheme)
		return;
	cairo_set_source_rgb(drw->context, drw->scheme->bg->r, drw->scheme->bg->g, drw->scheme->bg->b);
	cairo_rectangle(drw->context, x, y, w, h);
	cairo_fill(drw->context);
	if(!text || !drw->font)
		return;
	olen = strlen(text);
	drw_font_getexts(drw->font, text, olen, &tex);
	th = drw->font->ascent + drw->font->descent;
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
	cairo_move_to(drw->context, tx, ty);
	cairo_set_source_rgb(drw->context, drw->scheme->fg->r, drw->scheme->fg->g, drw->scheme->fg->b);
	pango_cairo_update_layout(drw->context, drw->font->layout);
	pango_cairo_show_layout(drw->context, drw->font->layout);
}

void
drw_map(Drw *drw, Window win, int x, int y, unsigned int w, unsigned int h) {
	if(!drw)
		return;
	cairo_xlib_surface_set_drawable(drw->surface, win, w, h);
	cairo_set_source_surface(drw->context, drw->surface, x, y);
	cairo_paint(drw->context);
	//TODO needed? cairo_show_page(drw->context);
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
