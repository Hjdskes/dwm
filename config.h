/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

#define MODKEY Mod1Mask
#define MONKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* appearance */
static const char font[]                 = "Cantarell 9";
static const char normbordercolor[]      = "#353535";
static const char normbgcolor[]          = "#353535";
static const char normfgcolor[]          = "#F9F9F9";
static const char selbordercolor[]       = "#4A90D9";
static const char selbgcolor[]           = "#353535";
static const char selfgcolor[]           = "#4A90D9";
static const char urgbgcolor[]           = "#353535";
static const char urgfgcolor[]           = "#DC322F";
static const unsigned int borderpx       = 2;
static const unsigned int snap           = 2;
static const unsigned int systrayspacing = 6;
static const float mfact                 = 0.63;
static const char chatclient[]           = "jente_etnej - Skype™";
static const char clock_fmt[]            = "%a %d %b, %R";
static const int nmaster                 = 1;
static const Bool showbar                = True;
static const Bool topbar                 = True;
static const Bool resizehints            = False;

/* layouts */
static const Layout layouts[] = {
  /* symbol arrange */
	{ "T",  tile },
	{ "C",  chat },
	{ "B",  bstack },
	{ "M",  monocle },
	{ "F",  NULL },
};

/* tagging */
static const Tag tags[] = {
	/* name	     layout       mfact	nmaster */
	{ "1 web",   &layouts[0], -1,	-1 },
	{ "2 chill", &layouts[1], 0.80, -1 },
	{ "3 term",  &layouts[0], -1,	-1 },
	{ "4 misc",  &layouts[3], -1,	-1 },
};


static const Rule rules[] = {
    /*WM_CLASS              WM_CLASS    WM_NAME
      class                 instance    title               tags mask   isfloating  attachaside  monitor */
    { "Firefox",            NULL,       NULL,               1,          False,      False,       -1 },
    { "Skype",              NULL,       NULL,               1 << 1,     False,      False,       -1 },
    { "Skype",              NULL,       "jente_etnej - Skype™", 1 << 1, False,      True,        -1 },
    { "Skype",              NULL,       "Call with",        1 << 1,     True,       False,       -1 },
    { "Gnome-terminal",	    NULL,       NULL,               1 << 2,     False,      True,        -1 },
    { "Gedit",              NULL,       NULL,               1 << 2,     False,      False,       -1 },
    { "Audacious",          NULL,       NULL,               1 << 3,     False,      False,       -1 },
    { "MPlayer",            NULL,       NULL,               1 << 3,     True,       False,       -1 },
    { "Gimp",               NULL,       NULL,               1 << 3,     False,      False,       -1 },
    { "Cheese",             NULL,       NULL,               1 << 3,     False,      False,       -1 },
    { "Brasero",            NULL,       NULL,               1 << 3,     False,      False,       -1 },
    { "Transmission-gtk",   NULL,       NULL,               1 << 3,     False,      False,       -1 },
    { "VirtualBox",         NULL,       NULL,               1 << 3,     False,      False,       -1 },
    { "Evince",             NULL,       NULL,               1 << 3,     False,      False,       -1 },
    { "libreoffice-writer", NULL,       NULL,               1 << 3,     False,      False,       -1 },
    { "libreoffice-startcenter", NULL,  NULL,               1 << 3,     False,      False,       -1 },
};

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", font, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *dmenu[]   = { "dmenu_run", "-f", "-p", "Uitvoeren:", "-fn", font, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *find[]    = { "dmenu_finder", NULL };
static const char *dmfm[]    = { "dmenu_fm", NULL };
static const char *term[]    = { "gnome-terminal", NULL };
static const char *browser[] = { "firefox", NULL };
static const char *scrot[]   = { "gnome-screenshot", NULL };
static const char *xkill[]   = { "xkill", NULL };
static const char *lock[]    = { "slock", NULL };
static const char *soff[]    = { "xset", "dpms", "force", "off", NULL };
static const char *halt[]    = { "dmenu_shutdown", NULL };
static const char *volup[]   = { "amixer", "-q", "sset", "Master", "5%+", "unmute", NULL };
static const char *voldown[] = { "amixer", "-q", "sset", "Master", "5%-", "unmute", NULL };
static const char *volmute[] = { "amixer", "-q", "sset", "Master", "toggle", NULL };
static const char *play[]    = { "audtool", "playback-playpause", NULL };
static const char *next[]    = { "audtool", "playlist-advance", NULL };
static const char *prev[]    = { "audtool", "playlist-reverse", NULL };
static const char *stop[]    = { "audtool", "playback-stop", NULL };

