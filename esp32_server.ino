/******************************************************************
 Created with PROGRAMINO IDE for Arduino - 30.06.2023 23:18:21
 Project     : ESP32_Server
 Libraries   : 
 Author      : Timur Rakhimov
 Description : WiFi server on ESP32
******************************************************************/

#include <WiFi.h>
#include <ESP32WebServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include "EEPROM_RWTypes.h"
#include "Button.h"
#include "ArduinoJson.h"
#include <HTTPClient.h>
#include <UnixTime.h>
#include "MysteriousCrystal_I2C.h"
#include "MyCustomChars.h"
#include "Transliteration.h"

// Для библиотека UnixTime
UnixTime timestamp(3);  // указать GMT (3 для Москвы)
uint32_t unixtime_of_start;

// Название и пароль "мягкой" точки доступа ESP32
const char* ssid_ap = "ESP32";
const char* password_ap = "00000000";
ESP32WebServer server(80);

// Пины светодиодов
#define GREEN_PIN 2                             // Светодиод "Зеленый"
#define RED_PIN 4                               // Светодиод "Красный"
#define YELLOW_PIN 15                           // Светодиод "Желтый"
#define SETTINGS_LED_PIN 21                     // Светодиод "Настройка"

bool settings_mode = false;                     // Флаг режима "Настройка"

// Пины кнопок
#define LEFT_BUT_PIN 17                         // Кнопка "Левая"
#define RIGHT_BUT_PIN 16                        // Кнопка "Правая"
#define SETT_BUT_PIN 23                         // Кнопка "Настройка"
#define BACK_BUT_PIN 5                          // Кнопка "Назад"

// Пины энкодеров
#define ENCODER_PIN 32                          // Энкодер "Крутилка"
uint32_t encoder_timer;

button left_button(LEFT_BUT_PIN, true);         // Экземпляр класса Button для кнопки "Левая"
button right_button(RIGHT_BUT_PIN, true);       // Экземпляр класса Button для кнопки "Правая"
button settings_button(SETT_BUT_PIN);           // Экземпляр класса Button для кнопки "Настройка"
button back_button(BACK_BUT_PIN);               // Экземпляр класса Button для кнопки "Назад"


// = = = = = = = = = = = = = = = I2C = = = = = = = = = = = = = = = //

// пины I2C
#define I2C_SDA 19
#define I2C_SCL 18

// Адреса устройств I2C
#define LCD_I2C_ADDR 0x3F
#define EEPROM_I2C_ADDR 0x50

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //

// = = = = = = = = = = = = = = EEPROM = = = = = = = = = = = = = = //

// Время сохранения текста на EEPROM в секундах
#define EEPROM_SAVE_TIME 10
uint32_t last_eeprom_save_time;
char saved_text[33] {};

// Адрес настроек в памяти EEPROM
#define SETTINGS_ADDRESS 80

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //

// = = = = = = = = = = = = = = ДИСПЛЕЙ = = = = = = = = = = = = = = //

// Номера страниц дисплея в рабочем режиме
#define TEXT_PAGE 0                             // Страница с сохраненным текстом
#define MAIN_PAGE 1                             // Главная страница (для обоих режимов)
#define SSID_PAGE 2                             // Страница с информацией о подключении
#define SETT_PAGE 3                             // Страница перехода в меню "Настройки"

#define SETT_PAGE_1 4
#define SETT_PAGE_2 5
#define SETT_PAGE_3 6

uint8_t ML_Char = 1;                            // Номер пользовательского символа рамки
bool love_mode = false;                         // Флаг Любви


// Номер страниц дисплея в режиме настройки
//#define MAIN_PAGE_S 0                             // Главная страница (для обоих режимов)
//#define MAIN_PAGE_S 0                             // Главная страница (для обоих режимов)

MysteriousCrystal_I2C lcd(LCD_I2C_ADDR, 16, 2, 7);

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = //

