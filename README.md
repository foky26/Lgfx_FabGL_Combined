I always wanted to use my code in LovyanGFX with my ESP32 TTGO VGA32 board.
But LovyanGFX has not defined an VGA output, so it was hard to get it.
FABGL is the reference library for TTGO VGA, so I thouth it could be possible to integrate FABL as a Custom Panel in LovyanGFX,
so that FABGL made the interface to VGA, while I could still use the methods of LovyanGFX and so, reuse the script I had for Lovyan.

You will need, of course, the libraries LovyanGFX and FabGL installed.

For the animated gif example, you will need also the AnimatedGIF library.

The Panel connection is defined in LGFX_FabGL_Combined.h file. 
This is the piece you have to reuse in your code.
Enjoy! Now you can use yor scripts in LovyanGFX in your TTGO VGA32.
