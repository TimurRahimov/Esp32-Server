/******************************************************************
 Created with PROGRAMINO IDE for Arduino - 07.07.2023 21:35:16
 Libraries   :
 Author      :
 Description :
******************************************************************/


#ifndef EEPROM_RWTypes_H
#define EEPROM_RWTypes_H


void eeprom_write(byte page, byte address, byte* value, byte text_size);
void eeprom_read(byte page, byte address, byte* value, byte text_size);



#endif