void handleRoot(void);
void handlePostText(void);
void handlePostWifiSta(void);
void handlePostTimeFormat(void);
void button_handler(void);
void encoder_handler(void);
void clock_tick(void);
void eeprom_tick(void);

void setup()
{
    // Инициализация пинов свтодиодов в качестве выхода
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
    pinMode(SETTINGS_LED_PIN, OUTPUT);
    pinMode(SETT_BUT_PIN, INPUT_PULLUP);
    pinMode(LEFT_BUT_PIN, INPUT_PULLUP);
    pinMode(RIGHT_BUT_PIN, INPUT_PULLUP);
    pinMode(BACK_BUT_PIN, INPUT_PULLUP);
    pinMode(ENCODER_PIN, ANALOG);
    
    // Включение желтого светодиода на время настройки МК
    digitalWrite(YELLOW_PIN, HIGH);
    
    // Инициализация I2C
    Wire.begin(I2C_SDA, I2C_SCL);
    
    /*
    byte clear_byte = 0;
    eeprom_write(0, 80, &clear_byte, 1);
    eeprom_write(0, 81, &clear_byte, 1);
    eeprom_write(0, 82, &clear_byte, 1);
    */
    
    // Чтение сохраненного значения дисплея из памяти
    eeprom_read(0, 48, (byte*) saved_text, 32);
    lcd.pg_print(TEXT_PAGE, String(saved_text));
    
    lcd.pg_print(SETT_PAGE, "  HACTPOUKU     ");
    lcd.pg_print(SETT_PAGE, "  O CUCTEME     ");
    lcd.pg_print(SETT_PAGE, 0, 0, (byte) 126);
    lcd.pg_print(SETT_PAGE, 0, 1, (byte) 165);
    
    for(int f = 0; f < 3; f++)
    {
        lcd.pg_print(4 + f, "Settings" + String(f));
    }
    lcd.current_page = 1;
    
    // Инициализация LCD
    lcd.init();              
    lcd.backlight(); // Включаем подсветку дисплея
    lcd.createChar(ML_Char, CustomChar_Gachi);
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.write(char(ML_Char));
    lcd.setCursor(13, 0);
    lcd.write(char(ML_Char));
    lcd.setCursor(4, 0);
    lcd.print("Well cum");
    lcd.setCursor(0, 1);
    
    switch(random(0, 5)) 
    {
        case 0:
            lcd.print("Pay me 300 bucks");
            break;
        case 1:
            lcd.print("My Fucking Slave");
            break;
        case 2:
            lcd.print("   to Dungeon   ");
            break;
        case 3:
            lcd.print(" Dungeon Master ");
            break;
        
        default:
            lcd.print("   My Jabroni   ");
            break;
    }
    
    delay(100);
    if (!digitalRead(SETT_BUT_PIN)) 
    {
        digitalWrite(SETTINGS_LED_PIN, HIGH);
        settings_mode = true;
        lcd.pg_print(MAIN_PAGE, 0, 0, "|   Settings   |");
        lcd.pg_print(MAIN_PAGE, 0, 1, "|     mode     |");
        lcd.pg_print(MAIN_PAGE, 0, 0, (byte) ML_Char);
        lcd.pg_print(MAIN_PAGE, 15, 0, (byte) ML_Char);
        lcd.pg_print(MAIN_PAGE, 0, 1, (byte) ML_Char);
        lcd.pg_print(MAIN_PAGE, 15, 1, (byte) ML_Char);
    }
    
    // Инициализация монитора порта
    Serial.begin(115200);
    // Запас времени на открытие монитора порта — 5 секунд (0.2 сек до чтения состояния кнопки, 4.8 после)
    delay(4000);
    lcd.createChar(ML_Char, CustomChar_Monogram); 
    lcd.clear();
    delay(1000);
    
    if (settings_mode) 
    {
        WiFi.mode(WIFI_AP);
        // Создание "мягкой" точки доступа Wi-Fi
        Serial.print("Creating Soft AP ");
        Serial.println(ssid_ap);
        WiFi.softAP(ssid_ap, password_ap);
        delay(100);
        Serial.println("Wi-Fi AP created");

        lcd.pg_print(SSID_PAGE, 0, 0, (String) ssid_ap);
        lcd.pg_print(SSID_PAGE, 0, 1, WiFi.softAPIP().toString());
        
    } 
    else 
    {
        char ssid_wifi_sta[16] {};
        char pass_wifi_sta[16] {};
        eeprom_read(0, 0, (byte*) ssid_wifi_sta, 16);
        eeprom_read(0, 16, (byte*) pass_wifi_sta, 16);
        
        WiFi.mode(WIFI_AP_STA);
        Serial.print("Connecting to ");
        Serial.print(ssid_wifi_sta);
        Serial.print(":::");
        Serial.println(pass_wifi_sta);
        WiFi.begin(ssid_wifi_sta, pass_wifi_sta);
        
        lcd.print("Connecting");
        lcd.setCursor(0, 1);
        lcd.print("to " + (String) ssid_wifi_sta);
        
        uint8_t cursor = 11;
        uint8_t dir = 0;
        
        while (WiFi.status() != WL_CONNECTED) 
        {
        
            if (cursor == 16 & dir == 0) 
            {
                cursor = 11;
                dir = 1;
            } 
            else if (cursor == 15 & dir == 1) 
            {
                dir = 2;
            } 
            else if (cursor == 10 & dir == 2) 
            {
                cursor = 15;
                dir = 3;
            } 
            else if (cursor == 11 & dir == 3) 
            {
                dir = 0;
            } 
            
            if (dir == 0) 
            {
                lcd.setCursor(cursor++, 0);
                lcd.write('.');
            } 
            else if (dir == 1) 
            {
                lcd.setCursor(cursor++, 0);
                lcd.write(' ');
            } 
            else if (dir == 2) 
            {
                lcd.setCursor(cursor--, 0);
                lcd.write('.');
            } 
            else if (dir == 3) 
            {
                lcd.setCursor(cursor--, 0);
                lcd.write(' ');
            }
            
            delay(200);
        }
        
        lcd.clear();
        
        lcd.pg_print(SSID_PAGE, 0, 0, (String) ssid_wifi_sta);
        lcd.pg_print(SSID_PAGE, 0, 1, WiFi.localIP().toString());
      
        
        // Создание "мягкой" точки доступа Wi-Fi
        Serial.print("Creating ");
        Serial.println(ssid_ap);
        WiFi.softAP(ssid_ap, password_ap);
        Serial.println("Wi-Fi created");
        Serial.println("IP-address: "); 
        Serial.println(WiFi.softAPIP());
        
        // создаем DNS-запись для домена esp32.local, связывающую его с IP-адресом
        if (!MDNS.begin("esp32")) 
        {
            Serial.println("Error setting up MDNS responder!");
        }
        
        HTTPClient http;
        // подключаемся к веб-странице
        http.begin("http://worldtimeapi.org/api/timezone/Europe/Moscow");
        delay(1000);
        // делаем GET запрос
        int httpCode = http.GET();
        // проверяем успешность запроса
        if (httpCode > 0) 
        {
            StaticJsonDocument<500> doc;
            DeserializationError error = deserializeJson(doc, (String) http.getString());
            if (error) 
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
            }
            unixtime_of_start = (uint32_t) doc["unixtime"] - (uint32_t) (millis()/1000);
            Serial.println(unixtime_of_start);
            eeprom_write(0, 32, (byte*)&unixtime_of_start, 4);
        }
        else 
        {
            while(1)
            {
                Serial.println("Ошибка HTTP-запроса");
                http.begin("http://worldtimeapi.org/api/timezone/Europe/Moscow");
                delay(1000);
                // делаем GET запрос
                int httpCode = http.GET();
                // проверяем успешность запроса
                if (httpCode > 0) 
                {
                    StaticJsonDocument<500> doc;
                    DeserializationError error = deserializeJson(doc, (String) http.getString());
                    if (error) 
                    {
                        Serial.print(F("deserializeJson() failed: "));
                        Serial.println(error.f_str());
                    }
                    unixtime_of_start = (uint32_t) doc["unixtime"] - (uint32_t) (millis()/1000);
                    Serial.println(unixtime_of_start);
                    eeprom_write(0, 32, (byte*)&unixtime_of_start, 4);
                    break;
                }
                delay(1000);
            }
        }
        // освобождаем ресурсы микроконтроллера
        http.end();
      
    }

    server.on("/", handleRoot);
    server.on("/on_red", []()
    {
        Serial.println("LED RED ON");
        digitalWrite(RED_PIN, HIGH);
        server.send(200, "text/html", "");
    });
    server.on("/off_red", []()
    {
        Serial.println("LED RED OFF");
        digitalWrite(RED_PIN, LOW);
        server.send(200, "text/html", "");
    });
    server.on("/on_green", []()
    {
        Serial.println("LED GREEN ON");
        digitalWrite(GREEN_PIN, HIGH);
        server.send(200, "text/html", "");
    });
    server.on("/off_green", []()
    {
        Serial.println("LED GREEN OFF");
        digitalWrite(GREEN_PIN, LOW);
        server.send(200, "text/html", "");
    });
    server.on("/lcd_on", []()
    {
        Serial.println("LCD ON");
        lcd.backlight();
        server.send(200, "text/html", "");
    });
    server.on("/lcd_off", []()
    {
        Serial.println("LCD OFF");
        lcd.noBacklight();
        server.send(200, "text/html", "");
    });
    server.on("/get_english", []()
    {
        Serial.println("GET ENGLISH");
        server.send(200, "text/html", String(saved_text));
    });
    server.on("/get_russian", []()
    {
        Serial.println("GET RUSSIAN");
        server.send(200, "text/html", 
        R"=="==(<meta name="viewport" content="width=device-width, initial-scale=1" charset="UTF-8">)=="==" +
        english_to_russian(String(saved_text)));
    });
    
    server.on("/text", handlePostText);
    server.on("/wifi_sta", handlePostWifiSta);
    server.on("/time_format", handlePostTimeFormat);
    
    server.begin();
    Serial.println ("HTTP server started");
    
    lcd.pg_update_page();
    digitalWrite(YELLOW_PIN, LOW);
}

