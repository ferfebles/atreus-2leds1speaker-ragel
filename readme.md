# Atreus Firmware + Ragel logic

This is a fork from the Atreus Firmware, in which I have implemented the layer
change using only the 'fn' key using the Ragel state machine compiler.

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

I tried to implement all this logic by hand. But every new feature that I wanted
to add introduced several bugs or strange behaviours.

With Ragel, I was able to define a clear state machine implementing the previous
logic, and simplified other problems like the need for a 'fn_decay'.

Adding new features like going directly from any layer to 'Hold', exiting any
layer by clicking once, or avoiding unwanted keys when changing layers was
quite easy.

An interesting trick was adding a disabled layer that blocks all the keys
except for 'fn'. This layer is used in those moments in which you still don't
know in which layer you are, p.e when the 'fn' key just has been pressed and
you don't know if it will be a click (quickly released) or a hold. There are
only two lapse macros defined for the minimum number of cycles needed for a
click and the maximum cycles between a double-click.

The code has a few differences with the Atreus original firmware:

- I have installed two leds that I use to show the layer in which the keyboard
is. There is a function in 'atreus.c' called 'set_leds' that can be deleted.
- The file layout_common.c is compiled from the layout_common.rl, where you can
find all the Ragel logic.
- I use my Atreus with a Windows computer, and write mainly code and English and
Spanish mails or documents. I use the [US International keyboard with no dead
keys](https://answers.microsoft.com/en-us/windows/forum/windows_7-desktop/disable-dead-keys-for-us-international-keyboard/1de44160-83d9-4cd8-9eb3-e6b06b8604a4). The
resulting layout is something like the one you can see in 'zlayout.txt'.
- I removed some files that were used to select different layouts.

I would like to thanks Dr. Adrian D. Thurston for the Ragel compiler. It's
a fantastic tool that I would love to use in other projects. It produces
speedy compact code, and it's invaluable when you try to produce correct code
for parsing data or defining a complex logic. It has been a fun and rewarding
journey to use it. Read more about it in [Ragel](http://www.colm.net/open-source/ragel/).

And thanks to Phil Hagelberg for the Atreus keyboard. It's not only a pleasure
to use it, but to tinker with. Please, go to [Atreus keyboard](https://github.com/technomancy/atreus) if you want to know how to
program your Atreus.

This code is a fork from the Atreus original firmware, and it has the same
GNU GPL v3 license.

The logic code implemented with Ragel is dedicated to the public domain and
follows the [Unlicense](http://unlicense.org/).
