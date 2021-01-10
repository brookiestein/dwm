/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include <X11/keysymdef.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "mononoki Nerd Font Mono:size=10" };
static const char dmenufont[]       = "mononoki Nerd Font Mono:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

static const char *const autostart[] = {
        /* "xwallpaper", "--stretch", "the/path", NULL, */
        "setxkbmap", "-layout", "us", NULL,
        "compton", "--backend", "glx", "--paint-on-overlay", "--vsync", "opengl-swc", NULL,
        "spm", "--daemon", "--file=/tmp/spm.log", "--monitor", "--verbose", NULL,
        "pulseaudio", "--start", NULL,
        "slstatus", NULL,
        "dunst", NULL,
        "mega-cmd-server", NULL,
        "/home/brayan/.config/scripts/resolution", "on", NULL,
        /* Disabling dpms */
        "xset", "-dpms", NULL,
        "xset", "s", "off", NULL,
        "xset", "s", "noblank", NULL,
        NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

/* grid of tags */
#define DRAWCLASSICTAGS             1 << 0
#define DRAWTAGGRID                 1 << 1

#define SWITCHTAG_UP                1 << 0
#define SWITCHTAG_DOWN              1 << 1
#define SWITCHTAG_LEFT              1 << 2
#define SWITCHTAG_RIGHT             1 << 3
#define SWITCHTAG_TOGGLETAG         1 << 4
#define SWITCHTAG_TAG               1 << 5
#define SWITCHTAG_VIEW              1 << 6
#define SWITCHTAG_TOGGLEVIEW        1 << 7

static const unsigned int drawtagmask = DRAWTAGGRID; /* | DRAWCLASSICTAGS to show classic row of tags */
static const int tagrows = 2;

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     iscentered   isfloating   monitor */
        { "Pavucontrol","pavucontrol",  NULL,   0,      1,      1,      -1 },
        { "mpv",        "gl",           NULL,   0,      1,      1,      -1 },
        { "wpa_gui",    "wpa_gui",      NULL,   0,      1,      1,      -1 },
        { "Thunar",     "thunar",       NULL,   0,      1,      1,      -1 },
        /* Other programs without floating setting */
        { "Firefox",    NULL,           NULL,   1,      0,      0,      -1 },
        { "KeePassXC",  "keepassxc",    NULL,   1 << 2, 1,      0,      -1 },
        { "qBittorrent","qbittorrent",  NULL,   1 << 3, 1,      0,      -1 },
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
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *termcmd[]  = { "st", "-e", "tmux", NULL };
static const char *dmenucmd[]                   = {
        "dmenu_run", "-m", dmenumon,
        "-fn", dmenufont, "-nb", col_gray1,
        "-nf", col_gray3, "-sb", col_cyan,
        "-sf", col_gray4, NULL
};
static const char *scrot[]                      = {
        "scrot", "BrookieShot_%F_%H-%M-%S.png",
        "-e", "sxiv ~/$f",
        NULL
};
/* Touchpad settings: Use xinput to know your touchpad's id. */
static const char *turn_off_tpad[]              = {
        "xinput", "disable",
        "SynPS/2 Synaptics TouchPad", NULL
};
static const char *turn_on_tpad[]               = {
        "xinput", "enable",
        "SynPS/2 Synaptics TouchPad", NULL
};
static const char *dec_brightness[]             = { "xbacklight", "-dec", "10", NULL };
static const char *inc_brightness[]             = { "xbacklight", "-inc", "10", NULL };
/* You must have installed gtk3-nocsd! */
static const char *evince[]                     = { "gtk3-nocsd", "evince", NULL };
/* This is a own script. If you want to watch it, see my dotfiles script repository. */
static const char *toggle_keyboard[]            = { "toggle-keyboard", NULL };

/* Multimedia commands */
/* Notice that this command will use PulseAudio! */
static const char *upvol[]      = {
        "amixer", "-q", "sset",
        "Master", "5%+", NULL
};

static const char *downvol[]    = {
        "amixer", "-q", "sset",
        "Master", "5%-", NULL
};

static const char *upvol2[]     = {
        "amixer", "-q", "sset",
        "Master", "10%+", NULL
};

static const char *downvol2[]   = {
        "amixer", "-q", "sset",
        "Master", "10%-", NULL
};

static const char *mute[]       = {
        "amixer", "-q", "sset",
        "Master", "toggle", NULL
}; /* for muting/unmuting */

static const char *mutemic[]    = {
        "amixer", "-q", "sset",
        "Capture", "toggle", NULL
}; /* The same, but for microphone */

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
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
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
	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },

    { MODKEY|ControlMask,           XK_Up,     switchtag,      { .ui = SWITCHTAG_UP     | SWITCHTAG_VIEW } },
    { MODKEY|ControlMask,           XK_Down,   switchtag,      { .ui = SWITCHTAG_DOWN   | SWITCHTAG_VIEW } },
    { MODKEY|ControlMask,           XK_Right,  switchtag,      { .ui = SWITCHTAG_RIGHT  | SWITCHTAG_VIEW } },
    { MODKEY|ControlMask,           XK_Left,   switchtag,      { .ui = SWITCHTAG_LEFT   | SWITCHTAG_VIEW } },

    { MODKEY|Mod4Mask,              XK_Up,     switchtag,      { .ui = SWITCHTAG_UP     | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
    { MODKEY|Mod4Mask,              XK_Down,   switchtag,      { .ui = SWITCHTAG_DOWN   | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
    { MODKEY|Mod4Mask,              XK_Right,  switchtag,      { .ui = SWITCHTAG_RIGHT  | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
    { MODKEY|Mod4Mask,              XK_Left,   switchtag,      { .ui = SWITCHTAG_LEFT   | SWITCHTAG_TAG | SWITCHTAG_VIEW } },

        /* Sound config */
        { 0,              XF86XK_AudioRaiseVolume,      spawn,  {.v = upvol } },
        { 0,              XF86XK_AudioLowerVolume,      spawn,  {.v = downvol } },
        { 0,              XF86XK_AudioMute,             spawn,  {.v = mute} },
        { 0,              XF86XK_AudioMicMute,          spawn,  {.v = mutemic } },
        /* Increase and decrease 10+ the volume */
        { ShiftMask,      XF86XK_AudioRaiseVolume,      spawn,  {.v = upvol2 } },
        { ShiftMask,      XF86XK_AudioLowerVolume,      spawn,  {.v = downvol2 } },
        { 0,            XF86XK_MonBrightnessDown,       spawn,  {.v = dec_brightness } },
        { 0,            XF86XK_MonBrightnessUp,         spawn,  {.v = inc_brightness } },
        /* Enable and disable touchpad. See xinput's output if you want know your touchpad's name. */
        { MODKEY|ShiftMask,             XK_l,           spawn,  SHCMD("slock") }, /* Lock the screen */
        { MODKEY|ControlMask,           XK_comma,       spawn,  {.v = turn_off_tpad } },
        { MODKEY|ControlMask,           XK_period,      spawn,  {.v = turn_on_tpad } },
        /* Key bindings for change the keyboard layout */
        /* Key bindings for launch programs. Ordered alphabetically. (Mainly) */
        { MODKEY,                       XK_c,           spawn,  SHCMD("clipmenu") },
        { MODKEY,                       XK_e,           spawn,  {.v = evince } }, /* PDF Viewer */
        { MODKEY|ShiftMask,             XK_f,           spawn,  SHCMD("thunar") }, /* File explorer */
        { MODKEY,                       XK_s,           spawn,  {.v = toggle_keyboard } }, /* See my dotfiles script directory. */
        { MODKEY,                       XK_Print,       spawn,  SHCMD("flameshot") }, /* "Professional screenshoter." */
        { MODKEY|ShiftMask,             XK_k,           spawn,  SHCMD("keepassxc") }, /* Password manager. */
        { 0,                            XK_Print,       spawn,  {.v = scrot } }, /* Take fast screenshots. */
        { MODKEY|ShiftMask,             XK_p,           spawn,  SHCMD("pavucontrol") }, /* Sound devices manager. */
        { MODKEY,                       XK_q,           spawn,  SHCMD("qbittorrent") }, /* Bittorrent admin. */
        { MODKEY|ShiftMask,             XK_Delete,      spawn,  SHCMD("spm") }, /* GUI to shutdown, reboot, etc. */
        /* Lock the screen because of double suspension in the old way. Look at: 55a8e9f */
        { 0,                            XF86XK_Sleep,   spawn,  SHCMD("slock") },
        { MODKEY|ShiftMask,             XK_t,           spawn,  SHCMD("telegram") }, /* Telegram messenger. */
        { MODKEY|ControlMask,           XK_t,           spawn,  SHCMD("torbrowser") }, /* Tor Browser. */
        { MODKEY,                       XK_w,           spawn,  SHCMD("firefox") }, /* Web browser. */
        { MODKEY|ShiftMask,             XK_w,           spawn,  SHCMD("firefox --private-window") } /* Web browser in private mode. */
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

