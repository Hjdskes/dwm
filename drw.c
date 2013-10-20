/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

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
	XFreeGC(drw->dpy, drw->gc);
	free(drw);
}

Fnt *
drw_font_create(Display *dpy, const char *fontname) {
	Fnt *font;

	font = (Fnt *)calloc(1, sizeof(Fnt));
	if(!font)
		return NULL;
	if(!(font->xfont = XftFontOpenName(dpy, DefaultScreen(dpy), fontname))
			&& !(font->xfont = XftFontOpenName(dpy, DefaultScreen(dpy), "fixed")))
		die("error, cannot load font: '%s'\n", fontname);
	font->ascent = font->xfont->ascent;
	font->descent = font->xfont->descent;
	font->h = font->ascent + font->descent;
	return font;
}

void
drw_font_free(Display *dpy, Fnt *font) {
	if(!font)
		return;
	free(font);
}

Clr *
drw_clr_create(Drw *drw, const char *clrname) {
	Clr *clr;
	XftColor color;

	if(!drw)
		return NULL;
	clr = (Clr *)calloc(1, sizeof(Clr));
	if(!clr)
		return NULL;
	if(!XftColorAllocName(drw->dpy, DefaultVisual(drw->dpy, drw->screen), DefaultColormap(drw->dpy, drw->screen), clrname, &color))
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
drw_text(Drw *drw, int x, int y, unsigned int w, unsigned int h, const char *text, int invert) {
	char buf[256];
	int i, tx, ty, th, len, olen;
	Extnts tex;
	XftDraw *d;

	if(!drw || !drw->scheme)
		return;
	XSetForeground(drw->dpy, drw->gc, invert ? drw->scheme->fg->rgb.pixel : drw->scheme->bg->rgb.pixel);
	XFillRectangle(drw->dpy, drw->drawable, drw->gc, x, y, w, h);
	if(!text || !drw->font)
		return;
	olen = strlen(text);
	drw_font_getexts(drw->dpy, drw->font, text, olen, &tex);
	th = drw->font->ascent + drw->font->descent;
	ty = y + (h / 2) - (th / 2) + drw->font->ascent;
	tx = x + (h / 2);
	/* shorten text if necessary */
	for(len = MIN(olen, sizeof buf); len && (tex.w > w - tex.h || w < tex.h); len--)
		drw_font_getexts(drw->dpy, drw->font, text, len, &tex);
	if(!len)
		return;
	memcpy(buf, text, len);
	if(len < olen)
		for(i = len; i && i > len - 3; buf[--i] = '.');
	d = XftDrawCreate(drw->dpy, drw->drawable, DefaultVisual(drw->dpy, drw->screen), DefaultColormap(drw->dpy, drw->screen));
	XftDrawStringUtf8(d, invert ? &drw->scheme->bg->rgb : &drw->scheme->fg->rgb, drw->font->xfont, tx, ty, (XftChar8 *) buf, len);
	XftDrawDestroy(d);
}

void
drw_map(Drw *drw, Window win, int x, int y, unsigned int w, unsigned int h) {
	if(!drw)
		return;
	XCopyArea(drw->dpy, drw->drawable, win, drw->gc, x, y, w, h, x, y);
	XSync(drw->dpy, False);
}


void
drw_font_getexts(Display *dpy, Fnt *font, const char *text, unsigned int len, Extnts *tex) {
	XGlyphInfo ext;

	if(!font || !text)
		return;
	else {
		XftTextExtentsUtf8(dpy, font->xfont, (XftChar8 *) text, len, &ext);
		tex->h = font->ascent + font->descent;
		tex->w = ext.width;
	}
}

unsigned int
drw_font_getexts_width(Display *dpy, Fnt *font, const char *text, unsigned int len) {
	Extnts tex;

	if(!font)
		return -1;
	drw_font_getexts(dpy, font, text, len, &tex);
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
