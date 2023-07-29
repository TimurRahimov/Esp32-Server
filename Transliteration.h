/******************************************************************
 Created with PROGRAMINO IDE for Arduino - 28.07.2023 14:32:25
 Libraries   :
 Author      :
 Description :
******************************************************************/

#include <Arduino.h>

#ifndef Transliteration_H
#define Transliteration_H


String russian_to_english(String russian){
    String result{}; 
    for(int i = 0; i < russian.length(); i++)
    {
        if (russian[i] == 208) {
            i++;
            switch(russian[i]) {
                case 144: // А 208
                result += "A";
                break;
                case 145: // Б 208
                result += "B";
                break;
                case 146: // В 208
                result += "V";
                break;
                case 147: // Г 208
                result += "G";
                break;
                case 148: // Д 208
                result += "D";
                break;
                case 149: // Е 208
                result += "E";
                break;
                case 129: // Ё 208
                result += "YO";
                break;
                case 150: // Ж 208
                result += "ZH";
                break;
                case 151: // З 208
                result += "Z";
                break;
                case 152: // И 208
                result += "I";
                break;
                case 153: // Ё 208
                result += "J";
                break;
                case 154: // К 208
                result += "K";
                break;
                case 155: // Л 208
                result += "L";
                break;
                case 156: // М 208
                result += "M";
                break;
                case 157: // Н 208
                result += "N";
                break;
                case 158: // О 208
                result += "O";
                break;
                case 159: // П 208
                result += "P";
                break;
                case 160: // Р 208
                result += "R";
                break;
                case 161: // С 208
                result += "S";
                break;
                case 162: // Т 208
                result += "T";
                break;
                case 163: // У 208
                result += "U";
                break;
                case 164: // Ф 208
                result += "F";
                break;
                case 165: // Х 208
                result += "KH";
                break;
                case 166: // Ц 208
                result += "C";
                break;
                case 167: // Ч 208
                result += "CH";
                break;
                case 168: // Ш 208
                result += "SH";
                break;
                case 169: // Щ 208
                result += "SHH";
                break;
                case 170: // Ъ 208
                result += "``";
                break;
                case 171: // Ы 208
                result += "y`";
                break;
                case 172: // Ь 208
                result += "`";
                break;
                case 173: // Э 208
                result += "E`";
                break;
                case 174: // Ю 208
                result += "YU";
                break;
                case 175: // Я 208
                result += "YA";
                break;
                case 176: // а 208
                result += "a";
                break;
                case 177: // б 208
                result += "b";
                break;
                case 178: // в 208
                result += "v";
                break;
                case 179: // г 208
                result += "g";
                break;
                case 180: // д 208
                result += "d";
                break;
                case 181: // е 208
                result += "e";
                break;
                case 182: // ж 208
                result += "zh";
                break;
                case 183: // з 208
                result += "z";
                break;
                case 184: // и 208
                result += "i";
                break;
                case 185: // й 208
                result += "j";
                break;
                case 186: // к 208
                result += "k";
                break;
                case 187: // л 208
                result += "l";
                break;
                case 188: // м 208
                result += "m";
                break;
                case 189: // н 208
                result += "n";
                break;
                case 190: // о 208
                result += "o";
                break;
                case 191: // п 208
                result += "p";
                break;
                
                default:
                result += russian[i];
                break;
            }
        } else if (russian[i] == 209) {
            i++;
            switch(russian[i]) {
                case 128: // р 209
                result += "r";
                break;
                case 129: // с 209
                result += "s";
                break;
                case 130: // т 209
                result += "t";
                break;
                case 131: // у 209
                result += "u";
                break;
                case 132: // ф 209
                result += "f";
                break;
                case 133: // х 209
                result += "kh";
                break;
                case 134: // ц 209
                result += "c";
                break;
                case 135: // ч 209
                result += "ch";
                break;
                case 136: // ш 209
                result += "sh";
                break;
                case 137: // щ 209
                result += "shh";
                break;
                case 138: // ъ 209
                result += "``";
                break;
                case 139: // ы 209
                result += "y`";
                break;
                case 140: // ь 209
                result += "`";
                break;
                case 141: // э 209
                result += "e`";
                break;
                case 142: // ю 209
                result += "yu";
                break;
                case 143: // я 209
                result += "ya";
                break;
                case 145: // ё 209
                result += "yo";
                break;
                default:
                result += russian[i];
                break;
            }
        } else {
            result += russian[i];
        }
    }
    return result;
}

