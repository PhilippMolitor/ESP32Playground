#include <Arduino.h>
#include <FunctionalInterrupt.h>

void IRAM_ATTR isrRotation(void *arg);
void IRAM_ATTR isrButton(void *arg);

struct EC11Event
{
    enum Rotation
    {
        None = 0,
        CW,
        CCW
    };

    Rotation rotation;
    uint8_t amount;
    bool pressed;

    EC11Event() : rotation(None), amount(0), pressed(false) {}
};

class EC11
{
private:
    // constants
    const ulong _btnDebounceDelay = 50;

    // state
    uint8_t _rotationStateBuf;
    ulong _btnDebounceTime;
    bool _btnState;
    EC11Event _event;

    // config
    uint8_t _pinA;
    uint8_t _pinB;
    uint8_t _pinBtn;

    void addRotation(EC11Event::Rotation rot)
    {

        if (_event.rotation == rot)
        {
            if (_event.amount != 0xFF)
            {
                _event.amount++;
            }
        }
        else
        {
            _event.rotation = rot;
            _event.amount = 1;
        }
    }

public:
    EC11(uint8_t pinA, uint8_t pinB, uint8_t pinBtn)
        : _rotationStateBuf(0), _btnDebounceTime(0), _btnState(false), _pinA(pinA), _pinB(pinB), _pinBtn(pinBtn) {}

    void begin()
    {
        pinMode(_pinA, INPUT_PULLUP);
        pinMode(_pinB, INPUT_PULLUP);
        pinMode(_pinBtn, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(_pinA), std::bind(isrRotation, this), CHANGE);
        attachInterrupt(digitalPinToInterrupt(_pinB), std::bind(isrRotation, this), CHANGE);
        attachInterrupt(digitalPinToInterrupt(_pinBtn), std::bind(isrButton, this), CHANGE);
    }

    void handleRotationEvent()
    {
        uint8_t state = (digitalRead(_pinB) << 1) | digitalRead(_pinA);

        if (state != (_rotationStateBuf & 0x3))
        {
            _rotationStateBuf = (_rotationStateBuf << 2) | state;

            if (_rotationStateBuf == 0b10000111)
                addRotation(EC11Event::CCW);
            else if (_rotationStateBuf == 0b01001011)
                addRotation(EC11Event::CW);
        }
    }

    void handleButtonEvent()
    {
        const uint8_t state = digitalRead(_pinBtn);

        if (state == LOW && !_btnState && (millis() - _btnDebounceTime) > _btnDebounceDelay)
        {
            _btnDebounceTime = millis();
            _btnState = true;
            _event.pressed = true;
        }
        else if (state == HIGH)
        {
            _btnState = false;
        }
    }

    void reset()
    {
        // _rotationStateBuf = 0;
        // _btnStateBuf = false;
        _event = EC11Event();
    }

    bool getEvents(EC11Event *e)
    {
        noInterrupts();

        if (_event.amount == 0 && !_event.pressed)
        {
            interrupts();
            return false;
        }

        // set referenced event object to current state
        *e = _event;

        // reset internal state
        reset();

        interrupts();
        return true;
    }
};

void IRAM_ATTR isrRotation(void *arg)
{
    EC11 *ec = static_cast<EC11 *>(arg);
    ec->handleRotationEvent();
};

void IRAM_ATTR isrButton(void *arg)
{
    EC11 *ec = static_cast<EC11 *>(arg);
    ec->handleButtonEvent();
};
