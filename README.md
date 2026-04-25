I always wanted to use my code in LovyanGFX with my ESP32 TTGO VGA32 board.
But LovyanGFX has not defined an VGA output, so it was hard to get it.
FABGL is the reference library for TTGO VGA, so I thouth it could be possible to integrate FABL as a Custom Panel in LovyanGFX,
so that FABGL made the interface to VGA, while I could still use the methods of LovyanGFX and so, reuse the script I had for Lovyan.

Here you have two examples:
- Prueba_LovyanGFX_FABGL is demo test of integration of both libraries.
- animated_gif_sdcard_LovyanGFX_FABGL is a script that reads all the animated gif in your SD and plays them in loop.

You will need, of course, the libraries LovyanGFX and FabGL installed.
For the second example, you will need also the AnimatedGIF library.

Enjoy! Now you can use yor scripts in LovyanGFX in your TTGO VGA32.