void loop()
{
    server.handleClient();
    eeprom_tick();
    button_handler();
    encoder_handler();
    clock_tick();
    
    lcd.pg_update();
}

void handlePostTimeFormat() 
{
    if (server.hasArg("plain")== false) // Check if body received
    {
        server.send(200, "text/plain", "Body not received");
        return;
    }

    String message = "Body received:\n";
    message += server.arg("plain");
    message += "\n";
    
    Serial.println(message);
    
    uint8_t time_format = server.arg("plain").toInt();
    byte format_settings;
    eeprom_read(0, 82, &format_settings, 1);
    format_settings &= ~(B11);
    format_settings = format_settings | (time_format & B11);
    eeprom_write(0, 82, &format_settings, 1);
    
    server.send(200, "text/plain", "");
}

void handlePostText() 
{
    if (server.hasArg("plain")== false) // Check if body received
    {
        server.send(200, "text/plain", "Body not received");
        return;
    }

    String message = "Body received:\n";
    message += server.arg("plain");
    message += "\n";
    
    String to_display = russian_to_english(server.arg("plain"));

    to_display.toCharArray(saved_text, 33);
    lcd.pg_clear(TEXT_PAGE);
    lcd.pg_print(TEXT_PAGE, to_display);

    Serial.println(message);
    Serial.println(sizeof(message));
    Serial.println(to_display);
    
    //for(int i = 0; i < server.arg("plain").length(); i++)
    //{
    //    Serial.println((int) server.arg("plain")[i]);
    //}
    server.send(200, "text/plain", "");
}

