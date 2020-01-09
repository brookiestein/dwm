/* See LICENSE file for copyright and license details. */

/************************************************************************************************************/
/* For sound */
#include <X11/XF86keysym.h>

/* commands */
static const char *upvol[] = { "amixer", "set", "Master", "5+", NULL };
static const char *downvol[] = { "amixer", "set", "Master", "5-", NULL };

/* for muting/unmuting */
static const char *mute[] = { "amixer", "-q", "set", "Master", "toggle", NULL };

/* for pulse compatible */
/* static const char *upvol[] = { "amixer", "-q", "sset", "Master", "1%+", NULL }; */
/* static const char *downvol[] = { "amixer", "-q", "sset", "Master", "1%-", NULL }; */
/* static const char *mute[] = { "amixer", "-q", "-D", "pulse", "sset", "Master", "toggle", NULL }; */
/*************************************************************************************************************/

/* appearance */
static const unsigned int borderpx           = 1;        /* border pixel of windows */
static const unsigned int snap               = 32;       /* snap pixel */
static const unsigned int showbar            = 1;        /* 0 means no bar */
static const unsigned int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]                   = { "monospace:size=10" };
static const char dmenufont[]                = "monospace:size=10";
static const char col_gray1[]                = "#222222";
static const char col_gray2[]                = "#444444";
static const char col_gray3[]                = "#bbbbbb";
static const char col_gray4[]                = "#eeeeee";
static const char col_cyan[]                 = "#005577";
static const char *colors[][3]               = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	/*{ "Gimp",     NULL,       NULL,       0,            1,           -1 },*/
	{ "Firefox",  NULL,       NULL,       0 << 1,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask /* Change from Mod1Mask (ALT key) to Mod4Mask ("Windows key") */
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
/* static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL }; */
static const char *dmenucmd[]                   = { "rofi", "-lines", "12", "-padding", "18", "-width", "60",
                                                "-location", "0", "-show", "drun", "-sidebar-mode",
                                                "-columns", "3", "-font", "monospace 8" };
/* Keyboard layout key bindings */
static const char *k_latam[]                    = { "setxkbmap", "-layout", "latam", NULL };
static const char *k_us_intl[]                  = { "setxkbmap", "-layout", "us", "-variant", "intl", NULL };
/* Most used programs */
static const char *termcmd[]                    = { "alacritty", NULL };
static const char *firefox[]                    = { "firefox", NULL };
static const char *vlc[]                        = { "vlc", NULL };
static const char *keepassxc[]                  = { "keepassxc", NULL };
static const char *qbittorrent[]                = { "qbittorrent", NULL };
static const char *spotify[]                    = { "spotify", NULL };
static const char *libreoffice[]                = { "libreoffice", NULL };
static const char *sqlitebrowser[]              = { "sqlitebrowser", NULL };
static const char *telegram[]                   = { "Telegram", NULL };
/* static const char *engrampa[]                   = { "engrampa", NULL }; */
static const char *caja[]                       = { "caja", NULL };
static const char *wireshark[]                  = { "wireshark", NULL };
static const char *simplescreenrecorder[]       = { "simplescreenrecorder", NULL };
static const char *atril[]                      = { "atril", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	/* { MODKEY|ShiftMask,             XK_c,      killclient,     {0} }, */
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
        /* Custom binding keys */
	/* { MODKEY|ShiftMask,             XK_q,      quit,           {0} }, */
        /* Reconfiguring key bindings for kill a window and exit of dwm */
        { MODKEY|ShiftMask,             XK_e,      quit,           {0} },
        { MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
        /* Sound config */
        { 0,              XF86XK_AudioRaiseVolume, spawn,          {.v = upvol } },
        { 0,              XF86XK_AudioLowerVolume, spawn,          {.v = downvol } },
        { 0,              XF86XK_AudioMute,        spawn,          {.v = mute } },
        /* Key bindings for change the keyboard layout */
        { MODKEY|ShiftMask,             XK_x,       spawn,         {.v = k_latam } },
        { MODKEY|ShiftMask,             XK_z,       spawn,         {.v = k_us_intl } },
        /* Key bindings for launch programs */
        { MODKEY|ShiftMask,             XK_f,       spawn,         {.v = firefox } }, /* Firefox */
        { MODKEY|ShiftMask,             XK_v,       spawn,         {.v = vlc } }, /* VLC */
        { MODKEY|ShiftMask,             XK_k,       spawn,         {.v = keepassxc } }, /* KeePassXC */
        { MODKEY|ControlMask,           XK_q,       spawn,         {.v = qbittorrent } }, /* qBittorrent - Meta + Control + Q
                                                                                             by collision with exit command. */

        { MODKEY|ShiftMask,             XK_s,       spawn,         {.v = spotify } }, /* Spotify */
        { MODKEY|ControlMask|ShiftMask, XK_l,       spawn,         {.v = libreoffice } }, /* LibreOffice */
        { MODKEY|ControlMask|ShiftMask, XK_q,       spawn,         {.v = sqlitebrowser } }, /* SQLiteBrowser - Meta + Control + Shift + Q
                                                                                             by collision with exit command and qbittorrent */

        { MODKEY|ShiftMask,             XK_t,       spawn,         {.v = telegram } }, /* Telegram */
        { MODKEY|ShiftMask,             XK_c,       spawn,         {.v = caja } }, /* Caja */
        { MODKEY|ShiftMask,             XK_w,       spawn,         {.v = wireshark } }, /* Wireshark */
        { MODKEY|ControlMask|ShiftMask, XK_s,       spawn,         {.v = simplescreenrecorder } }, /* SimpleScreenRecorder */
        { MODKEY|ShiftMask,             XK_a,       spawn,         {.v = atril } }, /* Atril */

};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

