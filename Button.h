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
    button (byte pin) {
        _pin = pin;
        pinMode(_pin, INPUT_PULLUP);
    }
    bool click() {
        bool btnState = digitalRead(_pin);
        if (!btnState && !_flag && millis() - _tmr >= 100) {
            _flag = true;
            _tmr = millis();
            return true;
        }
            if (!btnState && _flag && millis() - _tmr >= 500) {
            _tmr = millis ();
            return true;
        }
        if (btnState && _flag) {
            _flag = false;
            _tmr = millis();
        }
        return false;
    }
private:
    byte _pin;
    uint32_t _tmr;
    bool _flag;
};

#endif