void handlePostWifiSta() 
{
    if (server.hasArg("plain") == false) // Check if body received
    {
        server.send(200, "text/plain", "Body not received");
        return;
    }
    
    StaticJsonDocument<96> doc;
    DeserializationError error = deserializeJson(doc, (String) server.arg("plain"));
    if (error) 
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
    }
    const char* ssid_wifi_sta = doc["ssid"];
    const char* pass_wifi_sta = doc["pass"];
    eeprom_write(0, 0, (byte*) ssid_wifi_sta, 16);
    eeprom_write(0, 16, (byte*) pass_wifi_sta, 16);
    Serial.println(ssid_wifi_sta);
    Serial.println(pass_wifi_sta);
    lcd.clear();
    lcd.print(">> " + String(ssid_wifi_sta));
    lcd.setCursor(0, 1);
    lcd.print(">> " + String(pass_wifi_sta));
    delay(5000);
    lcd.pg_update_page();
    
    server.send(200, "text/plain", "");
}

void handleRoot() 
{ 

const String rootPage2(

R"=="==(

<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1" charset="UTF-8">
    <title>ESP32 Server</title>
    <script>
    async function send() {
        let ssid = document.getElementById('ssid_wifi_sta').value;
        let pass = document.getElementById('pass_wifi_sta').value;
        await fetch('wifi_sta', {method: 'POST', body: JSON.stringify({ssid: ssid, pass: pass})});
    }
    async function set_time_format() {
        let time_format = document.getElementById("time_format").value;
        await fetch('time_format', {method: 'POST', body: time_format});
    }
    </script> 
</head>
<body>
<h1>ESP32 - Web Server</h1>
<p>Красный светодиод
    <a>
        <button onclick="fetch('on_red')">ON</button>
    </a>&nbsp;
    <a>
        <button onclick="fetch('off_red')">OFF</button>
    </a>
</p>

<p>Зеленый светодиод
    <a>
        <button onclick="fetch('on_green')">ON</button>
    </a>&nbsp;
    <a>
        <button onclick="fetch('off_green')">OFF</button>
    </a>
</p>

<p>Текст на дисплее
    <input type="text" id="text"
           oninput="fetch('text', {method: 'POST', body: document.getElementById('text').value})">
</p>

<p>LCD Backlight
    <a>
        <button onclick="fetch('lcd_on')">ON</button>
    </a>&nbsp;
    <a>
        <button onclick="fetch('lcd_off')">OFF</button>
    </a>
</p>

<p>
    <label for="ssid_wifi_sta">SSID точки доступа</label>
    <input type="text" id="ssid_wifi_sta">
    <label for="pass_wifi_sta">Пароль точки доступа</label>
    <input type="text" id="pass_wifi_sta">

    <button onclick="send()">
        Сохранить
    </button>
</p>

<p>

    <label for="time_format">Формат отображения времени</label>
    <select id="time_format" name="time_format">
    <option value="0">чч:мм:сс</option>
    <option value="1">чч:мм</option>
    <option value="2">ч:мм:сс</option>
    <option value="3">ч:мм</option>
    </select>
    <button onclick="set_time_format()">Выбрать</button>

</p>

</body>
</html>
    
)=="==");
    
    server.send(200, "text/html", rootPage2);
}

