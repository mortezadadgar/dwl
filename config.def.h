#include <X11/XF86keysym.h>

#define TERMINAL "foot"

/* Taken from https://github.com/djpohly/dwl/issues/466 */
#define COLOR(hex)    { ((hex >> 24) & 0xFF) / 255.0f, \
                        ((hex >> 16) & 0xFF) / 255.0f, \
                        ((hex >> 8) & 0xFF) / 255.0f, \
                        (hex & 0xFF) / 255.0f }
/* appearance */
static const int sloppyfocus               = 0;  /* focus follows mouse */
static const int bypass_surface_visibility = 0;  /* 1 means idle inhibitors will disable idle tracking even if it's surface isn't visible  */
static const int smartgaps                 = 0;  /* 1 means no outer gap when there is only one window */
static int gaps                            = 1;  /* 1 means gaps between windows are added */
static const unsigned int gappx            = 10; /* gap pixel between windows */
static const unsigned int borderpx         = 2;  /* border pixel of windows */
static const float rootcolor[]             = COLOR(0x000000ff);
static const float bordercolor[]           = COLOR(0x444444ff);
static const float focuscolor[]            = COLOR(0xC4A7E7ff);
static const float urgentcolor[]           = COLOR(0xEB6F92ff);
/* This conforms to the xdg-protocol. Set the alpha to zero to restore the old behavior */
static const float fullscreen_bg[]         = {0.1f, 0.1f, 0.1f, 1.0f}; /* You can also use glsl colors */
/* keyboard layout change notification for status bar */
static const char  kblayout_file[] = "";
static const char *kblayout_cmd[]  = {NULL};

/* tagging - TAGCOUNT must be no greater than 31 */
#define TAGCOUNT (9)

/* logging */
static int log_level = WLR_ERROR;

/* NOTE: ALWAYS keep a rule declared even if you don't use rules (e.g leave at least one example) */
static const Rule rules[] = {
        /* app_id                    title            tags mask     isfloating   monitor scratchkey */
        { "firefox",                 NULL,            1   << 1,     0,           -1,     0 },
        { "chromium",                NULL,            1   << 1,     0,           -1,     0 },
        { "brave-browser",           NULL,            1   << 1,     0,           -1,     0 },
        { "org.telegram.desktop",    NULL,            1   << 2,     0,           -1,     0 },
        { "steam",                   NULL,            1   << 2,     1,           -1,     0 },
        { "discord",                 NULL,            1   << 2,     0,           -1,     0 },
        { "mpv",                     NULL,            1   << 3,     0,           -1,     0 },
        { "org.pwmt.zathura",        NULL,            1   << 4,     0,           -1,     0 },
        { "thunderbird",             NULL,            1   << 6,     0,           -1,     0 },
        { "qBittorrent",             NULL,            1   << 7,     1,           -1,     0 },
        { "Clockify",                NULL,            0,            1,           -1,     0 },
        { "oblivion-desktop",        NULL,            0,            1,           -1,     0 },
        { "hiddify",                 NULL,            0,            1,           -1,     0 },
        { "obsidian",                NULL,            0,            0,           -1,     0 },
        { "org.gnome.Calculator",    NULL,            0,            1,           -1,     0 },
        { NULL,                      "floating",      0,            1,           -1,     0 },
        { NULL,                      "Windscribe",    0,            1,           -1,     0 },
        { NULL,                      "ncmpcpp",       1   << 5,     1,           -1,     0 },
	{ NULL,                      "scratchpad",    0,            1,           -1,     's' },
};

/* layout(s) */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* monitors */
/* (x=-1, y=-1) is reserved as an "autoconfigure" monitor position indicator
 * WARNING: negative values other than (-1, -1) cause problems with Xwayland clients
 * https://gitlab.freedesktop.org/xorg/xserver/-/issues/899
*/
/* NOTE: ALWAYS add a fallback rule, even if you are completely sure it won't be used */
static const MonitorRule monrules[] = {
	/* name       mfact  nmaster scale layout       rotate/reflect                x    y */
	/* example of a HiDPI laptop monitor:
	{ "eDP-1",    0.5f,  1,      2,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 },
	*/
	/* defaults */
	{ NULL,       0.55f, 1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 },
};

/* keyboard */
static const struct xkb_rule_names xkb_rules = {
	/* can specify fields: rules, model, layout, variant, options */
	/* example:
	.options = "ctrl:nocaps",
	*/
        .layout = "us,ir",
        .options = "caps:escape,grp:alt_shift_toggle",
};

static const int repeat_rate = 50;
static const int repeat_delay = 300;

