/* See LICENSE file for copyright and license details. */

/* Libraries utilised */
/* For some keys */
#include <X11/XF86keysym.h>
#include <X11/keysymdef.h>

/* appearance */
/* Systray settings */
static const unsigned int systraypinning = 0;  /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;  /* systray spacing */
static const int systraypinningfailfirst = 1;  /* 1: if pinning fails, display systray on the first monitor, 0: display systray on the last monitor*/
static const int showsystray             = 1;  /* 0 means no systray */

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
	/* class                        instance                        title       tags mask       iscentered       isfloating   monitor */
        /* Configuring floating windows */
        { "Caja",                       "caja",                         NULL,           0,            1,                 1,           -1 },
        { "Leafpad",                    "leafpad",                      NULL,           0,            1,                 1,           -1 },
        { "Pavucontrol",                "pavucontrol",                  NULL,           0,            1,                 1,           -1 },
        { "Lxappearance",               "lxappearance",                 NULL,           0,            1,                 1,           -1 },
        { "Xfce4-power-manager-settings","xfce4-power-manager-settings",NULL,           0,            1,                 1,           -1 },
        { "Viewnior",                   "viewnior",                     NULL,           0,            1,                 1,           -1 },
        { "Engrampa",                   "engrampa",                     NULL,           0,            1,                 1,           -1 },
        /* Other programs without floating setting */
        { "Firefox",                    NULL,                           NULL,           1,            0,                 0,           -1 },
        { "vlc",                        "vlc",                          NULL,           1 << 1,       0,                 0,           -1 },
        { "KeePassXC",                  "keepassxc",                    NULL,           1 << 2,       0,                 0,           -1 },
        { "qBittorrent",                "qbittorrent",                  NULL,           1 << 3,       0,                 0,           -1 },
        { "libreoffice-startcenter",    "libreoffice",                  NULL,           1 << 4,       0,                 0,           -1 },
        { "TelegramDesktop",            "Telegram",                     NULL,           1 << 7,       0,                 0,           -1 },
        { "SimpleScreenRecorder",       "simplescreenrecorder",         NULL,           1 << 8,       0,                 0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile }, /* First entry is default. */
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
static const char *dmenucmd[]                   = { "dmenu_run", "-l", "20", "-c", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1,
                                                "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
/* Keyboard layout key bindings */
static const char *k_latam[]                    = { "setxkbmap", "-layout", "latam", NULL };
static const char *k_us_intl[]                  = { "setxkbmap", "-layout", "us", "-variant", "intl", NULL };
/* Most used programs */
static const char *caja[]                       = { "caja", NULL };
static const char *cheese[]			= { "cheese", NULL };
static const char *cmus[]                       = { "st", "-e", "cmus", NULL };
static const char *dec_brightness[]             = { "xbacklight", "-dec", "10", NULL };
static const char *inc_brightness[]             = { "xbacklight", "-inc", "10", NULL };
static const char *disable_tpad[]               = { "xinput", "disable", "SynPS/2 Synaptics TouchPad", NULL }; /* Use xinput for know your touchpad's id */
static const char *enable_tpad[]                = { "xinput", "enable", "SynPS/2 Synaptics TouchPad", NULL }; /* The same */
static const char *eclipse[]                    = { "/home/brookie/.eclipse/java-2020-03/eclipse/eclipse", NULL };
static const char *firefox[]                    = { "firefox", NULL };
static const char *flameshot[]                  = { "flameshot", NULL };
static const char *libreoffice[]                = { "libreoffice", NULL };
static const char *keepassxc[]                  = { "keepassxc", NULL };
static const char *pavucontrol[]                = { "pavucontrol", NULL };
static const char *qbittorrent[]                = { "qbittorrent", NULL };
static const char *scrot[]                      = { "scrot", "BrookieShot_\\%a-\\%d\\%b%y_%H.%M.\\%S.png", "-e", "viewnior ~/$f", NULL };
static const char *syspowermanager[]            = { "syspowermanager", NULL };
static const char *suspend[]                    = { "syspowermanager", "--suspend", NULL };
static const char *slock[]                      = { "slock", NULL }; /* Lock the screen */
static const char *telegram[]                   = { "/opt/Telegram/Telegram", NULL };
static const char *termcmd[]                    = { "st", NULL };
static const char *vlc[]                        = { "vlc", NULL };
static const char *xfce4_power_manager[]        = { "xfce4-power-manager-settings", NULL };
static const char *torbrowser[]                 = { "/opt/tor-browser_en-US/Browser/start-tor-browser",
                                                "--detach || ([ ! -x /opt/tor-browser_en-US/Browser/start-tor-browser ]",
                                                "&& /opt/tor-browser_en-US/start-tor-browser", "--detach)'", "dummy %k", NULL };

/* Commands */
/* static const char *upvol[] = { "amixer", "set", "Master", "2+", NULL }; */
/* static const char *downvol[] = { "amixer", "set", "Master", "2-", NULL }; */
/* static const char *mute[] = { "amixer", "-q", "set", "Master", "toggle", NULL }; /1* for muting/unmuting *1/ */

/* For pulse compatible */
static const char *upvol[] = { "amixer", "-q", "sset", "Master", "5%+", NULL };
static const char *downvol[] = { "amixer", "-q", "sset", "Master", "5%-", NULL };
static const char *mute[] = { "amixer", "-q", "-D", "pulse", "sset", "Master", "toggle", NULL }; /* for muting/unmuting */

static const char *upvol2[] = { "amixer", "-q", "sset", "Master", "10%+", NULL };
static const char *downvol2[] = { "amixer", "-q", "sset", "Master", "10%-", NULL };

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
        { MODKEY|ShiftMask,             XK_e,      quit,           {0} },
        { MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
        /* Sound config */
        { 0,              XF86XK_AudioRaiseVolume, spawn,          {.v = upvol } },
        { 0,              XF86XK_AudioLowerVolume, spawn,          {.v = downvol } },
        { 0,              XF86XK_AudioMute,        spawn,          {.v = mute } },
        /* Increase and decrease 10+ the volume */
        { ShiftMask,      XF86XK_AudioRaiseVolume, spawn,          {.v = upvol2 } },
        { ShiftMask,      XF86XK_AudioLowerVolume, spawn,          {.v = downvol2 } },
        /* Increase and decrease the screen brightness. */
        /* It works only if your user belongs to video's group and is allowed by udev. */
        /* If that isn't so, please see the next repository for make a rule that allow it. */
        /* https://github.com/brookiestein/dotfiles/tree/master/etc/udev/rules.d/ */
        /* Only copy that file to the same directory (as root obviously). */
        /* If you wish know if your user belongs to video's group, execute the next command in a terminal: */
        /* $ groups $USER | grep video */
        /* If command's output is empty, log in as root and add your user to that group: */
        /* # gpasswd -a $USER video */
        /* Note: Here is important that you change $USER to your user's name, since so like is, it will add */
        /* to root user to that group. */
        { 0,            XF86XK_MonBrightnessDown,   spawn,         {.v = dec_brightness } },
        { 0,            XF86XK_MonBrightnessUp,     spawn,         {.v = inc_brightness } },
        /* Enable and disable touchpad. See xinput's output if you want know your touchpad's name. */
        { MODKEY,                       XK_F1,      spawn,         {.v = k_latam } }, /* Put the keyboard layout to LATAM. */
        { MODKEY,                       XK_F2,      spawn,         {.v = k_us_intl } }, /* Put the keyboard layout to US-Intl. */
        { MODKEY,                       XK_F3,      spawn,         {.v = slock } }, /* Lock the screen */
	{ MODKEY,                       XK_F6,      spawn,         {.v = cheese } }, /* Launch camera's app. */
        { MODKEY,                       XK_Escape,  spawn,         {.v = disable_tpad } },
        { MODKEY,                       XK_Delete,  spawn,         {.v = enable_tpad } },
        /* Key bindings for change the keyboard layout */
        /* Key bindings for launch programs. Ordered alphabetically. (Mainly) */
        { MODKEY|ShiftMask,             XK_c,       spawn,         {.v = caja } }, /* File manager */
        { MODKEY|ControlMask|ShiftMask, XK_e,       spawn,         {.v = eclipse } }, /* IDE for Java Developers Edition. */
        { MODKEY|ShiftMask,             XK_f,       spawn,         {.v = firefox } }, /* Web explorer */
        { MODKEY|ControlMask,           XK_f,       spawn,         {.v = flameshot } }, /* Screenshotter */
        { MODKEY|ShiftMask,             XK_k,       spawn,         {.v = keepassxc } }, /* Password manager */
        { MODKEY|ShiftMask,             XK_l,       spawn,         {.v = libreoffice } }, /* Office suite */
        { MODKEY|ShiftMask,             XK_p,       spawn,         {.v = pavucontrol } }, /* Sound devices manager */
        { MODKEY|ControlMask,           XK_q,       spawn,         {.v = qbittorrent } }, /* Bittorrent admin */
        { 0,                            XK_Print,   spawn,         {.v = scrot } }, /* Take screenshots */
        { MODKEY|ShiftMask,             XK_Delete,  spawn,         {.v = syspowermanager } }, /* GUI with options for shutdown, reboot, etc. */
        { 0,                      XF86XK_Sleep,     spawn,         {.v = suspend } }, /* Put the system to sleep */
        { 0,                      XF86XK_AudioPlay, spawn,         {.v = cmus } }, /* Launch music's reproductor. */
        { MODKEY|ShiftMask,             XK_t,       spawn,         {.v = telegram } }, /* Telegram */
        { MODKEY|ControlMask,           XK_t,       spawn,         {.v = torbrowser } }, /* Tor Browser */
        { MODKEY|ShiftMask,             XK_v,       spawn,         {.v = vlc } }, /* Media reproductor */
        { MODKEY,                       XK_e,       spawn,         {.v = xfce4_power_manager } }, /* XFCE 4's power manager */
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

