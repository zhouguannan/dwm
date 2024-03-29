#include <X11/XF86keysym.h>
#include "layouts.c"
#include "tcl.c"
/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 3;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const unsigned int gappih    = 21;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 21;       /* vert inner gap between windows */
static const unsigned int gappoh    = 21;       /* horiz outer gap between windows and screen edge */

static const int vertpad            = 10;       /* vertical padding of bar */   
static const int sidepad            = 10;       /* horizontal padding of bar */ 
static const unsigned int gappov    = 21;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const Bool viewontag         = True;     /* Switch view on tag switch */
static const char *fonts[]          = { "SauceCodePro Nerd Font Mono:size=16" };
static const char dmenufont[]       = "SauceCodePro Nerd Font Mono:size=16:";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#ffffff";
static const char col_cyan[]        = "#37474F";
static const char col_border[]      = "#FF75BC";
static const unsigned int baralpha = 0x67;
static const unsigned int borderalpha = OPAQUE;
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */

  /* 1 means respect size hints in tiled resizals */      
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_border  },
	[SchemeHid]  = { col_cyan,  col_gray1, col_border  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
//static const char *tags[] = { "一", "二", "三", "四", "五", "六", "七", "八", "九" };
static const char *tags[] = { "\uf120", "\uf7ae", "\uf121", "\uf04b", "\ue62e", "\uf251", "\ue727", "\uf537", "\uf684" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class                 instance    title       tags mask     isfloating isterminal noswallow  monitor */
	{ "Android Emulator",   NULL,       NULL,       0,            1,           0 ,        0,     -1 },
	{ "vlc",								NULL,       NULL,       0,            1,           0 ,        0,     -1 },
	{ "mpv",								NULL,       NULL,       0,            1,            0,        0,     -1 },
	{ "Thunar",             NULL,       NULL,       0,            1,            0,        0,     -1 },
	{ "Mousepad",						NULL,       NULL,       0,            1,            0,        0,     -1 },
  { "Catfish",            NULL,       NULL,       0,            1,            0,        0,     -1 },
	{ "Emulator",           NULL,       NULL,       0,            1,            0,        0,     -1 },
	{ "quemu-system-i386",  NULL,       NULL,       0,            1,            0,        0,     -1 },
	{ "Firefox",            NULL,       NULL,       1 << 8,       0,            0,        0,     -1 },
	{ "St",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "Tile",     tile },    /* first entry is default */
	{ "Null",     NULL },    /* no layout function means floating behavior */
	{ "Mono",     monocle },
  { "Grid",     grid },
  { "|||",      tcl },
  { "TTT",      bstack },      
  { "===",      bstackhoriz },
  { "|M|",      centeredmaster },         
  { ">M>",      centeredfloatingmaster }, 
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
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *bluetcmd[] = {"blueman-applet" , NULL};
static const char *mailcmd[] = {"mailspring" , NULL};
static const char *nemocmd[] = { "thunar" , NULL};
static const char *browsercmd[]  = { "google-chrome-stable", NULL };
static const char *musiccmd[] = {"netease-cloud-music" , NULL};
static const char *fctixcmd[] = {"fcitx5" , NULL};
static const char *upvol[]   = { "/home/kevin/scripts/vol-up.sh",  NULL };
static const char *downvol[] = { "/home/kevin/scripts/vol-down.sh",  NULL };
static const char *mutevol[] = { "/home/kevin/scripts/vol-toggle.sh",  NULL };
static const char *wpcmd[]  = { "/home/kevin/scripts/wp-change.sh", NULL };
static const char *sktogglecmd[]  = { "/home/kevin/scripts/sck-tog.sh", NULL };
static const char *searchcmd[] = { "catfish" , NULL };
static const char *nautilustogcmd[]  = { "/home/kevin/scripts/nautilus-tog.sh", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "80x24", NULL };

static const char *setqwertycmd[]  = { "/home/kevin/scripts/setxmodmap-qwerty.sh", NULL };
static const char *setcolemakcmd[]  = { "/home/kevin/scripts/setxmodmap-colemak.sh", NULL };

static const char *suspendcmd[]  = { "/home/kevin/scripts/suspend.sh", NULL };

static const char *screenlockcmd[]  = { "/home/kevin/scripts/screenlock.sh", NULL};

static const char *soundcmd[]  = { "pavucontrol", NULL};
static const char *incbacklightcmd[]  = { "/home/kevin/scripts/inc-backlight.sh", NULL};
static const char *decbacklightcmd[]  = { "/home/kevin/scripts/dec-backlight.sh", NULL};

static const char *screenshotcmd[] = { "flameshot", "gui", NULL };

static Key keys[] = {
	/* modifier            key                      function        argument */
	{ MODKEY,              XK_d,                    spawn,          {.v = dmenucmd } },
	{ MODKEY,              XK_Return,               spawn,          {.v = termcmd } },
    { MODKEY,              XK_Return,               spawn,          {.v = fctixcmd } },
    { MODKEY,              XK_Return,               spawn,          {.v = bluetcmd} },
    { MODKEY,              XK_i,      				incnmaster,     {.i = +1 } },
	{ MODKEY,              XK_u,      				incnmaster,     {.i = -1 } },
  { MODKEY|ControlMask,                       XK_r,      setlayout,      {.v = &layouts[7]} },
	{ MODKEY|ControlMask,                       XK_t,      setlayout,      {.v = &layouts[8]} },
	{ MODKEY|ControlMask,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ControlMask,                       XK_u,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY|ControlMask,                       XK_o,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY|ControlMask,           XK_u,      setlayout,      {.v = &layouts[5]} },
	{ MODKEY|ControlMask,           XK_o,      setlayout,      {.v = &layouts[6]} },
	{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[3]} },
//  { MODKEY,              XK_Return,               spawn,          {.v = mailcmd} },
	{ MODKEY,              XK_s,                    spawn,          {.v = searchcmd } },
	
{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[5]} },   
{ MODKEY,                       XK_p,      setlayout,      {.v = &layouts[4]} },   
	{ MODKEY,              XK_c,                    spawn,          {.v = browsercmd } },
	{ MODKEY,              XK_c,                    spawn,          {.v = fctixcmd } },
    { MODKEY,              XK_c,                    spawn,          {.v = bluetcmd} },
    { MODKEY,              XK_x,                    spawn,          {.v = mailcmd} },
    { MODKEY,	           XK_v,			        spawn,		      {.v = musiccmd } },
	{ MODKEY,              XK_h,                    setmfact,       {.f = -0.02} },
	{ MODKEY,              XK_l,                    setmfact,       {.f = +0.02} },
	{ MODKEY, 	           XK_z,			        spawn,		      {.v = nemocmd } },
	{ MODKEY|ShiftMask,    XK_m,                    spawn,          {.v = setcolemakcmd } },
	{ MODKEY|ShiftMask,    XK_p,                    spawn,          {.v = soundcmd } },
	{ MODKEY|ControlMask,  XK_s,                    spawn,          {.v = sktogglecmd } },
	{ MODKEY,              XK_f,                    spawn,          {.v = nautilustogcmd } },
	{ MODKEY|ControlMask,  XK_p,                    spawn,          {.v = screenshotcmd } },
	{ MODKEY|ControlMask,    XK_8,                    spawn,          {.v = downvol } },
	{ MODKEY|ControlMask,   XK_9,                    spawn,          {.v = mutevol } },
	{ MODKEY|ControlMask,    XK_0,                    spawn,          {.v = upvol   } },
	{ MODKEY,              XK_bracketleft,          spawn,          {.v = downvol } },
	{ MODKEY,              XK_backslash,            spawn,          {.v = mutevol } },
	{ MODKEY,              XK_bracketright,         spawn,          {.v = upvol   } },
	{ MODKEY,              XK_b,                    spawn,          {.v = wpcmd } },
//	{ MODKEY,              XK_comma,                spawn,          {.v = decbacklightcmd } },
//	{ MODKEY,              XK_period,               spawn,          {.v = incbacklightcmd } },
	{ MODKEY|ShiftMask,    XK_e,                    rotatestack,    {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_u,                    rotatestack,    {.i = -1 } },
	{ MODKEY,              XK_j,                    focusstack,     {.i = +1 } },
	{ MODKEY,              XK_k,                    focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,    XK_n,                    tagtoleft,      {0} },
	{ MODKEY|ShiftMask,    XK_i,                    tagtoright,     {0} },
	{ MODKEY|ShiftMask,    XK_h,                    incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_l,                    incnmaster,     {.i = -1 } },
	{ MODKEY,              XK_h,                    setmfact,       {.f = -0.02} },
	{ MODKEY,              XK_l,                    setmfact,       {.f = +0.02} },
	{ MODKEY,              XK_w,                    hidewin,        {0} },
	{ MODKEY,					     XK_e,                    restorewin,     {0} },
	{ MODKEY,              XK_o,                    hideotherwins,  {0}},
	{ MODKEY|ShiftMask,    XK_o,                    restoreotherwins, {0}},
	{ MODKEY|ShiftMask,    XK_b,                    togglebar,      {0} },
	{ MODKEY|ShiftMask,    XK_Return,               zoom,           {0} },
	{ MODKEY,              XK_Tab,                  view,           {0} },
	{ MODKEY,   	       XK_q,                    killclient,     {0} },
	{ MODKEY,              XK_t,                    setlayout,      {.v = &layouts[0]} },
    { MODKEY,              XK_f,                    setlayout,      {.v = &layouts[1]} },
	{ MODKEY,              XK_m,                    setlayout,      {.v = &layouts[2]} },
{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,    	       XK_f,                    fullscreen,     {0} },
	{ MODKEY,              XK_space,                setlayout,      {0} },
	{ MODKEY|ShiftMask,    XK_space,                togglefloating, {0} },
	
{ MODKEY|ShiftMask,             XK_g,      setlayout,      {.v = &layouts[3]} },
{ MODKEY|ShiftMask,             XK_n,      setlayout,      {0} },
  { MODKEY,              XK_apostrophe,           togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,              XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,    XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,              XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,              XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,    XK_period,               tagmon,         {.i = +1 } },
	TAGKEYS(               XK_1,                      0)
	TAGKEYS(               XK_2,                      1)
	TAGKEYS(               XK_3,                      2)
	TAGKEYS(               XK_4,                      3)
	TAGKEYS(               XK_5,                      4)
	TAGKEYS(               XK_6,                      5)
	TAGKEYS(               XK_7,                      6)
	TAGKEYS(               XK_8,                      7)
	TAGKEYS(               XK_9,                      8)
	{ MODKEY|ControlMask,  XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button1,        togglewin,      {0} },
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