/* key definitions */
static Key keys[] = {
	/* modifier                 key                         function        argument */
	{ MODKEY,                   XK_r,                       spawn,          {.v = dmenu } },
	{ MODKEY,                   XK_x,                       spawn,          {.v = xkill } },
	{ MODKEY,                   XK_l,                       spawn,          {.v = lock } },
	{ MODKEY,                   XK_Escape,                  spawn,          {.v = halt } },
	{ MODKEY,                   XK_s,                       spawn,          {.v = find } },
	{ MODKEY,                   XK_o,                       spawn,          {.v = dmfm } },
	{ MODKEY,                   XK_e,                       spawn,          {.v = term } },
	{ MODKEY,                   XK_w,                       spawn,          {.v = browser } },
	{ 0,                        XK_Print,                   spawn,          {.v = scrot } },
	{ 0,                        XF86XK_Display,             spawn,          {.v = soff } },
	{ 0,                        XF86XK_AudioRaiseVolume,    spawn,          {.v = volup } },
	{ 0,                        XF86XK_AudioLowerVolume,    spawn,          {.v = voldown } },
	{ 0,                        XF86XK_AudioMute,           spawn,          {.v = volmute } },
	{ 0,                        XF86XK_AudioPlay,           spawn,          {.v = play } },
	{ 0,                        XF86XK_AudioNext,           spawn,          {.v = next } },
	{ 0,                        XF86XK_AudioPrev,           spawn,          {.v = prev } },
	{ 0,                        XF86XK_AudioStop,           spawn,          {.v = stop } },
	{ MODKEY|ControlMask,       XK_b,                       togglebar,      {0} },
	{ MODKEY|ControlMask,       XK_q,                       quit,           {0} },
	{ MODKEY,                   XK_Tab,                     focusstack,     {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_Tab,                     focusstack,     {.i = -1 } },
	{ MODKEY,                   XK_Return,                  zoom,           {0} },
	{ MODKEY,                   XK_dead_grave,              view,           {0} },
	{ MODKEY,                   XK_q,                       killclient,     {0} },
	{ MODKEY,                   XK_bracketleft,             setmfact,       {.f = -0.05} },
	{ MODKEY,                   XK_bracketright,            setmfact,       {.f = +0.05} },
	/*{ MODKEY|ShiftMask,         XK_bracketleft,             setsmfact,      {.f = +0.05} },
	{ MODKEY|ShiftMask,         XK_bracketright,            setsmfact,      {.f = -0.05} },*/
	{ MODKEY,                   XK_equal,                   incnmaster,     {.i = +1 } },
	{ MODKEY,                   XK_minus,                   incnmaster,     {.i = -1 } },
	{ MODKEY,                   XK_space,                   setlayout,      {0} },
	{ MODKEY,                   XK_Up,                      pushup,         {0} },
	{ MODKEY,                   XK_Down,                    pushdown,       {0} },
	/*{ MODKEY,                   XK_Left,                    cycle,          {.i = -1 } },
	{ MODKEY,                   XK_Right,                   cycle,          {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_Left,                    tagcycle,       {.i = -1 } },
	{ MODKEY|ShiftMask,         XK_Right,                   tagcycle,       {.i = +1 } },*/
	{ MODKEY|ShiftMask,         XK_f,                       togglefloating, {0} },
	{ MODKEY,                   XK_t,                       setlayout,      {.v = &layouts[0] } },
	{ MODKEY,                   XK_c,                       setlayout,      {.v = &layouts[1] } },
	{ MODKEY,                   XK_b,                       setlayout,      {.v = &layouts[2] } },
	{ MODKEY,                   XK_m,                       setlayout,      {.v = &layouts[3] } },
	{ MODKEY,                   XK_f,                       setlayout,      {.v = &layouts[4] } },
	TAGKEYS(                    XK_1,                       0)
	TAGKEYS(                    XK_2,                       1)
	TAGKEYS(                    XK_3,                       2)
	TAGKEYS(                    XK_4,                       3)
	{ MODKEY,                   XK_a,                       view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,         XK_a,                       tag,            {.ui = ~1 } },
	{ MONKEY,                   XK_Left,                    focusmon,       {.i = -1 } },
	{ MONKEY,                   XK_Right,                   focusmon,       {.i = +1 } },
	{ MONKEY|ShiftMask,         XK_Left,                    tagmon,         {.i = -1 } },
	{ MONKEY|ShiftMask,         XK_Right,                   tagmon,         {.i = +1 } },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkClock, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkClock,             0,              Button2,        killclient,     {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = term } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