String english_to_russian(String english){
    String result{}; 
    for(int i = 0; i < english.length(); i++)
    {
        switch(english[i]) {
            case 'a':
            result += "а";
            break;
            
            case 'b':
            result += "б";
            break;
            
            case 'v':
            result += "в";
            break;
            
            case 'g':
            result += "г";
            break;
            
            case 'd':
            result += "д";
            break;
            
            case 'e':
            if (english[i + 1] == '`')
            {
                result += "э";
                i++;
            }
            else
                result += "е";
            break;
            
            case 'y':
            if (english[i + 1] == 'o')
            {
                result += "ё";
                i++;
            }
            else if (english[i + 1] == '`')
            {
                result += "ы";
                i++;
            }
            else if (english[i + 1] == 'u')
            {
                result += "ю";
                i++;
            }
            else if (english[i + 1] == 'a')
            {
                result += "я";
                i++;
            } else {
                result += english[i];
            }
            break;
            
            case 'z':
            if (english[i + 1] == 'h')
            {
                result += "ж";
                i++;
            }
            else
                result += "з";
            break;
            
            case 'i':
            result += "и";
            break;
            
            case 'j':
            result += "й";
            break;
            
            case 'k':
            if (english[i + 1] == 'h')
            {
                result += "х";
                i++;
            }
            else
                result += "к";
            break;
            
            case 'l':
            result += "л";
            break;
        
            case 'm':
            result += "м";
            break;
            
            case 'n':
            result += "н";
            break;
            
            case 'o':
            result += "о";
            break;
            
            case 'p':
            result += "п";
            break;
            
            case 'r':
            result += "р";
            break;
            
            case 's':
            if (english[i + 1] == 'h')
            {
                if (english[i + 2] == 'h')
                {
                    result += "щ";
                    i++;
                } else {
                    result += "ш";
                }
                i++;
            }
            else
                result += "с";
            break;
            
            case 't':
            result += "т";
            break;
            
            case 'u':
            result += "у";
            break;
            
            case 'f':
            result += "ф";
            break;
            
            case 'с':
            if (english[i + 1] == 'h')
            {
                result += "ч";
                i++;
            }
            else
                result += "ц";
            break;    
        
            case '`':
            if (english[i + 1] == '`')
            {
                result += "ъ";
                i++;
            }
            else
                result += "ь";
            break;      
            
            case 'A':
            result += "А";
            break;
            
            case 'B':
            result += "Б";
            break;
            
            case 'V':
            result += "В";
            break;
            
            case 'G':
            result += "Г";
            break;
            
            case 'D':
            result += "Д";
            break;
            
            case 'E':
            if (english[i + 1] == '`')
            {
                result += "Э";
                i++;
            }
            else
                result += "У";
            break;
            
            case 'Y':
            if (english[i + 1] == 'O')
            {
                result += "Ё";
                i++;
            }
            else if (english[i + 1] == '`')
            {
                result += "Ы";
                i++;
            }
            else if (english[i + 1] == 'U')
            {
                result += "Ю";
                i++;
            }
            else if (english[i + 1] == 'A')
            {
                result += "Я";
                i++;
            } else {
                result += english[i];
            }
            break;
            
            case 'Z':
            if (english[i + 1] == 'H')
            {
                result += "Ж";
                i++;
            }
            else
                result += "З";
            break;
            
            case 'I':
            result += "И";
            break;
            
            case 'J':
            result += "Й";
            break;
            
            case 'K':
            if (english[i + 1] == 'H')
            {
                result += "Х";
                i++;
            }
            else
                result += "К";
            break;
            
            case 'L':
            result += "Л";
            break;
        
            case 'M':
            result += "М";
            break;
            
            case 'N':
            result += "Н";
            break;
            
            case 'O':
            result += "О";
            break;
            
            case 'P':
            result += "П";
            break;
            
            case 'R':
            result += "Р";
            break;
            
            case 'S':
            if (english[i + 1] == 'H')
            {
                if (english[i + 2] == 'H')
                {
                    result += "Щ";
                    i++;
                } else {
                    result += "Ш";
                }
                i++;
            }
            else
                result += "С";
            break;
            
            case 'T':
            result += "Т";
            break;
            
            case 'U':
            result += "У";
            break;
            
            case 'F':
            result += "Ф";
            break;
            
            case 'C':
            if (english[i + 1] == 'H')
            {
                result += "Ч";
                i++;
            }
            else
                result += "Ц";
            break;    
            
        
            default:
            result += english[i];
            break;
        }
    }
    return result;
}


#endif
