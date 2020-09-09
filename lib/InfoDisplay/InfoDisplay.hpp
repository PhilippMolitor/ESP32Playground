#include <Arduino.h>
#include <U8g2lib.h>
#include "AppState.hpp"

struct ScreenMode
{
    enum ScreenModeType
    {
        Battery,
        LoadTime,
        Disabled
    };
};

class InfoDisplay
{
private:
    // state
    U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C _display;
    ScreenMode::ScreenModeType _mode = ScreenMode::Battery;
    AppState *_state;
    bool _isSelected = false;

    void drawMode1()
    {
        _display.setFont(u8g2_font_6x10_tr);

        char buf[20] = "";
        sprintf(buf, "Active Display: %d", _state->activeDisplay);

        _display.drawStr(3, 10, "Hello Info!");
        _display.drawStr(3, 20, buf);
    }

    void drawMode2()
    {
        _display.setFont(u8g2_font_7x14_tr);
        _display.drawStr(30, 24, "Mode 2 Active");
    }

public:
    InfoDisplay(uint8_t sclPin, uint8_t sdaPin)
        : _display(U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C(U8G2_R0, sclPin, sdaPin, U8X8_PIN_NONE))
    {
        log_d("display init");
    }

    void begin(AppState *state)
    {
        _state = state;
        _display.begin();
    }

    void setSelected(bool active)
    {
        _isSelected = active;
    }

    void update()
    {
        _display.clearBuffer();

        // draw selected screen
        switch (_mode)
        {
        case ScreenMode::Battery:
            drawMode1();
            break;
        default:
            drawMode2();
            break;
        }

        if (_isSelected)
            _display.drawFrame(0, 0, 128, 32);

        _display.sendBuffer();
    }

    ScreenMode::ScreenModeType getMode()
    {
        return _mode;
    }

    void setMode(ScreenMode::ScreenModeType mode)
    {
        _mode = mode;
    }
};
