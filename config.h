/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      toggletag,      {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      tag,            {.ui = 1 << TAG} },
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* appearance */
static const char font[]                 = "Sans 9";
static const char normbordercolor[]      = "#707070";
static const char normbgcolor[]          = "#FFFFFF";
static const char normfgcolor[]          = "#4D4D4C";
static const char selbordercolor[]       = "#DE935F";
static const char selbgcolor[]           = "#FFFFFF";
static const char selfgcolor[]           = "#4A90D9";
static const char urgbgcolor[]           = "#FFFFFF";
static const char urgfgcolor[]           = "#C82829";
static const unsigned int borderpx       = 1;
static const unsigned int snap           = 2;
static const float mfact                 = 0.63;
static const int nmaster                 = 1;
static const Bool showbar                = True;
static const Bool topbar                 = False;
static const Bool resizehints            = False;

static const char *tags[] = { "web", "skype", "term", "code", "media", "doc", "n/a" };

static const Layout layouts[] = {
	/* symbol   arrange */
	{ "T",      tile },
	{ "B",      bstack },
	{ "M",      monocle },
	{ "F",      NULL },
};

static const Rule rules[] = {
	/*WM_CLASS              WM_CLASS    WM_NAME
      class                 instance    title               tags mask   isfloating  attachaside  monitor */
	{ "Firefox",            NULL,       NULL,               1 << 0,     False,      False,       -1 },
	{ "Skype",              NULL,       NULL,               1 << 1,     False,      True,        -1 },
	{ "Termite",            NULL,       NULL,               1 << 2,     False,      True,        -1 },
	{ "Eclipse",            NULL,       NULL,               1 << 3,     False,      False,       -1 },
	{ "Java",               NULL,       NULL,               1 << 3,     True,       False,       -1 },
	{ "MPlayer",            NULL,       NULL,               1 << 4,     True,       False,       -1 },
	{ "Zathura",            NULL,       NULL,               1 << 5,     False,      False,       -1 },
	{ "libreoffice",        NULL,       NULL,               1 << 5,     False,      False,       -1 },
};

/* commands */
static const char *dmenu[]   = { "dmenu_run", "-f", "-p", "Uitvoeren:", NULL };
static const char *find[]    = { "dmenu_finder", NULL };
static const char *dmfm[]    = { "dmenu_fm", NULL };
static const char *dmctl[]   = { "dmenu_netctl", NULL };
static const char *term[]    = { "termite", NULL };
static const char *scrot[]   = { "scrot", "/home/jente/afbeeldingen/screenshots/%d-%m-%Y-%R.png", NULL };
static const char *xkill[]   = { "xkill", NULL };
static const char *lock[]    = { "slock", NULL };
static const char *soff[]    = { "xset", "dpms", "force", "off", NULL };
static const char *sup[]     = { "xbacklight", "-inc", "10", NULL };
static const char *sdown[]   = { "xbacklight", "-dec", "10", NULL };
static const char *halt[]    = { "dmenu_shutdown", NULL };
static const char *volup[]   = { "amixer", "-q", "sset", "Master", "5%+", "unmute", NULL };
static const char *voldown[] = { "amixer", "-q", "sset", "Master", "5%-", "unmute", NULL };
static const char *volmute[] = { "amixer", "-q", "sset", "Master", "toggle", NULL };
static const char *play[]    = { "mpc", "toggle", NULL };
static const char *next[]    = { "mpc", "next", NULL };
static const char *prev[]    = { "mpc", "prev", NULL };
static const char *stop[]    = { "mpc", "stop", NULL };

