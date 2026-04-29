#include <Arduino.h>
#include <fabgl.h>
#include "LGFX_FabGL.h"

fabgl::VGAController displayController;
LGFX display;

void setup() {
    Serial.begin(115200);
    delay(500);

    displayController.begin();
    displayController.setResolution(VGA_320x200_70Hz);
    delay(100);

    display.setVGA(&displayController);
    display.init();
    display.setColorDepth(16);

    Serial.println("Iniciando dibujo...");

    display.changePalette();
    // Test rápido de colores
    display.fillScreen(TFT_YELLOW);
    display.fillCircle(80,  100, 50, TFT_RED);
    display.fillCircle(80,  100, 20, TFT_PURPLE);
    display.drawPixel(80,100,0xF800);
    display.fillCircle(160, 100, 50, TFT_GREEN);
    display.fillCircle(240, 100, 50, TFT_BLUE);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(2);
    display.drawString("LovyanGFX OK", 60, 160);

    Serial.println("Dibujo completado.");

    Serial.println("Dibujo completado.");
}

void loop() {}