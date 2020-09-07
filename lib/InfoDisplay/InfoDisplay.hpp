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
    bool _isActive = false;

public:
    InfoDisplay(uint8_t sclPin, uint8_t sdaPin)
        : _display(U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C(U8G2_R0, sclPin, sdaPin, U8X8_PIN_NONE))
    {
        log_d("display init");
    }

    void begin()
    {
        _display.begin();
    }

    void setActive(bool active)
    {
        _isActive = active;
    }

    void update(AppState *state)
    {
        _display.clearBuffer();

        _display.setFont(u8g2_font_6x10_tr);

        char buf[10] = "";
        sprintf(buf, "%d", state->counter);

        _display.drawStr(0, 10, "Hello Info!");
        _display.drawStr(0, 20, buf);

        if (_isActive)
            _display.drawFrame(0, 0, 128, 32);

        _display.sendBuffer();
    }

    void setMode(ScreenMode::ScreenModeType mode)
    {
        _mode = mode;
    }
};