/* Trackpad */
static const int tap_to_click = 1;
static const int tap_and_drag = 1;
static const int drag_lock = 1;
static const int natural_scrolling = 0;
static const int disable_while_typing = 1;
static const int left_handed = 0;
static const int middle_button_emulation = 0;
/* You can choose between:
LIBINPUT_CONFIG_SCROLL_NO_SCROLL
LIBINPUT_CONFIG_SCROLL_2FG
LIBINPUT_CONFIG_SCROLL_EDGE
LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN
*/
static const enum libinput_config_scroll_method scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;

/* You can choose between:
LIBINPUT_CONFIG_CLICK_METHOD_NONE
LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS
LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER
*/
static const enum libinput_config_click_method click_method = LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS;

/* You can choose between:
LIBINPUT_CONFIG_SEND_EVENTS_ENABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE
*/
static const uint32_t send_events_mode = LIBINPUT_CONFIG_SEND_EVENTS_ENABLED;

/* You can choose between:
LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT
LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE
*/
static const enum libinput_config_accel_profile accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;
static const double accel_speed = 0.0;

/* You can choose between:
LIBINPUT_CONFIG_TAP_MAP_LRM -- 1/2/3 finger tap maps to left/right/middle
LIBINPUT_CONFIG_TAP_MAP_LMR -- 1/2/3 finger tap maps to left/middle/right
*/
static const enum libinput_config_tap_button_map button_map = LIBINPUT_CONFIG_TAP_MAP_LRM;

/* If you want to use the windows key for MODKEY, use WLR_MODIFIER_LOGO */
#define MODKEY WLR_MODIFIER_LOGO

