/******************************************************************
 Created with PROGRAMINO IDE for Arduino - 09.07.2023 18:31:01
 Libraries   :
 Author      :
 Description :
******************************************************************/


#ifndef Button_H
#define Button_H

class button {
public:
    button (byte pin, bool impuls = true) {
        _pin = pin;
        _impuls = impuls;
        pinMode(_pin, INPUT_PULLUP);
    }
    bool click() {
        bool btnState = !digitalRead(_pin);
        if (btnState) {
            if (!_flag) {
                if(millis() - _tmr >= 100) {
                    _flag = true;
                    _tmr = millis();
                    return true;
                }
            } else {
                if(_impuls) {
                    if (millis() - _tmr >= 500) {
                        _tmr = millis();
                        return true;
                    }
                } else {
                    return true;
                }
            }
        } else {
            if (_flag) {
                _flag = false;
                _tmr = millis();
            }
        }
        return false;
    }
private:
    byte _pin;
    uint32_t _tmr;
    bool _flag;
    bool _impuls;
};

#endif
