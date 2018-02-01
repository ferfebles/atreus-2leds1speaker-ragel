# Atreus Firmware + Ragel logic

This is a fork from the Atreus Firmware, in which I have implemented the layer
change using only the 'fn' key with the Ragel state machine compiler.

The idea was that the keyboard will have 4 layers:

- Normal: the usual QWERTY layout
- Hold: while holding down the 'fn' key. I put here all the keys that usually
are followed by another from the same layer (like numbers or cursor movement
keys).
- Click: a transient layer activated by pressing and releasing quickly the
'fn' key. It will last only for one key. I put here all the symbols that aren't
usually followed by another from the same layer (like Spanish accented vowels).
- Double Click: a layer activated by double-clicking the 'fn' key, where I put
the keys that are usually followed by another from the same layer (cursors
again) and the ones that I use less (like function keys).

I know that there are advanced keyboard firmwares that allow you to do that,
but I really wanted to do it by myself, just for fun. The original Atreus 
firmware was perfect because the code is small, elegant and easy to understand.

![Atreus with leds](https://github.com/ferfebles/atreus-2leds1speaker-ragel/raw/master/atreus-leds.jpg)

I implemented all this logic by hand inside the per_cycle() function: about 40 
lines of 'if', 'else if' and several status variables and counters. It was 
'almost' working but sometimes the keyboard acted weird: sending the two 
characters from different layers of the same key, or eating one key when two
were pressed quickly.

And more importantly, it felt wrong. Trying to implement something new always ended
with an unusable firmware and a lot of tests until I put it to work again.

The classic solution for this kind of logic is to draw your states and transitions,
use a variable for the states, some as inputs for your transitions, and write a 
(hopefully more clear) bunch of 'ifs' and 'elses'.

The problem is that even drawing a few states, you usually forget to draw all the 
transitions for your input variables: the typical error is to do something when a>0, 
another thing when a<0 and then forget the case a=0. The real problems are usually 
more subtle and harder to spot.

Here is where a state machine compiler helps a lot. You define the state machine
with some code and the first thing you should do is to ask it to 'draw' a picture 
of your state machine. You look at it and is like... WTF!! I didn't mean that. Then 
you realize that you forgot something or that you enabled a strange loop. Without 
having to fry the Atmega and to press a lot of keys trying to spot any problem. 

This is the 'picture' of my keyboard layer logic.

![Ragel state machine](https://github.com/ferfebles/atreus-2leds1speaker-ragel/raw/master/layout_common.png)

With Ragel, I was able to define a clear state machine implementing the previous
logic, and simplified other problems like the need for a 'fn_decay'.

Usually, state machines have a 'string' or 'array' as input, a start state, and at
the end of the process, the current state is your output. Ragel is very interesting
because it allows you to:

- Process the input piece by piece
- Decide the next transition not only based on the input but on conditions of other variables.
- Execute code when entering/staying/leaving states or transitions. Even code that changes the input or the states.

This makes feasible to write a machine that is not meant to end, only to process the
next input piece, and modify the variables that you want. In this case, the input is 
an 'int' with only two significant bits, one for 'fn' pressed and another for any 
alphanumeric key pressed. The output is the layer that the keyboard should be in.

Ragel allowed me to add some logic easily:

- An interesting trick was adding a disabled layer (layer -1) that blocks all
the keys except for 'fn'. This layer is used in those moments in which you 
still don't know in which layer you should be, p.e when the 'fn' key just has
been pressed and the state machine can't know if you are going to release it
quickly as a click, or you'll be holding it for a while.
- To store the first key pressed when in click layer, and only allow this key until
released because sometimes when writing quickly, the next key happened in the click
layer.
- To disable the click layer by pressing again the 'fn' key after the double click
time has passed.

Some warnings if you want to use Ragel:

- It makes much more than this. It's very interesting (for other uses) that it allows 
deciding your next transition based on a regexp of the input. This can simplify your 
state machine a lot, and the documentation talks a lot about it. But the most needed
feature for my logic was to decide your transition using conditions, and it was hidden 
in the last pages of the documentation.
- You should use 'actions' for your code and comparisons inside Ragel (they are like 
functions). Not only makes your code easier to read but allows Ragel to identify the
same code when you use it in different places and to simplify the resulting state machine.

The code has a few differences with the Atreus original firmware:

- All the Ragel code is inside the 'layout_common.rl'. It gets compiled to 'layout_common.png' 
first, and then to 'layout_common.h' that is a cryptic C code meant only for GCC.
- The per_cycle() function only sets the input (kb_value) and calls the state machine to parse it.
- There are two changes in calculate_presses() inside 'atreus.c' to block keypresses when 
layer_disable, and to allow only for the first_click_key when you are in click_layer.
- I removed some files that were used to select different layouts.

If you want to use this code, you should comment the 'set_leds()' function (I modded 
the Atreus with two LEDs) and change 'layout.h' and 'layer2.h', because I use a modified 
[US International keyboard with no dead
keys](https://answers.microsoft.com/en-us/windows/forum/windows_7-desktop/disable-dead-keys-for-us-international-keyboard/1de44160-83d9-4cd8-9eb3-e6b06b8604a4) layout under Windows (this allows me to send 
Spanish accented keys in the click layer and frees the single quote key from being 
a dead key)

I would like to thanks Dr. Adrian D. Thurston for the Ragel compiler. It's
a fantastic tool that I would love to use in other projects. It produces
speedy compact code, and it's invaluable when you try to produce correct code
for parsing data or defining a complex logic. It has been a fun and rewarding
journey to use it. Read more about it in [Ragel](http://www.colm.net/open-source/ragel/).

And thanks to Phil Hagelberg for the Atreus keyboard. It's not only a pleasure
to use it every day, but to tinker with. Please, go to [Atreus keyboard](https://github.com/technomancy/atreus) if you want to know how to
program your Atreus.

This code is a fork from the Atreus original firmware, and it has the same
GNU GPL v3 license.

The logic code implemented with Ragel is dedicated to the public domain and
follows the [Unlicense](http://unlicense.org/).
