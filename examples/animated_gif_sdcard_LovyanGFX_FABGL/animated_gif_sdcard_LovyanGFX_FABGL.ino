#include <SD.h>
#include <SPI.h>
#include <FS.h>
#include <Arduino.h>

#include <LGFX_FabGL_Combined.h>
#include <AnimatedGIF.h>

#define LGFX_USE_V1

// ── Hardware ──────────────────────────────────────────────
#define SD_CS_PIN 13
#define SD_MOSI   12
#define SD_MISO    2
#define SD_SCK    14

// ── Objetos globales ──────────────────────────────────────
fabgl::VGAController displayController;
LGFX display;   // nuestro panel personalizado

AnimatedGIF gif;
File gifFile;
File f;
SPIClass sdSPI(VSPI);

int x_offset, y_offset;

#define DISPLAY_WIDTH  display.width()
#define DISPLAY_HEIGHT display.height()

// ── Callbacks AnimatedGIF ─────────────────────────────────
void * GIFOpenFile(const char *fname, int32_t *pSize)
{
  f = SD.open(fname);
  if (f) {
    *pSize = f.size();
    return (void *)&f;
  }
  return NULL;
}

void GIFCloseFile(void *pHandle)
{
  File *f = static_cast<File *>(pHandle);
  if (f != NULL) f->close();
}

int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
  int32_t iBytesRead = iLen;
  File *f = static_cast<File *>(pFile->fHandle);
  if ((pFile->iSize - pFile->iPos) < iLen)
    iBytesRead = pFile->iSize - pFile->iPos - 1;
  if (iBytesRead <= 0) return 0;
  iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
  pFile->iPos = f->position();
  return iBytesRead;
}

int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  return pFile->iPos;
}

// ── Helpers ───────────────────────────────────────────────
int ErasedFile(char *fname)
{
  int iLen = strlen(fname);
  int i;
  for (i = iLen - 1; i > 0; i--)
    if (fname[i] == '/') break;
  return (fname[i + 1] == '.');
}

void ShowGIF(char *name)
{
  display.fillScreen(TFT_BLACK);
  if (gif.open(name, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw))
  {
    x_offset = (DISPLAY_WIDTH  - gif.getCanvasWidth())  / 2;
    y_offset = (DISPLAY_HEIGHT - gif.getCanvasHeight()) / 2;
    if (x_offset < 0) x_offset = 0;
    if (y_offset < 0) y_offset = 0;
    Serial.printf("GIF abierto; canvas = %d x %d\n",
                  gif.getCanvasWidth(), gif.getCanvasHeight());
    while (gif.playFrame(true, NULL)) {}
    gif.close();
  }
}

// ── Setup ─────────────────────────────────────────────────
void setup()
{
  Serial.begin(115200);
  delay(500);

  // Iniciar VGA
  displayController.begin();
  displayController.setResolution(VGA_320x200_70Hz);
  delay(100);

  // Iniciar display LovyanGFX sobre FabGL
  display.setVGA(&displayController);
  if (!display.init()) {
    Serial.println("ERROR: display.init() falló");
    return;
  }

  display.fillScreen(TFT_BLACK);
  display.setCursor(80, 80);
  display.setTextColor(TFT_WHITE);
  display.print("Hello again <3");
  delay(3000);

  // Iniciar SD
  Serial.println("Inicializando SD...");
  sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS_PIN);

  bool sdOk = false;
  for (int i = 0; i < 3; i++) {
    if (SD.begin(SD_CS_PIN, sdSPI)) { sdOk = true; break; }
    Serial.printf("Intento %d fallido, reintentando...\n", i + 1);
    delay(1000);
  }
  if (!sdOk) {
    Serial.println("SD card fallida tras 3 intentos");
    display.fillScreen(TFT_RED);
    display.setCursor(10, 10);
    display.setTextColor(TFT_WHITE);
    display.print("SD CARD FAILED");
    return;
  }
  Serial.println("SD inicializada.");

  // Listar archivos
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
    Serial.printf("Archivo: %s  (%d bytes)\n", entry.name(), entry.size());
    entry.close();
  }
  root.close();

  gif.begin(LITTLE_ENDIAN_PIXELS);
  display.fillScreen(TFT_BLACK);

  display.changePalette(); // Change Colors
}

// ── Loop ──────────────────────────────────────────────────
void loop()
{
  char *szDir = "/";
  char fname[256];
  File root, temp;


  while (1) {
    root = SD.open(szDir);
    if (root) {
      temp = root.openNextFile();
      while (temp) {
        if (!temp.isDirectory()) {
          strcpy(fname, szDir);
          strcat(fname, temp.name());
          if (!ErasedFile(fname)) {
            Serial.printf("Reproduciendo %s\n", temp.name());
           
            ShowGIF(fname);
           
          }
        }
        temp.close();
        temp = root.openNextFile();
      }
      root.close();
    }
    delay(1000);
  }
}


