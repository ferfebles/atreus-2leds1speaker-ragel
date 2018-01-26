
/* #line 1 "layout_common.rl" */
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


/* #line 149 "layout_common.rl" */



/* #line 82 "layout_common.h" */
static const int microscript_start = 1;
static const int microscript_first_final = 9;
static const int microscript_error = 0;

static const int microscript_en_main = 1;


/* #line 151 "layout_common.rl" */


void init_microscript( void ) {
  
/* #line 95 "layout_common.h" */
	{
	cs = microscript_start;
	}

/* #line 154 "layout_common.rl" */

}

void parse_microscript(void) {
  unsigned char* p = &kb_value;
  const unsigned char* pe = p + 1; // end pointer

  
/* #line 109 "layout_common.h" */
	{
	short _widec;
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
tr10:
/* #line 90 "layout_common.rl" */
	{ current_layer_number = 0; }
	goto st1;
tr16:
/* #line 101 "layout_common.rl" */
	{ first_click_key = 0; }
/* #line 90 "layout_common.rl" */
	{ current_layer_number = 0; }
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
/* #line 130 "layout_common.h" */
	switch( (*p) ) {
		case 1u: goto tr1;
		case 3u: goto tr1;
	}
	goto st1;
tr1:
/* #line 89 "layout_common.rl" */
	{ current_layer_number = -1; }
/* #line 94 "layout_common.rl" */
	{ state_timer = MIN_HOLD_CYCLES; }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
/* #line 146 "layout_common.h" */
	_widec = (*p);
	if ( (*p) > 1u ) {
		if ( 3u <= (*p) && (*p) <= 3u ) {
			_widec = (short)(256u + ((*p) - 0u));
			if ( 
/* #line 96 "layout_common.rl" */
 --state_timer > 0  ) _widec += 256;
		}
	} else if ( (*p) >= 1u ) {
		_widec = (short)(256u + ((*p) - 0u));
		if ( 
/* #line 96 "layout_common.rl" */
 --state_timer > 0  ) _widec += 256;
	}
	switch( _widec ) {
		case 0: goto tr2;
		case 2: goto tr2;
		case 257: goto tr4;
		case 259: goto tr4;
		case 513: goto st2;
		case 515: goto st2;
	}
	if ( 4 <= _widec )
		goto tr2;
	goto st0;
tr2:
/* #line 92 "layout_common.rl" */
	{ current_layer_number = 2; }
/* #line 95 "layout_common.rl" */
	{ state_timer = MAX_DOUBLE_CLICK_CYCLES; }
	goto st3;
tr6:
/* #line 97 "layout_common.rl" */
	{ if (state_timer > 1) state_timer--; }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
/* #line 186 "layout_common.h" */
	_widec = (*p);
	if ( (*p) < 2u ) {
		if ( 1u <= (*p) && (*p) <= 1u ) {
			_widec = (short)(256u + ((*p) - 0u));
			if ( 
/* #line 96 "layout_common.rl" */
 --state_timer > 0  ) _widec += 256;
		}
	} else if ( (*p) > 2u ) {
		if ( 3u <= (*p) && (*p) <= 3u ) {
			_widec = (short)(256u + ((*p) - 0u));
			if ( 
/* #line 96 "layout_common.rl" */
 --state_timer > 0  ) _widec += 256;
		}
	} else {
		_widec = (short)(768u + ((*p) - 0u));
		if ( 
/* #line 98 "layout_common.rl" */
 keyboard_keys[0] > 0  ) _widec += 256;
	}
	switch( _widec ) {
		case 0: goto tr6;
		case 257: goto tr7;
		case 259: goto tr7;
		case 513: goto tr8;
		case 515: goto tr8;
		case 770: goto tr6;
		case 1026: goto tr9;
	}
	if ( 4 <= _widec )
		goto tr6;
	goto st0;
st0:
cs = 0;
	goto _out;
tr7:
/* #line 89 "layout_common.rl" */
	{ current_layer_number = -1; }
/* #line 94 "layout_common.rl" */
	{ state_timer = MIN_HOLD_CYCLES; }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
/* #line 233 "layout_common.h" */
	_widec = (*p);
	if ( (*p) > 1u ) {
		if ( 3u <= (*p) && (*p) <= 3u ) {
			_widec = (short)(256u + ((*p) - 0u));
			if ( 
/* #line 96 "layout_common.rl" */
 --state_timer > 0  ) _widec += 256;
		}
	} else if ( (*p) >= 1u ) {
		_widec = (short)(256u + ((*p) - 0u));
		if ( 
/* #line 96 "layout_common.rl" */
 --state_timer > 0  ) _widec += 256;
	}
	switch( _widec ) {
		case 0: goto tr10;
		case 2: goto tr10;
		case 257: goto tr4;
		case 259: goto tr4;
		case 513: goto st4;
		case 515: goto st4;
	}
	if ( 4 <= _widec )
		goto tr10;
	goto st0;
tr4:
/* #line 91 "layout_common.rl" */
	{ current_layer_number = 1; }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
/* #line 267 "layout_common.h" */
	if ( (*p) == 0u )
		goto tr10;
	goto st5;
tr8:
/* #line 89 "layout_common.rl" */
	{ current_layer_number = -1; }
/* #line 94 "layout_common.rl" */
	{ state_timer = MIN_HOLD_CYCLES; }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
/* #line 281 "layout_common.h" */
	_widec = (*p);
	if ( (*p) > 1u ) {
		if ( 3u <= (*p) && (*p) <= 3u ) {
			_widec = (short)(256u + ((*p) - 0u));
			if ( 
/* #line 96 "layout_common.rl" */
 --state_timer > 0  ) _widec += 256;
		}
	} else if ( (*p) >= 1u ) {
		_widec = (short)(256u + ((*p) - 0u));
		if ( 
/* #line 96 "layout_common.rl" */
 --state_timer > 0  ) _widec += 256;
	}
	switch( _widec ) {
		case 0: goto tr13;
		case 2: goto tr13;
		case 257: goto tr4;
		case 259: goto tr4;
		case 513: goto st6;
		case 515: goto st6;
	}
	if ( 4 <= _widec )
		goto tr13;
	goto st0;
tr13:
/* #line 93 "layout_common.rl" */
	{ current_layer_number = 3; }
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
/* #line 315 "layout_common.h" */
	switch( (*p) ) {
		case 1u: goto tr7;
		case 3u: goto tr7;
	}
	goto st7;
tr9:
/* #line 99 "layout_common.rl" */
	{ first_click_key = keyboard_keys[0]; }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
/* #line 329 "layout_common.h" */
	_widec = (*p);
	_widec = (short)(1280u + ((*p) - 0u));
	if ( 
/* #line 100 "layout_common.rl" */
 first_click_key == keyboard_keys[0]  ) _widec += 256;
	if ( _widec > 1535 ) {
		if ( 1536 <= _widec && _widec <= 1791 )
			goto st8;
	} else if ( _widec >= 1280 )
		goto tr16;
	goto st0;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

/* #line 161 "layout_common.rl" */

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
