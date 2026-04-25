#include <Arduino.h>
#include <fabgl.h>
#include "LGFX_FabGL.h"

fabgl::VGAController displayController;
LGFX_FabGL display;

void setup() {
    Serial.begin(115200);
    delay(500);

    displayController.begin();
    displayController.setResolution(VGA_320x200_70Hz);
    delay(100);

    display.setVGA(&displayController);
    display.init();

    Serial.println("Iniciando dibujo...");

    // Fondo negro
    display.fillScreen(TFT_BLACK);
    delay(500);

    // Test rápido de colores
    display.fillScreen(TFT_RED);
    delay(800);
    display.fillScreen(TFT_GREEN);
    delay(800);
    display.fillScreen(TFT_BLUE);
    delay(800);

    // Dibujo final
    display.fillScreen(TFT_BLACK);
    display.drawRect(0, 0, 320, 200, TFT_WHITE);
    display.fillCircle(80,  100, 50, TFT_RED);
    display.fillCircle(80,  100, 20, TFT_WHITE);
    display.fillCircle(160, 100, 50, TFT_GREEN);
    display.fillCircle(240, 100, 50, TFT_BLUE);
    display.setTextColor(TFT_WHITE);
    display.setTextSize(2);
    display.drawString("LovyanGFX OK", 60, 160);

    Serial.println("Dibujo completado.");
}

void loop() {}