/******************************************************************
 Created with PROGRAMINO IDE for Arduino - 22.07.2023 16:42:23
 Project     :
 Libraries   :
 Author      :
 Description :
******************************************************************/

#include <Arduino.h>
#include "MysteriousCrystal_I2C.h"

MysteriousCrystal_I2C::MysteriousCrystal_I2C(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t lcd_pages) 
    : LiquidCrystal_I2C(lcd_Addr, lcd_cols, lcd_rows), current_page{ 0 } 
{
    _text_on_lcd = new String[lcd_pages];
    _text_to_lcd = new String[lcd_pages];
    _cursor_to_lcd = new uint8_t[lcd_pages];
    for(uint8_t i = 0; i < lcd_pages; i++)
    {
        _text_on_lcd[i].reserve(32);
        _text_on_lcd[i] = "                                 ";
        _text_to_lcd[i].reserve(32);
        _text_to_lcd[i] = "                                 ";
        _cursor_to_lcd[i] = 0;
    }
}

MysteriousCrystal_I2C::MysteriousCrystal_I2C(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows)
    : LiquidCrystal_I2C(lcd_Addr, lcd_cols, lcd_rows) 
{
    MysteriousCrystal_I2C(lcd_Addr, lcd_cols, lcd_rows, 1);
}


void MysteriousCrystal_I2C::pg_clear(uint8_t page)
{
    _text_to_lcd[page] = "                                 ";
    _cursor_to_lcd[page] = 0;
}

void MysteriousCrystal_I2C::pg_update(bool all) 
{

    for(uint8_t i = 0; i < 32; i++)
    {
        if (_text_on_lcd[current_page][i] != _text_to_lcd[current_page][i] | all) {
            this->setCursor(i % 16, i / 16);
            if ((byte) _text_to_lcd[current_page][i] > 0 && (byte) _text_to_lcd[current_page][i] < 9)
                this->write(byte(_text_to_lcd[current_page][i] - 1));
            else
                this->print(_text_to_lcd[current_page][i]);
            _text_on_lcd[current_page][i] = _text_to_lcd[current_page][i];
        }
    }
}

void MysteriousCrystal_I2C::pg_update_page()
{
    this->pg_update(1);
}

void MysteriousCrystal_I2C::pg_print(uint8_t page, uint8_t col, uint8_t row, String text) 
{    
    text = text.substring(0, 32);
    uint8_t i = col + 16 * row;
    uint8_t print_size = text.length() + i <= 32 ? text.length() + i : 32;
    for(uint8_t j = 0; i < print_size; i++)
    {
        if (text[j] != 0)
            _text_to_lcd[page][i] = text[j++];
    }
}

void MysteriousCrystal_I2C::pg_print(uint8_t page, uint8_t col, uint8_t row, char text) 
{    
    _text_to_lcd[page][col + 16 * row] = text;
}

void MysteriousCrystal_I2C::pg_print(uint8_t page, uint8_t col, uint8_t row, byte text) 
{   
    _text_to_lcd[page][col + 16 * row] = char(text + 1);
}

void MysteriousCrystal_I2C::pg_print(uint8_t page, String text) 
{
    text = text.substring(0, 32);
    
    uint8_t print_size = text.length() + _cursor_to_lcd[page] <= 32 ? text.length() + _cursor_to_lcd[page] : 32;
    for(uint8_t i = 0; _cursor_to_lcd[page] < print_size; _cursor_to_lcd[page]++)
    {
        if (text[i] != 0)
            _text_to_lcd[page][_cursor_to_lcd[page]] = text[i++];
    }
}

String MysteriousCrystal_I2C::pg_get_current_text()
{
    return (String) _text_to_lcd[current_page];
}