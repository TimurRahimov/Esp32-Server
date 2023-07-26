/******************************************************************
 Created with PROGRAMINO IDE for Arduino - 22.07.2023 17:13:55
 Libraries   :
 Author      :
 Description :
******************************************************************/

// библиотека для работы с дисплеем
#include <LiquidCrystal_I2C.h>

#ifndef MysteriousCrystal_I2C_H
#define MysteriousCrystal_I2C_H

class MysteriousCrystal_I2C : public LiquidCrystal_I2C
{
public:
    MysteriousCrystal_I2C(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows);
    MysteriousCrystal_I2C(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t lcd_pages);
    
    void pg_clear(uint8_t page);
    void pg_print(uint8_t page, uint8_t col, uint8_t row, String text);
    void pg_print(uint8_t page, uint8_t col, uint8_t row, char text);
    void pg_print(uint8_t page, uint8_t col, uint8_t row, byte text);
    void pg_print(uint8_t page, String text);
    void pg_update(bool all = 0);
    void pg_update_page();
    String pg_get_current_text();
    uint8_t current_page;
    
private:
    String *_text_on_lcd;
    String *_text_to_lcd;
    uint8_t *_cursor_to_lcd;
};

#endif
