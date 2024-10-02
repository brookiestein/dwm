/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int gappx     = 8;        /* gap pixel between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "JetBrainsMono Nerd Font Mono:size=13.5" };
static const char dmenufont[]       = "JetBrainsMono Nerd Font Mono:size=14";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
/* static const char col_cyan[]        = "#005577"; */
static const char col_cyan[]        = "#633ec2";
static const char *colors[][3]      = {
    /*               fg         bg         border   */
    [SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
    [SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

static const char *const autostart[] = {
    "picom", "--daemon", NULL,
    "xwallpaper", "--stretch", "/home/brayan/Wallpapers/360290.jpg", NULL,
    "setxkbmap", "-layout", "us", "-variant", "intl", NULL,
    "spm", "--daemon", "--file=/tmp/spm.log", "--monitor", "--verbose", NULL,
    "slstatus", NULL,
    "dunst", NULL,
    "clipmenud", NULL,
    "xset", "r", "rate", "300", "50", NULL, /* In order to move faster while writing. Useful, e.g., in vim. */
    "xset", "-dpms", NULL, /* Disable DPMS */
    "xset", "s", "off", NULL, /* Disable screensaver */
    "xset", "s", "noblank", NULL, /* Disable screen blank */
    "gentoo-pipewire-launcher", "restart", NULL,
    "blueman-applet", NULL,
    "nm-applet", NULL,
    NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                        instance            title       tags mask     iscentered   isfloating   monitor */
    { "Pavucontrol",                "pavucontrol",      NULL,       0,              1,          1,      -1 },
    { "mpv",                        "gl",               NULL,       0,              1,          1,      -1 },
    { "Sxiv",                       "sxiv",             NULL,       0,              1,          1,      -1 },
    { "Caja",                       "caja",             NULL,       0,              1,          1,      -1 },
    { "Engrampa",                   "engrampa",         NULL,       0,              1,          1,      -1 },
    { "Blueman-manager",            "blueman-manager",  NULL,       0,              1,          1,      -1 },
    { "System-config-printer.py",   "system-config-printer",        0,              1,          1,      -1 },
    /* Other programs without floating setting */
    { "Firefox",                    NULL,               NULL,       1,              0,          0,      -1 },
    { "Google-chrome",              "google-chrome",    NULL,       2,              0,          0,      -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[]=",      tile },    /* first entry is default */
    { "><>",      NULL },    /* no layout function means floating behavior */
    { "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
    { 0, MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
    { 0, MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
    { 0, MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
    { 0, MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define MULTIKEY_THRESHOLD_MS_PRESS 200
#define MULTIKEY_THRESHOLD_MS_HOLD 700

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *termcmd[] = { "alacritty", NULL };
static const char *dmenucmd[] = {
    "dmenu_run", "-m", dmenumon,
    "-fn", dmenufont, "-nb", col_gray1,
    "-nf", col_gray3, "-sb", col_cyan,
    "-sf", col_gray4, NULL
};

static const char *scrot[] = {
    "scrot", "BrookieShot_%F_%H-%M-%S.png",
    "-e", "sxiv ~/$f", NULL
};

static const char *dec_brightness[] = { "xbacklight", "-dec", "10", NULL };
static const char *inc_brightness[] = { "xbacklight", "-inc", "10", NULL };

/* Multimedia commands */
static const char *upvol5[] = {
    "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@",
    "0.05+", NULL
};

static const char *upvol10[] = {
    "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@",
    "0.10+", NULL
};

static const char *downvol5[] = {
    "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@",
    "0.05-", NULL
};

static const char *downvol10[] = {
    "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@",
    "0.10-", NULL
};

static const char *mute[] = {
    "wpctl", "set-mute", "@DEFAULT_AUDIO_SINK@",
    "toggle", NULL
};

static const char *mutemic[] = {
    "wpctl", "set-mute", "@DEFAULT_AUDIO_SOURCE@",
    "toggle", NULL
};

static Key keys[] = {
    /* npress, modifier                key        function        argument */
    { 1, MODKEY,                    XK_p,      spawn,          {.v = dmenucmd } },
    { 0, MODKEY|ShiftMask,          XK_Return, spawn,          {.v = termcmd } },
    { 0, MODKEY,                    XK_b,      togglebar,      {0} },
    { 0, MODKEY,                    XK_j,      focusstack,     {.i = +1 } },
    { 1, MODKEY,                    XK_k,      focusstack,     {.i = -1 } },
    { 0, MODKEY,                    XK_i,      incnmaster,     {.i = +1 } },
    { 0, MODKEY,                    XK_d,      incnmaster,     {.i = -1 } },
    { 0, MODKEY,                    XK_h,      setmfact,       {.f = -0.05} },
    { 1, MODKEY,                    XK_l,      setmfact,       {.f = +0.05} },
    { 0, MODKEY,                    XK_Return, zoom,           {0} },
    { 0, MODKEY,                    XK_Tab,    view,           {0} },
    { 0, MODKEY|ShiftMask,          XK_q,      killclient,     {0} },
    { 1, MODKEY,                    XK_t,      setlayout,      {.v = &layouts[0]} },
    { 0, MODKEY,                    XK_f,      setlayout,      {.v = &layouts[1]} },
    { 1, MODKEY,                    XK_m,      setlayout,      {.v = &layouts[2]} },
    { 0, MODKEY,                    XK_space,  setlayout,      {0} },
    { 0, MODKEY|ShiftMask,          XK_space,  togglefloating, {0} },
    { 0, MODKEY,                    XK_0,      view,           {.ui = ~0 } },
    { 0, MODKEY|ShiftMask,          XK_0,      tag,            {.ui = ~0 } },
    { 0, MODKEY,                    XK_comma,  focusmon,       {.i = -1 } },
    { 0, MODKEY,                    XK_period, focusmon,       {.i = +1 } },
    { 0, MODKEY|ShiftMask,          XK_comma,  tagmon,         {.i = -1 } },
    { 0, MODKEY|ShiftMask,          XK_period, tagmon,         {.i = +1 } },
    { 0, MODKEY|ShiftMask,          XK_e,      quit,           {0} },
    TAGKEYS(                        XK_1,                      0)
    TAGKEYS(                        XK_2,                      1)
    TAGKEYS(                        XK_3,                      2)
    TAGKEYS(                        XK_4,                      3)
    TAGKEYS(                        XK_5,                      4)
    TAGKEYS(                        XK_6,                      5)
    TAGKEYS(                        XK_7,                      6)
    TAGKEYS(                        XK_8,                      7)
    TAGKEYS(                        XK_9,                      8)
    /* Sound config */
    { 0, 0,                         XF86XK_AudioRaiseVolume,    spawn,  {.v = upvol5} },
    { 0, 0,                         XF86XK_AudioLowerVolume,    spawn,  {.v = downvol5} },
    { 0, 0,                         XF86XK_AudioMute,           spawn,  {.v = mute} },
    { 0, 0,                         XF86XK_AudioMicMute,        spawn,  {.v = mutemic } },
    /* Increase and decrease 10+ the volume */
    { 0, ShiftMask,                 XF86XK_AudioRaiseVolume,    spawn,  {.v = upvol10 } },
    { 0, ShiftMask,                 XF86XK_AudioLowerVolume,    spawn,  {.v = downvol10 } },
    { 0, 0,                         XF86XK_MonBrightnessDown,   spawn,  {.v = dec_brightness } },
    { 0, 0,                         XF86XK_MonBrightnessUp,     spawn,  {.v = inc_brightness } },
    /* Key bindings for launching programs. Alphabetically sorted. (Mostly) */
    { 1, MODKEY,                    XK_c,                       spawn,  SHCMD("clipmenu") },
    { 2, MODKEY,                    XK_c,                       spawn,  SHCMD("caja") },
    { 0, MODKEY|ShiftMask,          XK_c,                       spawn,  SHCMD("center_window") },
    { 2, MODKEY,                    XK_l,                       spawn,  SHCMD("slock") },
    { 2, MODKEY,                    XK_p,                       spawn,  SHCMD("pavucontrol") },
    { 1, 0,                         XK_Print,                   spawn,  {.v = scrot } }, /* Take fast screenshots. */
    { 2, 0,                         XK_Print,                   spawn,  SHCMD("flameshot") }, /* "Professional screenshoter." */
    /* Lock the screen because of double suspension in the old way. Look at: 55a8e9f */
    { 0, 0,                         XF86XK_Sleep,               spawn,  SHCMD("slock") },
    { 1, MODKEY,                    XK_s,                       spawn,  SHCMD("spm") },
    { 2, MODKEY,                    XK_s,                       spawn,  SHCMD("toggle-keyboard") }, /* See my dotfiles scripts directory. */
    { 2, MODKEY,                    XK_t,                       spawn,  SHCMD("telegram-desktop") }, /* Telegram messenger. */
    { 0, MODKEY,                    XK_q,                       spawn,  SHCMD("qbittorrent") },
    { 1, MODKEY,                    XK_w,                       spawn,  SHCMD("firefox") }, /* Web browser. */
    { 2, MODKEY,                    XK_w,                       spawn,  SHCMD("firefox --private-window") }, /* Web browser in private mode. */
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
