// -*- compile-command: "ragel -Vp -o layout_common.dot layout_common.rl && dot layout_common.dot -Tpng -o layout_common.png && ragel -pL -G2 -o layout_common.h layout_common.rl" -*-

/* The Atreus has "only" 42 keys. The idea is that the 'fn' key will have
   3 different meanings:
   - hold: when pressed, clicked another key, and then released
   - click: when pressed and released quickly
   - double click: when clicked, and then clicked again

   Each 'meaning' will correspond to a different layer:
   - layer0: the 'normal' layer where each key corresponds to the QWERTY layout
   - layer1: the 'hold' layer, where you put all the keys that usually are
     followed by another from the same layer1 (like numbers or cursor
     movement keys)
   - layer2: the 'click' layer, where I put all the symbols that aren't usually
     followed by another from the same layer2 (like Spanish accented vowels)
   - layer3: the 'double-click' layer, where I put keys that are usually
     followed by another from the same layer (cursors again) and the ones that
     are less used (like function keys)

   With this four layers, you have 42 * 4 = 168 keys. You don't really have so
   many because you want some keys like fn, shift, ctrl, alt or enter in every
   layer. But I've found that I don't need more keys.

   The problem is that the 'meaning' of the layers is not well defined:
   - sometimes you hold fn, press another key, and then release fn before the
     other key.
   - or you try to click fn but press the other key before releasing it

   The logic to solve those problems is full of "Ifs", "elses" and strange
   status variables. Usually, you forget some "If" condition, or modify a status
   variable when you shouldn't, and you keyboard start to act weird without you
   knowing why.

   Then I started to play with Ragel: a compiler for state machines, and I
   tried to solve this problem using it. The resulting logic is working quite
   good for me.

   Another trick was to use a disabled layer (layer -1), for those undefined
   moments when you can't decide which layer to use. The disabled layer will
   not send any key.

   The input of the state machine, is the variable "kb_value", an int with
   only two significant bits:
   - bit 0: is active when fn is pressed
   - bit 1: is active when any other key is pressed

   The state machine uses or modifies other variables, like a state_timer
   to count cycles, the array of keyboard_keys pressed, or the
   current_layer_number.

   There are probably other (simpler?) ways of implementing this logic, but
   this is being a rewarding journey: Ragel is a very interesting technology
   to use, and the Atreus is the keyboard that I enjoy using daily in my job.
 */


// Minimum number of cycles pressing fn to be considered a hold
#define MIN_HOLD_CYCLES 40

// Maximum number of cycles after wich another click does not be
// interpreted as a double click
#define MAX_DOUBLE_CLICK_CYCLES 40

/* function key pressed */
int fn_pressed = 0;

/* microscript variables */
static uint8_t cs;          // current parser state
static const char* eof = 0; // set eof = p = pe to mark input end
int state_timer;            // a timer needed for some states
uint8_t first_click_key;    // first key pressed while in click layer
unsigned char kb_value;     // | Alphanum | Function | -> input to microscript