#define TAGKEYS(KEY,SKEY,TAG) \
	{ MODKEY,                    KEY,            view,            {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL,  KEY,            toggleview,      {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_SHIFT, SKEY,           tag,             {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT,SKEY,toggletag, {.ui = 1 << TAG} }

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd[] = { TERMINAL, NULL };
static const char *menucmd[] = { "fuzzel", NULL };

/* named scratchpads - First arg only serves to match against key in rules*/
static const char *scratchpadcmd[] = { "s", TERMINAL, "-T", "scratchpad", "-w", "800x600", NULL };

static const Key keys[] = {
	/* Note that Shift changes certain key codes: c -> C, 2 -> at, etc. */
	/* modifier                    key                 function        argument */
	{ MODKEY,                      XKB_KEY_d,          spawn,          {.v = menucmd} },
	{ MODKEY,                      XKB_KEY_Return,     spawn,          {.v = termcmd} },
	{ MODKEY,                      XKB_KEY_u,          togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                      XKB_KEY_b,          togglebar,      {0} },
	{ MODKEY,                      XKB_KEY_j,          focusstack,     {.i = +1} },
	{ MODKEY,                      XKB_KEY_k,          focusstack,     {.i = -1} },
	{ MODKEY,                      XKB_KEY_i,          incnmaster,     {.i = +1} },
	{ MODKEY,                      XKB_KEY_d,          incnmaster,     {.i = -1} },
	{ MODKEY,                      XKB_KEY_h,          setmfact,       {.f = -0.05f} },
	{ MODKEY,                      XKB_KEY_l,          setmfact,       {.f = +0.05f} },
	{ MODKEY,                      XKB_KEY_space,      zoom,           {0} },
	{ MODKEY,                      XKB_KEY_Tab,        view,           {0} },
	{ MODKEY,                      XKB_KEY_g,          togglegaps,     {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT,   XKB_KEY_Q,          killclient,     {0} },
	{ MODKEY,                      XKB_KEY_t,          setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                      XKB_KEY_f,          setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                      XKB_KEY_m,          setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                      XKB_KEY_s,          togglefloating, {0} },
	{ MODKEY,                      XKB_KEY_e,          togglefullscreen, {0} },
	{ MODKEY,                      XKB_KEY_0,          view,           {.ui = ~0} },
	{ MODKEY|WLR_MODIFIER_SHIFT,   XKB_KEY_parenright, tag,            {.ui = ~0} },
	{ MODKEY,                      XKB_KEY_comma,      focusmon,       {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY,                      XKB_KEY_period,     focusmon,       {.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY|WLR_MODIFIER_SHIFT,   XKB_KEY_less,       tagmon,         {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_SHIFT,   XKB_KEY_greater,    tagmon,         {.i = WLR_DIRECTION_RIGHT} },
        { MODKEY|WLR_MODIFIER_CTRL,    XKB_KEY_equal,      spawn,          SHCMD("dmenupass") },
        { MODKEY|WLR_MODIFIER_CTRL,    XKB_KEY_p,          spawn,          SHCMD("dmenupower") },
        { MODKEY|WLR_MODIFIER_CTRL,    XKB_KEY_n,          spawn,          SHCMD("dmenuiwd") },
        { MODKEY|WLR_MODIFIER_CTRL,    XKB_KEY_m,          spawn,          SHCMD("open-ncmpcpp") },
        { MODKEY,                      XKB_KEY_Print,      spawn,          SHCMD("grimshot") },
        { 0, XF86XK_MonBrightnessUp,   spawn,              SHCMD("light -A 3") },
        { 0, XF86XK_MonBrightnessDown, spawn,              SHCMD("light -U 3") },
        { 0, XF86XK_AudioNext,         spawn,              SHCMD("playerctl next") },
        { 0, XF86XK_AudioPrev,         spawn,              SHCMD("playerctl previous") },
        { 0, XF86XK_AudioPlay,         spawn,              SHCMD("playerctl play-pause") },
        { 0, XF86XK_AudioStop,         spawn,              SHCMD("playerctl stop") },
	{ 0, XF86XK_AudioRaiseVolume,  spawn,              SHCMD("wpctl set-volume -l 1.5 @DEFAULT_AUDIO_SINK@ 5%+") },
	{ 0, XF86XK_AudioLowerVolume,  spawn,              SHCMD("wpctl set-volume -l 1.5 @DEFAULT_AUDIO_SINK@ 5%-") },
	{ 0, XF86XK_AudioMute,         spawn,              SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle") },
	// non-multimedia keyboards
	{ 0,      XKB_KEY_F11,         spawn,              SHCMD("ddcutil setvcp 10 - 10 --skip-ddc-checks --bus 3") },
	{ 0,      XKB_KEY_F12,         spawn,              SHCMD("ddcutil setvcp 10 + 10 --skip-ddc-checks --bus 3") },
	{ 0,      XKB_KEY_End,         spawn,              SHCMD("playerctl next") },
	{ 0,      XKB_KEY_Home,        spawn,              SHCMD("playerctl previous") },
	{ MODKEY, XKB_KEY_End,         spawn,              SHCMD("playerctl position 10+") },
	{ MODKEY, XKB_KEY_Home,        spawn,              SHCMD("playerctl position 10-") },
	{ 0,      XKB_KEY_Insert,      spawn,              SHCMD("playerctl play-pause") },
	{ MODKEY, XKB_KEY_Insert,      spawn,              SHCMD("playerctl stop") },
	{ 0,      XKB_KEY_Page_Up,     spawn,              SHCMD("wpctl set-volume -l 1.5 @DEFAULT_AUDIO_SINK@ 5%+") },
	{ 0,      XKB_KEY_Page_Down,   spawn,              SHCMD("wpctl set-volume -l 1.5 @DEFAULT_AUDIO_SINK@ 5%-") },
	{ 0,      XKB_KEY_Pause,       spawn,              SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle") },
	{ 0,      XKB_KEY_Scroll_Lock, spawn,              SHCMD("wpctl set-mute @DEFAULT_AUDIO_SOURCE@ toggle; pkill -RTMIN+8 waybar") },
        { WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT,            XKB_KEY_space,      spawn,          SHCMD("makoctl restore") },
        { WLR_MODIFIER_CTRL,                               XKB_KEY_space,      spawn,          SHCMD("makoctl dismiss") },
	TAGKEYS(          XKB_KEY_1, XKB_KEY_exclam,                     0),
	TAGKEYS(          XKB_KEY_2, XKB_KEY_at,                         1),
	TAGKEYS(          XKB_KEY_3, XKB_KEY_numbersign,                 2),
	TAGKEYS(          XKB_KEY_4, XKB_KEY_dollar,                     3),
	TAGKEYS(          XKB_KEY_5, XKB_KEY_percent,                    4),
	TAGKEYS(          XKB_KEY_6, XKB_KEY_asciicircum,                5),
	TAGKEYS(          XKB_KEY_7, XKB_KEY_ampersand,                  6),
	TAGKEYS(          XKB_KEY_8, XKB_KEY_asterisk,                   7),
	TAGKEYS(          XKB_KEY_9, XKB_KEY_parenleft,                  8),

	/* Ctrl-Alt-Backspace and Ctrl-Alt-Fx used to be handled by X server */
	{ WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_Terminate_Server, quit, {0} },
	/* Ctrl-Alt-Fx is used to switch to another VT, if you don't know what a VT is
	 * do not remove them.
	 */
#define CHVT(n) { WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_XF86Switch_VT_##n, chvt, {.ui = (n)} }
	CHVT(1), CHVT(2), CHVT(3), CHVT(4), CHVT(5), CHVT(6),
	CHVT(7), CHVT(8), CHVT(9), CHVT(10), CHVT(11), CHVT(12),
};

static const Button buttons[] = {
	{ MODKEY, BTN_LEFT,   moveresize,     {.ui = CurMove} },
	{ MODKEY, BTN_MIDDLE, togglefloating, {0} },
	{ MODKEY, BTN_RIGHT,  moveresize,     {.ui = CurResize} },
};
