#pragma once
#include <LovyanGFX.hpp>
#include <fabgl.h>

static constexpr uint16_t PANEL_W = 320;
static constexpr uint16_t PANEL_H = 200;





class Panel_FabGL : public lgfx::Panel_Device {
public:
    fabgl::VGAController* _vga = nullptr;
    bool toggle=true;

    Panel_FabGL() {
        _cfg.memory_width  = PANEL_W;
        _cfg.memory_height = PANEL_H;
        _cfg.panel_width   = PANEL_W;
        _cfg.panel_height  = PANEL_H;
        _cfg.offset_x      = 0;
        _cfg.offset_y      = 0;
        
    }

    bool init(bool use_reset) override {
        Serial.printf("[Panel] init — _vga=%s\n", _vga ? "OK" : "NULL");
        return (_vga != nullptr);
    }

    void beginTransaction() override {}
    void endTransaction()   override {}
    void waitDisplay()      override {}
    bool displayBusy()      override { return false; }

    lgfx::color_depth_t setColorDepth(lgfx::color_depth_t depth) override {
        _write_depth = depth;
        _read_depth  = depth;
        return depth;
    }

    void setRotation(uint_fast8_t r) override {
        _rotation = r;
        if (r & 1) { _width = PANEL_H; _height = PANEL_W; }
        else       { _width = PANEL_W; _height = PANEL_H; }
    }

    void setInvert(bool)    override {}
    void setSleep(bool)     override {}
    void setPowerSave(bool) override {}

    void writeCommand(uint32_t, uint_fast8_t) override {}
    void writeData   (uint32_t, uint_fast8_t) override {}
    void initDMA()    override {}
    void waitDMA()    override {}
    bool dmaBusy()    override { return false; }
    void display(uint_fast16_t, uint_fast16_t,
                 uint_fast16_t, uint_fast16_t) override {}
    bool isReadable()  const override { return false; }
    bool isBusShared() const override { return false; }

    void setWindow(uint_fast16_t xs, uint_fast16_t ys,
                   uint_fast16_t xe, uint_fast16_t ye) override {
        _xs = xs; _ys = ys; _xe = xe; _ye = ye;
        _cx = xs; _cy = ys;
    }

    void drawPixelPreclipped(uint_fast16_t x, uint_fast16_t y,
                             uint32_t rawcolor) override {
        if (!_vga) return;
        _vga->setRawPixel(x, y, _toRaw(rawcolor,toggle));
    }

    void writeFillRectPreclipped(uint_fast16_t x, uint_fast16_t y,
                                 uint_fast16_t w, uint_fast16_t h,
                                 uint32_t rawcolor) override {
        if (!_vga) return;
        uint8_t raw = _toRaw(rawcolor,toggle);
        for (uint_fast16_t row = y; row < y + h; row++)
            for (uint_fast16_t col = x; col < x + w; col++)
                _vga->setRawPixel(col, row, raw);
    }

    void writeBlock(uint32_t rawcolor, uint32_t len) override {
        if (!_vga) return;
        uint8_t raw = _toRaw(rawcolor,toggle);
        for (uint32_t i = 0; i < len; i++) {
            _vga->setRawPixel(_cx, _cy, raw);
            if (++_cx > _xe) { _cx = _xs; if (++_cy > _ye) _cy = _ys; }
        }
    }

    void writePixels(lgfx::pixelcopy_t* param,
                     uint32_t len, bool use_dma) override {
        if (!_vga) return;
        // Ruta rapida: src RGB565 sin transparencia — leemos directo del puntero
        if (param->src_depth == lgfx::rgb565_2Byte && param->transp == lgfx::pixelcopy_t::NON_TRANSP) {
            const uint16_t* src = reinterpret_cast<const uint16_t*>(param->src_data);
            if (src) {
                for (uint32_t i = 0; i < len; i++) {
                    _vga->setRawPixel(_cx, _cy, _toRaw(src[i],toggle));
                    if (++_cx > _xe) { _cx = _xs; if (++_cy > _ye) _cy = _ys; }
                }
                return;
            }
        }
        // Fallback: ruta normal por fp_copy con indices relativos
        constexpr uint32_t CHUNK = 64;
        uint16_t buf[CHUNK];
        uint32_t done = 0;
        while (done < len) {
            uint32_t chunk = min((uint32_t)CHUNK, len - done);
            param->fp_copy(buf, 0, chunk, param);
            for (uint32_t i = 0; i < chunk; i++) {
                _vga->setRawPixel(_cx, _cy, _toRaw(buf[i],toggle));
                if (++_cx > _xe) { _cx = _xs; if (++_cy > _ye) _cy = _ys; }
            }
            done += chunk;
        }
    }