%%{
  machine microscript;
  alphtype unsigned char;

  # The input for the microscript state machine is the variable "kb_value"
  # with only two significant bits: alpanum (an) + function (fn)
  # This internal constants help in defining transitions
  # constants:     Alphanum (an)   Function (fn)
  none =       0;  #          0               0
  fn   = (3 | 1);  #          0               1
  an   = (3 | 2);  #          1               0
  all  =       3;  #          1               1

  # These are the actions and comparisons used in the state machine
  action ExitError           { fgoto start; }
  # Default value for current_layer_number must be 0 (LayerNormal)
  action LayerDisabled       { current_layer_number = -1; }
  action LayerNormal         { current_layer_number = 0; }
  action LayerHold           { current_layer_number = 1; }
  action LayerClick          { current_layer_number = 2; }
  action LayerDoubleClick    { current_layer_number = 3; }
  action SetHoldTimer        { state_timer = MIN_HOLD_CYCLES; }
  action SetDoubleClickTimer { state_timer = MAX_DOUBLE_CLICK_CYCLES; }
  action IsTimerOn           { --state_timer > 0 }
  action DecTimer            { if (state_timer > 1) state_timer--; }
  action IsaClickKey         { keyboard_keys[0] > 0 }
  action StoreClickKey       { first_click_key = keyboard_keys[0]; }
  action IsClickKeyPressed   { first_click_key == keyboard_keys[0] }
  action ClearClickKey       { first_click_key = 0; }

  # This is the definition of the state machine with 8 states:
  # start, ClickOrHold, Click, ClickKey, NormalOrHold, DoubleClickOrHold,
  # Hold and DoubleClick.
  # The states with an 'Or' are the 'undefined' ones. Each transition to
  # an 'undefined' state uses the LayerDisabled action to avoid sending
  # keys inside the 'undefined' states.
  main := (
    # Normal, every key sent as defined in layer0 (QWERTY)
    start:          (any* :>
                     # if fn pressed goto ClickOrHold
                     fn >LayerDisabled >SetHoldTimer ->ClickOrHold),

    # fn just pressed, LayerDisabled, deciding between click or hold
    ClickOrHold:    (any* :>
                     # if fn still pressed after timer goto Hold
                     ((fn when !IsTimerOn >LayerHold ->Hold) |
                      # fn depressed, goto click
                      (^fn >LayerClick >SetDoubleClickTimer ->Click))),

    # fn pressed and released quickly, waiting for Alphanum key
    Click:          (any* $DecTimer :>
                     # Alphanum pressed (without fn), stored, and goto ClickKey
                     (((an - fn) when IsaClickKey >StoreClickKey ->ClickKey) |
                      # if fn still pressed after doubleclick timer, goto NormalOrHold
                      (fn when !IsTimerOn >LayerDisabled >SetHoldTimer ->NormalOrHold) |
                      # if fn pressed before doubleclick timer, goto DoubleClickOrHold
                      (fn when IsTimerOn >LayerDisabled >SetHoldTimer ->DoubleClickOrHold))),

    # click and then Alphanum key pressed and stored, waiting for release the stored key then goto start
    ClickKey:       (any* :> (any when !IsClickKeyPressed) >ClearClickKey >LayerNormal ->start),

    # after first click, doubleclick timer passed, and fn pressed again, LayerDisabled
    NormalOrHold:   (any* :>
                     # fn still pressed after HoldTimer, click discarded, goto Hold
                     ((fn when !IsTimerOn >LayerHold ->Hold) |
                      # fn released quickly, click discarded, goto start
                      (^fn >LayerNormal ->start))),

    # fn clicked, and fn pressed again before doubleclick timer passed, LayerDisabled
    DoubleClickOrHold: (any* :>
                     # fn still pressed after HoldTimer, doubleclick discarded, goto Hold
                     ((fn when !IsTimerOn >LayerHold ->Hold) |
                      # fn released, goto doubleclick
                      (^fn >LayerDoubleClick ->DoubleClick))),

    # fn pressed long enough, return to normal when no key (not Fn, nor Alphanum) pressed
    Hold:           (any* :> none >LayerNormal ->start),

    # two quick fn clicks, permanent layer until fn pressed
    DoubleClick:    (any* :> fn >LayerDisabled >SetHoldTimer ->NormalOrHold)
  );
}%%

%%{ write data;}%%

void init_microscript( void ) {
  %%{ write init;}%%
}

void parse_microscript(void) {
  unsigned char* p = &kb_value;
  const unsigned char* pe = p + 1; // end pointer

  %%{ write exec;}%%
}

void activate_fn() {
  fn_pressed = 1;
};

void (*layer_functions[])(void) = {reset, activate_fn};

void per_cycle() {
  /* kb_value:  usb_presses  fn_pressed */
  /*        0             0           0 */
  /*        1             0           1 */
  /*        2             1           0 */
  /*        3             1           1 */
  kb_value = 2 * (usb_presses > 0) + fn_pressed;
  fn_pressed = 0;
  parse_microscript();
};
