#ifndef __M5StackButton_H__
#define __M5StackButton_H__

class M5StackButton {
  public:
    void begin(int8_t pin, uint8_t invert, uint32_t dbTime) {
      _pin = pin;
      if (_pin == -1) {
        return;
      }
      if (_pin != 99) {
        pinMode(_pin, INPUT);
      }
      _invert = invert;
      _dbTime = dbTime;
      _state = getPin();
      _time = millis();
      _lastState = _state;
      _changed = 0;
      _hold_time = -1;
      _lastTime = _time;
      _lastChange = _time;
      _pressTime = _time;
    }

    void setAXP192(I2C_AXP192 *axp192) {
      _axp192 = axp192;
    }

    uint8_t getPin(void) {
      if (_pin == -1) {
        return 0;
      } else if (_pin == 99) {
        // AXP192 Pek
        return _axp192->getPekPress() != 0;
      }

      if (_invert) {
        return !digitalRead(_pin);
      }
      return digitalRead(_pin);
    }

    uint8_t read() {
      static uint32_t nowtime;
      static uint8_t pinVal;

      nowtime = millis();
      pinVal = getPin();
      if (nowtime - _lastChange < _dbTime) {
        _lastTime = _time;
        _time = nowtime;
        _changed = 0;
        return _state;
      }
      else {
        _lastTime = _time;
        _time = nowtime;
        _lastState = _state;
        _state = pinVal;
        if (_state != _lastState) {
          _lastChange = nowtime;
          _changed = 1;
          if (_state) {
            _pressTime = _time;
          }
        }
        else {
          _changed = 0;
        }
        return _state;
      }
    }
    uint8_t isPressed() {
      return _state == 0 ? 0 : 1;
    }
    uint8_t isReleased() {
      return _state == 0 ? 1 : 0;
    }
    uint8_t wasPressed() {
      return _state && _changed;
    }
    uint8_t wasReleased() {
      return !_state && _changed && millis() - _pressTime < _hold_time;
    }
    uint8_t pressedFor(uint32_t ms) {
      return (_state == 1 && _time - _lastChange >= ms) ? 1 : 0;
    }
    uint8_t releasedFor(uint32_t ms) {
      return (_state == 0 && _time - _lastChange >= ms) ? 1 : 0;
    }
    uint8_t wasReleasefor(uint32_t ms) {
      _hold_time = ms;
      return !_state && _changed && millis() - _pressTime >= ms;
    }
    uint32_t lastChange() {
      return _lastChange;
    }

  private:
    int8_t _pin;
    uint8_t _puEnable;
    uint8_t _invert;
    uint8_t _state;
    uint8_t _lastState;
    uint8_t _changed;
    uint32_t _time;
    uint32_t _lastTime;
    uint32_t _lastChange;
    uint32_t _dbTime;
    uint32_t _pressTime;
    uint32_t _hold_time;

    I2C_AXP192 *_axp192;
};

#endif
