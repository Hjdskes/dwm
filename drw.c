/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <pango/pangoxft.h>

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
	drw->drawable = XCreatePixmap(dpy, root, w, h, DefaultDepth(dpy, screen));
	drw->xftdrawable = XftDrawCreate(dpy, drw->drawable, DefaultVisual(dpy, screen), DefaultColormap(dpy, screen));
	drw->gc = XCreateGC(dpy, root, 0, NULL);
	XSetLineAttributes(dpy, drw->gc, 1, LineSolid, CapButt, JoinMiter);
	return drw;
}

void
drw_resize(Drw *drw, unsigned int w, unsigned int h) {
	if(!drw)
		return;
	drw->w = w;
	drw->h = h;
	if(drw->drawable != 0)
		XFreePixmap(drw->dpy, drw->drawable);
	drw->drawable = XCreatePixmap(drw->dpy, drw->root, w, h, DefaultDepth(drw->dpy, drw->screen));
}

void
drw_free(Drw *drw) {
	XFreePixmap(drw->dpy, drw->drawable);
	XftDrawDestroy(drw->xftdrawable);
	XFreeGC(drw->dpy, drw->gc);
	free(drw);
}

Fnt *
drw_font_create(Display *dpy, int screen, const char *fontname) {
	Fnt *font;
	PangoContext *context;
	PangoFontMap *fontmap;
	PangoFontDescription *desc;

	font = (Fnt *)calloc(1, sizeof(Fnt));
	if(!font)
		return NULL;

	fontmap = pango_xft_get_font_map(dpy, screen);
	context = pango_font_map_create_context(fontmap);
	font->layout = pango_layout_new(context);
	g_object_unref(context);
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
	Visual *vis;
	XftColor color;

	if(!drw)
		return NULL;
	clr = (Clr *)calloc(1, sizeof(Clr));
	if(!clr)
		return NULL;
	cmap = DefaultColormap(drw->dpy, drw->screen);
	vis = DefaultVisual(drw->dpy, drw->screen);
	if(!XftColorAllocName(drw->dpy, vis, cmap, clrname, &color))
		die("error, cannot allocate color '%s'\n", clrname);
	clr->rgb = color;
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
	XSetForeground(drw->dpy, drw->gc, drw->scheme->fg->rgb.pixel);
	dx = (drw->font->h + 2) / 4;
	if(filled)
		XFillRectangle(drw->dpy, drw->drawable, drw->gc, x+1, y+1, dx+1, dx+1);
	else if(empty)
		XDrawRectangle(drw->dpy, drw->drawable, drw->gc, x+1, y+1, dx, dx);
}

void
drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, const char *text) {
	char buf[256];
	int i, tx, ty, len, olen;
	Extnts tex;

	if(!drw || !drw->scheme)
		return;
	XSetForeground(drw->dpy, drw->gc, drw->scheme->bg->rgb.pixel);
	XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);
	if(!text || !drw->font)
		return;
	olen = strlen(text);
	drw_font_getexts(drw->font, text, olen, &tex);
	ty = y + (h / 2) - (drw->font->h / 2);
	tx = x + (h / 2);
	/* shorten text if necessary */
	for(len = MIN(olen, sizeof buf); len && (tex.w > w - tex.h || w < tex.h); len--)
		drw_font_getexts(drw->font, text, len, &tex);
	if(!len)
		return;
	memcpy(buf, text, len);
	if(len < olen)
		for(i = len; i && i > len - 3; buf[--i] = '.');
	XSetForeground(drw->dpy, drw->gc, drw->scheme->fg->rgb.pixel);
	pango_layout_set_text(drw->font->layout, buf, len);
	pango_xft_render_layout(drw->xftdrawable, &drw->scheme->fg->rgb, drw->font->layout, tx * PANGO_SCALE, ty * PANGO_SCALE);
}

void
drw_map(Drw *drw, Window barwin, int x, int y, unsigned int w, unsigned int h) {
	if(!drw)
		return;
	XCopyArea(drw->dpy, drw->drawable, barwin, drw->gc, x, y, w, h, x, y);
	XSync(drw->dpy, False);
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