/* key definitions */
static Key keys[] = {
	/* modifier                 key                         function        argument */
	{ MODKEY,                   XK_q,                       killclient,     {0} },
	{ MODKEY|ControlMask,       XK_b,                       togglebar,      {0} },
	{ MODKEY|ControlMask,       XK_q,                       quit,           {0} },
	{ MODKEY,                   XK_bracketleft,             setmfact,       {.f = -0.05} },
	{ MODKEY,                   XK_bracketright,            setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,         XK_bracketleft,             setcfact,       {.f = -0.05} },
	{ MODKEY|ShiftMask,         XK_bracketright,            setcfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,         XK_0,                       setcfact,       {.f = 0.0} },
	{ MODKEY,                   XK_equal,                   incnmaster,     {.i = +1 } },
	{ MODKEY,                   XK_minus,                   incnmaster,     {.i = -1 } },
	{ MODKEY,                   XK_Return,                  zoom,           {0} },
	{ MODKEY,                   XK_Tab,                     view,           {0} },
	{ MODKEY|ShiftMask,         XK_space,                   togglefloating, {0} },
	{ MODKEY,                   XK_t,                       setlayout,      {.v = &layouts[0] } },
	{ MODKEY,                   XK_b,                       setlayout,      {.v = &layouts[1] } },
	{ MODKEY,                   XK_m,                       setlayout,      {.v = &layouts[2] } },
	{ MODKEY,                   XK_f,                       setlayout,      {.v = &layouts[3] } },
	{ MODKEY,                   XK_j,                       focusstack,     {.i = +1 } },
	{ MODKEY,                   XK_k,                       focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,         XK_j,                       pushup,         {0} },
	{ MODKEY|ShiftMask,         XK_k,                       pushdown,       {0} },
	{ MODKEY,                   XK_dead_grave,              view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,         XK_dead_grave,              tag,            {.ui = ~0 } },
	TAGKEYS(                    XK_1,                       0)
	TAGKEYS(                    XK_2,                       1)
	TAGKEYS(                    XK_3,                       2)
	TAGKEYS(                    XK_4,                       3)
	TAGKEYS(                    XK_5,                       4)
	TAGKEYS(                    XK_6,                       5)
	TAGKEYS(                    XK_7,                       6)
	{ MODKEY,                   XK_comma,                   focusmon,       {.i = -1 } },
	{ MODKEY,                   XK_period,                  focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,         XK_comma,                   tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,         XK_period,                  tagmon,         {.i = +1 } },
	{ MODKEY,                   XK_r,                       spawn,          {.v = dmenu } },
	{ MODKEY,                   XK_Escape,                  spawn,          {.v = halt } },
	{ MODKEY,                   XK_e,                       spawn,          {.v = term } },
	{ MODKEY|ShiftMask,         XK_x,                       spawn,          {.v = xkill } },
	{ MODKEY|ShiftMask,         XK_l,                       spawn,          {.v = lock } },
	{ MODKEY|ShiftMask,         XK_s,                       spawn,          {.v = find } },
	{ MODKEY|ShiftMask,         XK_o,                       spawn,          {.v = dmfm } },
	{ MODKEY|ShiftMask,         XK_n,                       spawn,          {.v = dmctl } },
	{ 0,                        XK_Print,                   spawn,          {.v = scrot } },
	{ 0,                        XF86XK_Display,             spawn,          {.v = soff } },
	{ 0,                        XF86XK_AudioRaiseVolume,    spawn,          {.v = volup } },
	{ 0,                        XF86XK_AudioLowerVolume,    spawn,          {.v = voldown } },
	{ 0,                        XF86XK_AudioMute,           spawn,          {.v = volmute } },
	{ 0,                        XF86XK_AudioPlay,           spawn,          {.v = play } },
	{ 0,                        XF86XK_AudioNext,           spawn,          {.v = next } },
	{ 0,                        XF86XK_AudioPrev,           spawn,          {.v = prev } },
	{ 0,                        XF86XK_AudioStop,           spawn,          {.v = stop } },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkWinTitle,          0,              Button2,        killclient,     {0} },
	{ ClkStatusText,        0,              Button3,        spawn,          {.v = volmute } },
	{ ClkStatusText,        0,              Button4,        spawn,          {.v = volup } },
	{ ClkStatusText,        0,              Button5,        spawn,          {.v = voldown } },
	{ ClkStatusText,        MODKEY,         Button4,        spawn,          {.v = sup } },
	{ ClkStatusText,        MODKEY,         Button5,        spawn,          {.v = sdown } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        toggleview,     {0} },
	{ ClkTagBar,            0,              Button3,        view,           {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