void eeprom_tick() 
{
    if (millis() - last_eeprom_save_time > EEPROM_SAVE_TIME * 1000) 
    {
        digitalWrite(RED_PIN, HIGH);
        // Сохранение значения дисплея в память
        eeprom_write(0, 48, (byte*)saved_text, 32);
        last_eeprom_save_time = millis();
        digitalWrite(RED_PIN, LOW);
    }
}

void clock_tick() 
{
    if (!settings_mode)
    {
        byte format_settings, time_format, date_format;
        eeprom_read(0, 82, &format_settings, 1);
        time_format = format_settings & B11;
        date_format = (format_settings & B11100) >> 2;
        
        timestamp.getDateTime(unixtime_of_start + (uint32_t) (millis() / 1000));
        lcd.pg_clear(MAIN_PAGE);
        
        if (time_format == 0 || time_format == 2)
        {
            lcd.pg_print(MAIN_PAGE, "|   ");
            if (timestamp.hour < 10) 
                lcd.pg_print(MAIN_PAGE, time_format == 0 ? "0" : " ");
            lcd.pg_print(MAIN_PAGE, (String) timestamp.hour + ":");
            if (timestamp.minute < 10) 
                lcd.pg_print(MAIN_PAGE, "0");
            lcd.pg_print(MAIN_PAGE, (String) timestamp.minute + ":");
            if (timestamp.second < 10) 
                lcd.pg_print(MAIN_PAGE, "0");
            lcd.pg_print(MAIN_PAGE, (String) timestamp.second + "   |");
        } else if (time_format == 1 || time_format == 3) {
            lcd.pg_print(MAIN_PAGE, "|     ");
            if (timestamp.hour < 10) 
                lcd.pg_print(MAIN_PAGE, time_format == 1 ? "0" : " ");
                //if (time_format == 3)
                //    lcd.pg_print(MAIN_PAGE, " ");
                //else
                //    lcd.pg_print(MAIN_PAGE, "0");
            lcd.pg_print(MAIN_PAGE, (String) timestamp.hour + ":");
            if (timestamp.minute < 10) 
                lcd.pg_print(MAIN_PAGE, "0");
            lcd.pg_print(MAIN_PAGE, (String) timestamp.minute + "    |");
        }
        
        /*lcd.pg_print(MAIN_PAGE, "|   ");
        if (timestamp.hour < 10) 
            lcd.pg_print(MAIN_PAGE, "0");
        lcd.pg_print(MAIN_PAGE, (String) timestamp.hour + ":");
        if (timestamp.minute < 10) 
            lcd.pg_print(MAIN_PAGE, "0");
        lcd.pg_print(MAIN_PAGE, (String) timestamp.minute + ":");
        if (timestamp.second < 10) 
            lcd.pg_print(MAIN_PAGE, "0");
        lcd.pg_print(MAIN_PAGE, (String) timestamp.second + "   |");*/
        
        
        lcd.pg_setCursor(MAIN_PAGE, 0, 1);
        lcd.pg_print(MAIN_PAGE, "|  ");
        if (timestamp.day < 10) 
            lcd.pg_print(MAIN_PAGE, "0");
        lcd.pg_print(MAIN_PAGE, (String) timestamp.day + ".");
        if (timestamp.month < 10) 
            lcd.pg_print(MAIN_PAGE, "0");
        lcd.pg_print(MAIN_PAGE, (String) timestamp.month + "." + (String) timestamp.year + "  |");
        
        lcd.pg_print(MAIN_PAGE, 0, 0, (byte) ML_Char);
        lcd.pg_print(MAIN_PAGE, 15, 0, (byte) ML_Char);
        lcd.pg_print(MAIN_PAGE, 0, 1, (byte) ML_Char);
        lcd.pg_print(MAIN_PAGE, 15, 1, (byte) ML_Char);
        
        //lcd.pg_print(MAIN_PAGE, 1, 0, String(time_format));
        //lcd.pg_print(MAIN_PAGE, 1, 1, String(date_format));
    }
}

