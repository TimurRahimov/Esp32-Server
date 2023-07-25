/******************************************************************
 Created with PROGRAMINO IDE for Arduino - 07.07.2023 21:32:31
 Project     :
 Libraries   :
 Author      :
 Description :
******************************************************************/

#include <Arduino.h>
#include <Wire.h>

#ifndef EEPROM_I2C_ADDR
#define EEPROM_I2C_ADDR 0x50
#endif

void eeprom_write(byte page, byte address, byte* value, byte text_size) {
    byte transmission_address = EEPROM_I2C_ADDR | (page << 1);
    
    for(byte i = 0; i < text_size; i += 16)
    {
        byte tail = text_size - i;
        if (tail > 16)
            tail = 16;
        Wire.beginTransmission(transmission_address);
        Wire.write(address + i);
        Wire.write(value + i, tail);
        Wire.endTransmission();
        delay(5);
    }
}

void eeprom_read(byte page, byte address, byte* value, byte text_size) {
    byte transmission_address = EEPROM_I2C_ADDR | (page << 1);
    
    for(byte i = 0; i < text_size; i += 16)
    {
        byte tail = text_size - i;
        if (tail > 16)
            tail = 16;
        Wire.beginTransmission(transmission_address);
        Wire.write(address + i);
        Wire.endTransmission();
        delay(5);
        Wire.requestFrom(transmission_address, tail);
        for (byte rdata = 0; Wire.available() && rdata < tail; rdata++) {
            value[i + rdata] = (char) Wire.read();
        }
    }
}