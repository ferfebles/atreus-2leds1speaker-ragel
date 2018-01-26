// the function/arrow/nav-cluster layer, aka L2.
// this is shared among several layouts.

int layer2[44] = {
    ALTGR(KEY_1), SHIFT(KEY_2), ALTGR(KEY_E),      SHIFT(KEY_LEFT_BRACE), SHIFT(KEY_RIGHT_BRACE), 0,                  ALTGR(KEY_Y), ALTGR(KEY_U),    ALTGR(KEY_I), ALTGR(KEY_O),        ALTGR(KEY_5),
    ALTGR(KEY_A), SHIFT(KEY_4), SHIFT(KEY_MINUS),  SHIFT(KEY_9),          SHIFT(KEY_0),           0,                  0,            0,               0,            SHIFT(ALTGR(KEY_4)), ALTGR(SHIFT(KEY_SEMICOLON)),
    0,            0,            ALTGR(KEY_COMMA),  KEY_LEFT_BRACE,        KEY_RIGHT_BRACE,        KEYBOARD_LEFT_CTRL, ALTGR(KEY_N), ALTGR(KEY_M),    0,            SHIFT(KEY_6),        ALTGR(KEY_SLASH),
    0,            KEY_ESC,      KEYBOARD_LEFT_GUI, KEYBOARD_LEFT_SHIFT,   KEY_DELETE,             KEYBOARD_LEFT_ALT,  KEY_SPACE,    PRE_FUNCTION(1), 0,            KEY_TILDE,           KEY_ENTER };

int layer3[44] = {
    0,              KEY_HOME,        KEY_UP,            KEY_END,             KEYBOARD_VOLUME_UP,   KEY_0,              KEY_PAGE_UP,   KEY_F7,          KEY_F8, KEY_F9, KEY_F12,
    0,              KEY_LEFT,        KEY_DOWN,          KEY_RIGHT,           KEYBOARD_VOLUME_DOWN, KEY_0,              KEY_PAGE_DOWN, KEY_F4,          KEY_F5, KEY_F6, KEY_F11,
    KEYBOARD_POWER, KEY_PRINTSCREEN, CTRL(KEY_PAUSE),   0,                   KEYBOARD_MUTE,        KEYBOARD_LEFT_CTRL, 0,             KEY_F1,          KEY_F2, KEY_F3, KEY_F10,
    KEY_CAPS_LOCK,  FUNCTION(0),     KEYBOARD_LEFT_GUI, KEYBOARD_LEFT_SHIFT, KEY_DELETE,           KEYBOARD_LEFT_ALT,  KEY_SPACE,     PRE_FUNCTION(1), 0,      0,      KEY_ENTER };