bool encoder_mode = false;
uint8_t additional_menu_pos = 0;
uint8_t settings_menu_pos = 0;

void button_handler() {
    
    // Функция обрабатывает нажатия клавиш для разных страницах
    // Страницы настроек будут располагаться после 2-ой или 3-ей
    
    if (lcd.current_page >= 0 && lcd.current_page <= 2)
    {
        if (left_button.click()) 
        {
            Serial.println("Left Button!");
            if (lcd.current_page != 0)
                lcd.current_page--;
            Serial.println(lcd.current_page);
            Serial.println(lcd.pg_get_current_text());
        }
        if (right_button.click()) 
        {
            Serial.println("Right Button!");
            if (lcd.current_page != 3)
                lcd.current_page++;
            Serial.println(lcd.current_page);
            Serial.println(lcd.pg_get_current_text());
        }
        if (settings_button.click()) 
        {
            if (lcd.current_page == MAIN_PAGE)
                love_mode = !love_mode;
        }
        if(back_button.click()) 
            lcd.current_page = MAIN_PAGE;
    }
    else if (lcd.current_page == SETT_PAGE)  // На данный момент 3
    {
        if (left_button.click()) 
        {
            if (additional_menu_pos == 0)
                lcd.current_page--;
            else
                additional_menu_pos--;
        }
        if (right_button.click()) 
        {
            if (additional_menu_pos != 1)
                additional_menu_pos++;
        }
        /*if (settings_button.click()) 
        {
            lcd.current_page++;
        }*/
        if(back_button.click()) {
            lcd.current_page = MAIN_PAGE;
            additional_menu_pos = 0;
        }
        
        lcd.pg_print(SETT_PAGE, 0, 0, (byte) 165);
        lcd.pg_print(SETT_PAGE, 0, 1, (byte) 165);
        
        if (additional_menu_pos == 0)
            lcd.pg_print(SETT_PAGE, 0, 0, (byte) 126);
        else if (additional_menu_pos == 1)
            lcd.pg_print(SETT_PAGE, 0, 1, (byte) 126);
    }
    else if (lcd.current_page == -1)
    {
        settings_menu_pos = 0;
    }
    else if (lcd.current_page >= 4 && lcd.current_page <= 6)
    {
        if (left_button.click()) 
        {
            Serial.println("Left Button!");
            if (lcd.current_page != 4)
                lcd.current_page--;
            Serial.println(lcd.current_page);
            Serial.println(lcd.pg_get_current_text());
        }
        if (right_button.click()) 
        {
            Serial.println("Right Button!");
            if (lcd.current_page != 6)
                lcd.current_page++;
            Serial.println(lcd.current_page);
            Serial.println(lcd.pg_get_current_text());
        }
        if (settings_button.click()) 
        { 
            digitalWrite(GREEN_PIN, HIGH);
            delay(50);
            digitalWrite(GREEN_PIN, LOW);
        }
        if (back_button.click()) 
            lcd.current_page = 3;
    }
    
    if (love_mode)
        lcd.createChar(ML_Char, CustomChar_Love);
    else
        lcd.createChar(ML_Char, CustomChar_Monogram); 
        
    lcd.pg_update_page();
}

void encoder_handler() {
    if (millis() - encoder_timer > 300) {
        int enc_value = analogRead(ENCODER_PIN);
        
        if (encoder_mode && settings_mode) {
            /*for(byte j = 0; j < 4 ; j++)
                lcd_text[0][1][j] = ' ';
            
            if (enc_value / 1000 % 10)
                lcd_text[0][1][0] = enc_value / 1000;
            
            if (enc_value / 100 % 10)
                lcd_text[0][1][1] = enc_value / 100 % 10;
            
            if (enc_value / 10 % 10)
                lcd_text[0][1][2] = enc_value / 10 % 10;
            
            if (enc_value % 10)
                lcd_text[0][1][3] = enc_value % 10;*/
                
        } /*else {
            if (enc_value <= 1000)
                current_lcd_page = 0;
            else if (enc_value <= 2000)
                current_lcd_page = 1;
            else if (enc_value <= 3000)
                current_lcd_page = 2;

        }*/
        
        encoder_timer = millis();
    }
}