    void writeImage(uint_fast16_t x, uint_fast16_t y,
                    uint_fast16_t w, uint_fast16_t h,
                    lgfx::pixelcopy_t* param, bool use_dma) override {
        if (!_vga) return;
        // Ruta rapida para RGB565 sin transparencia (JPEG, pushRect, etc.)
        if (param->src_depth == lgfx::rgb565_2Byte && param->transp == lgfx::pixelcopy_t::NON_TRANSP) {
            const uint16_t* src = reinterpret_cast<const uint16_t*>(param->src_data);
            if (src) {
                setWindow(x, y, x + w - 1, y + h - 1);
                uint32_t len = (uint32_t)w * h;
                for (uint32_t i = 0; i < len; i++) {
                    _vga->setRawPixel(_cx, _cy, _toRaw(src[i],toggle));
                    if (++_cx > _xe) { _cx = _xs; if (++_cy > _ye) _cy = _ys; }
                }
                return;
            }
        }
        // Fallback: ruta normal por fp_copy
        setWindow(x, y, x + w - 1, y + h - 1);
        writePixels(param, (uint32_t)w * h, use_dma);
    }

    void writeImageARGB(uint_fast16_t x, uint_fast16_t y,
                        uint_fast16_t w, uint_fast16_t h,
                        lgfx::pixelcopy_t* param) override {
        writeImage(x, y, w, h, param, false);
    }

    void copyRect(uint_fast16_t, uint_fast16_t,
                  uint_fast16_t, uint_fast16_t,
                  uint_fast16_t, uint_fast16_t) override {}

    uint32_t readCommand(uint_fast16_t,
                         uint_fast8_t = 0,
                         uint_fast8_t = 4) override { return 0; }
    uint32_t readData(uint_fast8_t = 0,
                      uint_fast8_t = 4) override { return 0; }
    void readRect(uint_fast16_t, uint_fast16_t,
                  uint_fast16_t, uint_fast16_t,
                  void*, lgfx::pixelcopy_t*) override {}

private:
    uint_fast16_t _xs=0, _ys=0, _xe=0, _ye=0;
    uint_fast16_t _cx=0, _cy=0;

    // LovyanGFX con setColorDepth(16) pasa rawcolor como RGB565:
    //   bits [15:11] = R (5 bits)
    //   bits [10: 5] = G (6 bits)
    //   bits [ 4: 0] = B (5 bits)
    // Expandimos a RGB888 y dejamos que createRawPixel haga
    // la conversion correcta al formato interno de FabGL (6 bits).
    // Con rgb_order=true LovyanGFX entrega rawcolor en RGB565 estándar:
    //   bits [15:11] = R (5 bits)
    //   bits [10: 5] = G (6 bits)
    //   bits [ 4: 0] = B (5 bits)

    inline uint8_t _toRaw(uint32_t rawcolor,bool t) {
   
    uint16_t r,g,b;

    if (t){
     r = (rawcolor >> 3)  & 0x1F; // bits 7–3
     g = (rawcolor >> 10) & 0x3F; // bits 15–10
     b = (rawcolor>> 8)  & 0x1F; // bits 12–8
        }
    else
     {
        r = ((rawcolor >> 11) & 0x1F);  // 5 bits → 8 bits
        g = ((rawcolor >>  5) & 0x3F);  // 6 bits → 8 bits
        b = ( rawcolor        & 0x1F);  // 5 bits → 8 bits
      }
                 
        r =r << 3;  // 5 bits → 8 bits
        g = g << 2;  // 6 bits → 8 bits
        b = b << 3;  // 5 bits → 8 bits
        
        return _vga->createRawPixel(fabgl::RGB888(r, g, b));
    }
};

class LGFX : public lgfx::LGFX_Device {
    Panel_FabGL _panel;
public:
    LGFX() {

         

        setPanel(&_panel);
    }

    void setVGA(fabgl::VGAController* vga) {
        _panel._vga = vga;
    }

    void changePalette()
    {
        _panel.toggle=!_panel.toggle;
    }

    bool init() {
        bool ok = _panel.init(false);
        Serial.printf("[LGFX] panel.init() = %s\n", ok ? "OK" : "FAIL");
        if (!ok) return false;

        lgfx::LGFX_Device::init();
        setColorDepth(16);
        return true;
    }
};