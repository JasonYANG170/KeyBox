/*
    This example opens Sqlite3 databases from SD Card and
    retrieves data from them.
    Before running please copy following files to SD Card:
    examples/sqlite3_sdmmc/data/mdr512.db
    examples/sqlite3_sdmmc/data/census2000names.db
    Connections:
     * SD Card | ESP32
     *  DAT2       -
     *  DAT3       SS (D5)
     *  CMD        MOSI (D23)
     *  VSS        GND
     *  VDD        3.3V
     *  CLK        SCK (D18)
     *  DAT0       MISO (D19)
     *  DAT1       -
*/
#include <stdio.h>
#include <stdlib.h>

#include <SPI.h>
#include <FS.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sstream>
#include <vector>
#include "SD.h"
#include <ArduinoJson.h>
//配置数据结构
using namespace std;
int OutPutTimes;
int modelchooese=0;//0:JSON,1:TXT,2:SET
String OutPutString = "";
int keySize;
char* passwordIn;
struct key {
    String user;
    String password;
};
int passwordst=0;
const char* data = "Callback function called";
//#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// #define ESP32 0
// #define MEGA2560 1

// #define MCU 1

#define SPEED 4//16的因数
#define ICON_SPEED 12
#define ICON_SPACE 48//图标间隔，speed倍数

//  #define READ A0
//  #define BTN0 5
//  #define BTN1 6
//  #define BTN2 7

#define BTN0 12
#define BTN1 1
#define BTN2 0

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ 9, /* data=*/ 8);   // ESP32 Thing, pure SW emulated I2C

PROGMEM const uint8_t icon_pic[][200]
        {
                {
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,
                        0x00,0x0F,0x00,0x00,0x00,0x80,0x1F,0x00,0x00,0x00,0xC0,0x3F,0x00,0x00,0x00,0xC0,
                        0x3F,0x00,0x00,0x00,0xE0,0x7F,0x00,0x00,0x00,0xF0,0xFF,0x00,0x00,0x00,0xF8,0xFF,
                        0x01,0x00,0x00,0xFC,0xFF,0x03,0x00,0x00,0xFC,0xFF,0x03,0x00,0x00,0xFE,0xFF,0x07,
                        0x00,0x00,0xFF,0xFF,0x0F,0x00,0x80,0xFF,0xFF,0x1F,0x00,0x80,0xFF,0xFF,0x1F,0x00,
                        0x00,0x00,0x00,0x00,0x00,0x80,0xFF,0xFF,0x1F,0x00,0x80,0xFF,0xFF,0x1F,0x00,0x80,
                        0xFF,0xFF,0x1F,0x00,0x80,0xFF,0xFF,0x1F,0x00,0x80,0xFF,0xFF,0x1F,0x00,0x80,0x1F,
                        0x80,0x1F,0x00,0x80,0x1F,0x80,0x1F,0x00,0x80,0x1F,0x80,0x1F,0x00,0x80,0x1F,0x80,
                        0x1F,0x00,0x80,0x1F,0xB0,0x1F,0x00,0x80,0x1F,0xB0,0x1F,0x00,0x80,0x1F,0x80,0x1F,
                        0x00,0x80,0x1F,0x80,0x1F,0x00,0x80,0x1F,0x80,0x1F,0x00,0x80,0x1F,0x80,0x1F,0x00,
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x00,/*"C:\Users\qw200\Desktop\home.bmp",0*/
                },
                {
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,
                        0x00,0x0F,0x00,0x00,0x00,0x00,0xFF,0x3F,0x00,0x00,0x00,0xFF,0x3F,0x00,0x00,0x00,
                        0xFF,0x3F,0x00,0x00,0x00,0xFF,0x3F,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x0F,
                        0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x0F,0x00,
                        0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0xE0,0x7F,0x00,0x00,0x00,0xF8,0xFF,0x01,0x00,
                        0x00,0xFC,0xFF,0x03,0x00,0x00,0xFE,0xFF,0x07,0x00,0x00,0xFF,0xFF,0x0F,0x00,0x00,
                        0xFF,0xFF,0x0F,0x00,0x80,0xFF,0xFF,0x1F,0x00,0x80,0x7F,0xE0,0x1F,0x00,0x80,0x3F,
                        0xC0,0x1F,0x00,0x80,0x3F,0xC0,0x1F,0x00,0x80,0x3F,0xC0,0x1F,0x00,0x80,0x3F,0xC0,
                        0x1F,0x00,0x80,0x3F,0xC0,0x1F,0x00,0x80,0x7F,0xE0,0x1F,0x00,0x80,0xFF,0xFF,0x1F,
                        0x00,0x00,0xFF,0xFF,0x0F,0x00,0x00,0xFF,0xFF,0x0F,0x00,0x00,0xFE,0xFF,0x07,0x00,
                        0x00,0xFC,0xFF,0x03,0x00,0x00,0xF8,0xFF,0x01,0x00,0x00,0xE0,0x7F,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x00,/*"C:\Users\qw200\Desktop\key.bmp",0*/

                        /* (36 X 37 )*/
                },
                {
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x00,0xE0,0xFF,0xFF,0xFF,0x00,0xE0,0xFF,0xFF,0xFF,0x00,0xE0,0xFF,
                        0xFF,0xFF,0x00,0xE0,0xFF,0xFF,0xFF,0x00,0xE0,0xFF,0xFF,0xFF,0x00,0xE0,0xFF,0xFF,
                        0xFF,0x00,0xE0,0xFF,0xFF,0xFF,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,
                        0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,
                        0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,
                        0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,
                        0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,
                        0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,
                        0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,0x00,0x80,0x3F,0x00,0x00,
                        0x00,0x80,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x00,/*"C:\Users\qw200\Desktop\home.bmp",0*/

                },
                {
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                                                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                        0x00,0x0C,0x00,0x00,0x00,0x00,0x1C,0x00,
                                                                0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x00,
                        0xFC,0x00,0x00,0x00,0x00,0xFC,0x01,0x00,
                                                                0x00,0x00,0xFC,0x03,0x00,0x00,0x00,0xFC,
                        0x07,0x00,0x00,0x00,0xFC,0x0F,0x00,0x00,
                                                                0x00,0xFE,0x1F,0x00,0x00,0xF8,0xFF,0x3F,
                        0x00,0x00,0xFF,0xFF,0xFF,0x00,0xC0,0xFF,
                                                                0xFF,0xFF,0x01,0xE0,0xFF,0xFF,0xFF,0x03,
                        0xF0,0xFF,0xFF,0xFF,0x07,0xF0,0xFF,0xFF,
                                                                0xFF,0x0F,0xF8,0xFF,0xFF,0xFF,0x0F,0xFC,
                        0xFF,0xFF,0xFF,0x07,0xFC,0xFF,0xFF,0xFF,
                                                                0x03,0xFE,0xFF,0xFF,0xFF,0x01,0xFE,0xFF,
                        0xFF,0xFF,0x00,0xFF,0x03,0xFE,0x3F,0x00,
                                                                0xFF,0x00,0xFC,0x1F,0x00,0x3F,0x00,0xFC,
                        0x0F,0x00,0x1F,0x00,0xFC,0x07,0x00,0x07,
                                                                0x00,0xFC,0x03,0x00,0x03,0x00,0xFC,0x01,
                        0x00,0x01,0x00,0xFC,0x00,0x00,0x00,0x00,
                                                                0x3C,0x00,0x00,0x00,0x00,0x1C,0x00,0x00,
                        0x00,0x00,0x0C,0x00,0x00,0x00,0x00,0x00,
                                                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x00,/*"C:\Users\13944\Desktop\fenxiang.bmp",0*/
                },
        };

uint8_t icon_width[] = { 36,36,36,36 };

//main界面图片
PROGMEM const uint8_t LOGO[] =
        {
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0xEF,0x0F,0x70,0x00,0x00,0x00,0x00,0x00,0xE0,0xD8,0x87,0x3F,
                0x00,0x00,0x00,0x80,0xEF,0x0F,0x00,0x07,0x00,0x00,0x00,0x00,0xE0,0xDC,0x8F,0x3F,
                0x98,0xFF,0x7F,0xBF,0x0F,0x8F,0xFF,0xFF,0x03,0x00,0x00,0x00,0xE0,0x1D,0x8F,0x07,
                0x98,0xE1,0xFF,0x3F,0x87,0x87,0x73,0x77,0x03,0x00,0x00,0x00,0xC0,0x9F,0x87,0x1F,
                0x98,0xFD,0xF7,0x3B,0x87,0x83,0x7F,0xF7,0x07,0x00,0x00,0x00,0xC0,0xCF,0x03,0x3C,
                0xB0,0xF7,0xF7,0x3F,0xC7,0x73,0x7E,0x7F,0x00,0x00,0x00,0x00,0xC0,0xE7,0xB7,0x3F,
                0xF0,0xFE,0xF7,0x3F,0xE7,0xF1,0x7F,0xFF,0x03,0x00,0x00,0x00,0x80,0xE7,0xB7,0x1F,
                0xF0,0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x70,0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x7C,0x00,0x80,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0xF0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,
                0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,
                0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,
                0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0x83,0xFF,0xF0,0x01,0x00,0x8F,0xFF,0x1E,0x00,0x7E,0x80,0x0F,0x7F,0xFE,
                0xFF,0xFF,0x83,0x7F,0xF0,0x01,0x00,0x0F,0x7F,0x1E,0x7F,0x3C,0x00,0x0F,0x3E,0xFE,
                0xFF,0xFF,0xC1,0x1F,0xF8,0xF1,0xFF,0x1F,0x7E,0x1E,0x7F,0x1C,0x00,0x3E,0x3C,0xFE,
                0xFF,0xFF,0xC1,0x07,0xFE,0xF1,0xFF,0x3F,0x3C,0x1F,0x3F,0x1C,0x3C,0x7C,0x18,0xFE,
                0xFF,0xFF,0xC0,0x81,0xFF,0xF1,0xFF,0xFF,0x98,0x1F,0x0F,0x1C,0x7E,0xFC,0x08,0xFF,
                0xFF,0xFF,0x00,0xF0,0xFF,0xF1,0xFF,0xFF,0xC8,0x1F,0x00,0x1F,0x7F,0xFC,0x81,0xFF,
                0xFF,0xFF,0x00,0xFC,0xFF,0x01,0xC0,0xFF,0xC9,0x1F,0xF0,0x1F,0x7F,0xFC,0xC1,0xFF,
                0xFF,0xFF,0x00,0xFE,0xFF,0x01,0xC0,0xFF,0xE3,0x1F,0x80,0x1F,0x7F,0xFC,0xC0,0xFF,
                0xFF,0x7F,0x00,0xF8,0xFF,0x01,0x80,0xFF,0xF3,0x1F,0x00,0x1C,0x7F,0x7C,0xC0,0xFF,
                0xFF,0x7F,0xF0,0xF8,0xFF,0xF9,0xFF,0xFF,0xF3,0x1F,0x1F,0x18,0x7F,0x7C,0x88,0xFF,
                0xFF,0x7F,0xF8,0xF0,0xFF,0xF8,0xFF,0xFF,0xF3,0x9F,0x3F,0x18,0x7F,0x3C,0x0C,0xFF,
                0xFF,0x3F,0xF8,0xE1,0xFF,0xF8,0xFF,0xFF,0xF3,0x9F,0x3F,0x18,0x7F,0x3C,0x1E,0xFE,
                0xFF,0x3F,0xF8,0xC3,0x7F,0xFC,0xFF,0xFF,0xF3,0x9F,0x3F,0x18,0x7F,0x3C,0x1F,0xFE,
                0xFF,0x1F,0xFC,0x87,0x3F,0xFC,0xFF,0xFF,0xF3,0x9F,0x1F,0x18,0x3F,0x1C,0x3F,0xFC,
                0xFF,0x1F,0xFC,0x0F,0x3F,0x00,0xE0,0xFF,0xF3,0x1F,0x00,0x1C,0x00,0x1C,0x3F,0xF8,
                0xFF,0x1F,0xFC,0x0F,0x3E,0x00,0xE0,0xFF,0xF3,0x1F,0x00,0x7F,0x00,0xBC,0x7F,0xF8,
                0xFF,0x0F,0xFE,0x1F,0x3E,0x00,0xF0,0xFF,0xF3,0x1F,0xF0,0xFF,0x03,0xFF,0x7F,0xFC,
                0xFF,0xFF,0xFF,0x3F,0xFE,0xFF,0xFF,0xFF,0xF3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,
                0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,
                0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,
                0xF0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x38,0x9E,0xDF,0xDF,0xF3,0x30,0xCF,0x73,0x7F,0xDE,0x1F,
                0x00,0x00,0x00,0x00,0x00,0x38,0x9F,0xFF,0xFF,0xF7,0x99,0xDF,0xF7,0x7F,0xDF,0x1F,
                0x00,0x00,0x00,0x00,0x00,0xB8,0xBB,0xE7,0xFD,0xF7,0x9F,0xD9,0xF7,0x07,0x1F,0x0E,
                0x00,0x00,0x00,0x00,0x00,0xB8,0x31,0xFF,0xF8,0x7F,0xCF,0xD9,0xFD,0x7B,0x0E,0x0F,
                0x00,0x00,0x00,0x00,0x00,0xFC,0x3F,0xFE,0xFC,0x7C,0xC7,0xFF,0xFD,0x7B,0x8E,0x07,
                0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x7C,0xE7,0xFF,0xFD,0x7F,0x8E,0x07,
                0x00,0x00,0x00,0x00,0x00,0xFF,0xF1,0xCF,0xDF,0x3C,0xF7,0xF9,0x3D,0x7F,0xCE,0x03,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"C:\Users\qw200\Desktop\indexa.bmp",0*/
        };

const float PID_MAX = 10.00;//PID最大允许值
//PID变量
float Kpid[3] = { 9.97,0.2,0.01 };//Kp,Ki,Kd
// float Kp=8.96;
// float Ki=0.2;
// float Kd=0.01;

#define MAX_CHARS_PER_LINE 20  // 每行最大字符数
#define MAX_LINES_PER_PAGE 5   // 每页最大行数

String textContent;
int currentPage = 0;
int totalLines = 0;
uint8_t disappear_step = 1;


float angle, angle_last;
//实时坐标
uint8_t chart_x;
bool frame_is_drawed = false;

//指向buf首地址的指针
uint8_t* buf_ptr;
uint16_t buf_len;

//选择界面变量
uint8_t x;
int16_t y, y_trg;//目标和当前
uint8_t line_y, line_y_trg;//线的位置
uint8_t box_width, box_width_trg;//框的宽度
int16_t box_y, box_y_trg;//框的当前值和目标值
int8_t ui_select;//当前选中那一栏

//pid界面变量
//int8_t pid_y,pid_y_trg;
uint8_t pid_line_y, pid_line_y_trg;//线的位置
uint8_t pid_box_width, pid_box_width_trg;//框的宽度
int16_t pid_box_y, pid_box_y_trg;//框的当前值和目标值
int8_t pid_select;//当前选中那一栏

//icon界面变量
int16_t icon_x, icon_x_trg;
int16_t app_y, app_y_trg;

int8_t icon_select;

uint8_t ui_index, ui_state;


enum//ui_index
{
    M_LOGO,//开始界面
    M_SELECT,//选择界面
    M_PID,//PID界面
    M_PID_EDIT,//pid编辑
    M_ICON,//icon界面
    M_CHART,//图表
    M_TEXT_EDIT,//文字编辑
    M_VIDEO,//视频显示
    M_ABOUT,//关于本机
    M_IMPORT,
    M_TXTREAD,
    M_ABOUTDEV,
    M_SET
};


enum//ui_state
{
    S_NONE,
    S_DISAPPEAR,
    S_SWITCH,
    S_MENU_TO_MENU,
    S_MENU_TO_PIC,
    S_PIC_TO_MENU,
};

//const char* text="This is a text Hello world ! follow up one two three four may jun july";

//菜单结构体
typedef struct
{
    char* select;
} SELECT_LIST;

SELECT_LIST* list = NULL; // (主要的菜单结构，用于展示站点和文本)初始化结构体数组指针

SELECT_LIST* pid = NULL; // (二级菜单结构，用于展示站点的多个用户)初始化结构体数组指针


uint8_t pid_num = 5;//PID选项数量

//SELECT_LIST list[]
//{
//  {"MainUI"},
//  {"+PID Editor"},
//  {"-Icon Test"},
//  {"-Chart Test"},
//  {"-Text Edit"},
//  {"-Play Video"},
//  {"{ About }"},
//  {"{ About }"},
//  {"{ About }"},
//};
int listSize = 7;//选择界面菜单个数
int pidSize = 5;

uint8_t single_line_length = 63 / listSize;
uint8_t total_line_length = single_line_length * listSize + 1;

SELECT_LIST icon[]
        {
                {"HOME"},
                {"Authenticator"},
                {"TXT"},
                {"SET"},
                {"LOCK"},
        };

//设备名称
char  name[] = "";
String Passwordis="";
//允许名字的最大长度
const uint8_t name_len = 6;//0-11for name  12 for return
char  title[] = "Password";
char  bt[] = "sure";

uint8_t edit_index = 0;
bool edit_flag = false;//默认不在编辑
uint8_t blink_flag;//默认高亮
const uint8_t BLINK_SPEED = 16;//2的倍数

uint8_t icon_num = sizeof(icon) / sizeof(SELECT_LIST);

//按键变量
typedef struct
{
    bool val;
    bool last_val;
}KEY;

KEY key[3] = { false };

//按键信息
typedef struct
{
    uint8_t id;
    bool pressed;
}KEY_MSG;

KEY_MSG key_msg = { 0 };

void createJson(String fileName, String key, String value) {
    StaticJsonDocument<200> doc;   //JSON document size

    doc[key] = value;

    delay(500); // 添加延时以确保前一个文件句柄已被释放

    File jsonFile = SD.open("/"+fileName, FILE_WRITE); // 定义并初始化File对象

    delay(100); // 添加另一个延时

    if (jsonFile) {
        serializeJson(doc, jsonFile);
        jsonFile.close();
    } else {
        Serial.println("Error opening file for writing!");
    }
}
void updateJson(String fileName, String key, String value) {
    File  jsonFile = SD.open("/"+fileName, FILE_READ);

    DynamicJsonDocument doc(1024);

    DeserializationError error = deserializeJson(doc, jsonFile);

    jsonFile.close();

    if (error) {
        Serial.println("Error deserializing JSON file!");
        return;
    }

    doc[key] = value;

    delay(100); // 添加延时以确保前一个文件句柄已被释放

    jsonFile = SD.open("/"+fileName, FILE_WRITE);

    if (jsonFile) {
        serializeJson(doc, jsonFile);
        jsonFile.close();
    } else {
        Serial.println("Error opening222 file for writing!");
    }
}
void writeCSV(){

    // 打开CSV文件
    File csvFile = SD.open("/data.csv");

    // 跳过第一行
    if (csvFile) {
        csvFile.readStringUntil('\n'); // 跳过第一行
    } else {
        Serial.println("Error opening CSV file!");
        return;
    }

    // 读取CSV文件内容并创建/更新JSON文件
    while (csvFile.available()) {
        String data = csvFile.readStringUntil('\n'); // 读取一行数据
        if (data == "") {
            break; // 结束循环，文件已读取完整
        }

        String ip = data.substring(0, data.indexOf(","));
        int firstCommaIndex = data.indexOf(",");
        int secondCommaIndex = data.indexOf(",", firstCommaIndex + 1);
        int thirdCommaIndex = data.indexOf(",", secondCommaIndex + 1);
        int fourthCommaIndex = data.indexOf(",", thirdCommaIndex + 1);
        int fourth2CommaIndex = data.indexOf(",", fourthCommaIndex + 1);
        String key = data.substring(thirdCommaIndex + 1, fourthCommaIndex);
        String value = data.substring(fourthCommaIndex + 1, fourth2CommaIndex);
        Serial.println(ip);
        Serial.println(key);
        Serial.println(value);
        Serial.println("----------------------------------");
        String fileName = ip + ".json";

        if (SD.exists("/" + fileName)) {
            // 文件已存在，更新JSON文件中的键值对
            updateJson(fileName, key, value);
        } else {
            // 文件不存在，创建新的JSON文件
            createJson(fileName, key, value);
        }
    }

    csvFile.close(); // 关闭CSV文件
    // 关闭CSV文件

}
bool get_key_val(uint8_t ch)
{
    switch (ch)
    {
        case 0:
            return digitalRead(BTN0);
            break;
        case 1:
            return digitalRead(BTN1);
            break;
        case 2:
            return digitalRead(BTN2);
            break;
        default:
            break;
    }
}

void key_init()
{
    for (uint8_t i = 0; i < (sizeof(key) / sizeof(KEY)); ++i)
    {
        key[i].val = key[i].last_val = get_key_val(i);
    }
}

void key_scan()
{
    for (uint8_t i = 0; i < (sizeof(key) / sizeof(KEY)); ++i)
    {
        key[i].val = get_key_val(i);//获取键值
        if (key[i].last_val != key[i].val)//发生改变
        {
            key[i].last_val = key[i].val;//更新状态
            if (key[i].val == LOW)
            {
                key_msg.id = i;
                key_msg.pressed = true;
            }
        }
    }
}

//移动函数
bool move(int16_t* a, int16_t* a_trg)
{
    if (*a < *a_trg)
    {
        *a += SPEED;
        if (*a > *a_trg) *a = *a_trg;//加完超过
    }
    else if (*a > *a_trg)
    {
        *a -= SPEED;
        if (*a < *a_trg) *a = *a_trg;//减完不足
    }
    else
    {
        return true;//到达目标
    }
    return false;//未到达
}

//移动函数
bool move_icon(int16_t* a, int16_t* a_trg)
{
    if (*a < *a_trg)
    {
        *a += ICON_SPEED;
        if (*a > *a_trg) *a = *a_trg;//加完超过
    }
    else if (*a > *a_trg)
    {
        *a -= ICON_SPEED;
        if (*a < *a_trg) *a = *a_trg;//减完不足
    }
    else
    {
        return true;//到达目标
    }
    return false;//未到达
}

//宽度移动函数
bool move_width(uint8_t* a, uint8_t* a_trg, uint8_t select, uint8_t id)
{
    if (*a < *a_trg)
    {
        uint8_t step = 16 / SPEED;//判断步数
        uint8_t len;
        if (ui_index == M_SELECT)
        {
            len = abs(u8g2.getStrWidth(list[select].select) - u8g2.getStrWidth(list[id == 0 ? select + 1 : select - 1].select));
        }
        else if (ui_index == M_PID)
        {
            len = abs(u8g2.getStrWidth(pid[select].select) - u8g2.getStrWidth(pid[id == 0 ? select + 1 : select - 1].select));
        }
        uint8_t width_speed = ((len % step) == 0 ? (len / step) : (len / step + 1));   //计算步长
        *a += width_speed;
        if (*a > *a_trg) *a = *a_trg;//
    }
    else if (*a > *a_trg)
    {
        uint8_t step = 16 / SPEED;//判断步数
        uint8_t len;
        if (ui_index == M_SELECT)
        {
            len = abs(u8g2.getStrWidth(list[select].select) - u8g2.getStrWidth(list[id == 0 ? select + 1 : select - 1].select));
        }
        else if (ui_index == M_PID)
        {
            len = abs(u8g2.getStrWidth(pid[select].select) - u8g2.getStrWidth(pid[id == 0 ? select + 1 : select - 1].select));
        }
        uint8_t width_speed = ((len % step) == 0 ? (len / step) : (len / step + 1));   //计算步长
        *a -= width_speed;
        if (*a < *a_trg) *a = *a_trg;
    }
    else
    {
        return true;//到达目标
    }
    return false;//未到达
}

//进度条移动函数
bool move_bar(uint8_t* a, uint8_t* a_trg)
{
    if (*a < *a_trg)
    {
        uint8_t step = 16 / SPEED;//判断步数
        uint8_t width_speed = ((single_line_length % step) == 0 ? (single_line_length / step) : (single_line_length / step + 1));   //计算步长
        *a += width_speed;
        if (*a > *a_trg) *a = *a_trg;//
    }
    else if (*a > *a_trg)
    {
        uint8_t step = 16 / SPEED;//判断步数
        uint8_t width_speed = ((single_line_length % step) == 0 ? (single_line_length / step) : (single_line_length / step + 1));   //计算步长
        *a -= width_speed;
        if (*a < *a_trg) *a = *a_trg;
    }
    else
    {
        return true;//到达目标
    }
    return false;//未到达
}

//文字编辑函数
void text_edit(bool dir, uint8_t index)
{
    if (!dir)
    {
        if (name[index] >= 'A' && name[index] <= 'Z')//大写字母
        {
            if (name[index] == 'A')
            {
                name[index] = 'z';
            }
            else
            {
                name[index] -= 1;
            }
        }
        else if (name[index] >= 'a' && name[index] <= 'z')//小写字母
        {
            if (name[index] == 'a')
            {
                name[index] = ' ';
            }
            else
            {
                name[index] -= 1;
            }
        }
        else
        {
            name[index] = 'Z';
        }
    }
    else
    {
        if (name[index] >= 'A' && name[index] <= 'Z')//大写字母
        {
            if (name[index] == 'Z')
            {
                name[index] = ' ';
            }
            else
            {
                name[index] += 1;
            }
        }
        else if (name[index] >= 'a' && name[index] <= 'z')//小写字母
        {
            if (name[index] == 'z')
            {
                name[index] = 'A';
            }
            else
            {
                name[index] += 1;
            }
        }
        else
        {
            name[index] = 'a';
        }
    }
    Serial.println("NOW IS");
    Serial.println(name);
}

//消失函数
void disappear()
{
    switch (disappear_step)
    {
        case 1:
            for (uint16_t i = 0; i < buf_len; ++i)
            {
                if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x55;
            }
            break;
        case 2:
            for (uint16_t i = 0; i < buf_len; ++i)
            {
                if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0xAA;
            }
            break;
        case 3:
            for (uint16_t i = 0; i < buf_len; ++i)
            {
                if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x00;
            }
            break;
        case 4:
            for (uint16_t i = 0; i < buf_len; ++i)
            {
                if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0x00;
            }
            break;
        default:
            ui_state = S_NONE;
            disappear_step = 0;
            break;
    }
    disappear_step++;
}

int compare(const void *a, const void *b) {
    return strcmp(((SELECT_LIST*)a)->select, ((SELECT_LIST*)b)->select);
}
int compare2(const void *elem1, const void *elem2) {
    SELECT_LIST* item1 = (SELECT_LIST*)elem1;
    SELECT_LIST* item2 = (SELECT_LIST*)elem2;

    // 判断如果元素为最后一个元素，则不进行排序
    if (item1 == &pid[pidSize-1] || item2 == &pid[pidSize-1]) {
        return 0; // 返回0表示两个元素相等
    }

    return strcmp(item1->select, item2->select);
}

void drawPage(int page) {
    u8g2.firstPage();
    int startIdx = page * MAX_LINES_PER_PAGE;
    int endIdx = min((page + MAX_LINES_PER_PAGE) * MAX_LINES_PER_PAGE, totalLines);

    for (int i = startIdx; i < endIdx; i++) {

        String line = textContent.substring(i * (MAX_CHARS_PER_LINE + 1), (i + 1) * (MAX_CHARS_PER_LINE + 1));
        u8g2.setCursor(0, 10 + (i - startIdx) * 10);
        u8g2.print(line);
    }

    do {
        u8g2.nextPage();
    } while (u8g2.nextPage());
}
void txtread(){
    File file = SD.open("/hh.txt");
    if (file) {
        Serial.println("txt on");
        while (file.available()) {
            textContent += (char)file.read();
        }
        file.close();
    } else {
        Serial.println("无法打开文件");
    }

    // 计算总行数
    Serial.println("txt connte");
}
/**************************界面显示*******************************/

void logo_ui_show()//显示logo
{
    if (!SD.begin(7)) {
        Serial.println("SD 卡初始化失败！");
        u8g2.firstPage();
        do {
            u8g2.drawStr(0, 10, "No Find TF card !");
            u8g2.drawStr(0, 23, "Please check it");
            u8g2.drawStr(0, 36, "And restart...");
            u8g2.drawStr(0, 49, "More see at yang17.site");
            u8g2.drawStr(0, 62,"                       ---KeyBox");
        } while (u8g2.nextPage());

    }else {
        u8g2.drawXBMP(0, 0, 128, 64, LOGO);
    }
    // for(uint16_t i=0;i<buf_len;++i)
    // {
    //   if(i%4==0||i%4==1)
    //   {
    //   buf_ptr[i]=buf_ptr[i] & 0x33;
    //   }
    //   else
    //   {
    //   buf_ptr[i]=buf_ptr[i] & 0xCC;
    //   }
    // }

}

void select_ui_show()//选择界面
{
    move_bar(&line_y, &line_y_trg);
    move(&y, &y_trg);
    move(&box_y, &box_y_trg);
    move_width(&box_width, &box_width_trg, ui_select, key_msg.id);
    u8g2.drawVLine(126, 0, total_line_length);
    u8g2.drawPixel(125, 0);
    u8g2.drawPixel(127, 0);
    for (uint8_t i = 0; i < listSize; ++i)
    {
        u8g2.drawStr(x, 16 * i + y + 12, list[i].select);  // 第一段输出位置
        u8g2.drawPixel(125, single_line_length * (i + 1));
        u8g2.drawPixel(127, single_line_length * (i + 1));
    }
    u8g2.drawVLine(125, line_y, single_line_length - 1);
    u8g2.drawVLine(127, line_y, single_line_length - 1);
    u8g2.setDrawColor(2);
    u8g2.drawRBox(0, box_y, box_width, 16, 1);
    u8g2.setDrawColor(1);
}

void pid_ui_show()//PID界面
{
    move_bar(&pid_line_y, &pid_line_y_trg);
    move(&pid_box_y, &pid_box_y_trg);
    move_width(&pid_box_width, &pid_box_width_trg, pid_select, key_msg.id);//修改pid_select加1，修复指针问题
    u8g2.drawVLine(126, 0, 61);
    u8g2.drawPixel(125, 0);
    u8g2.drawPixel(127, 0);
    for (uint8_t i = 0; i < pidSize; ++i)
    {
        Serial.println("-----------pidsize");
        Serial.println(pidSize);
        Serial.println(pid[i].select);
        u8g2.drawStr(x, 16 * i + 12, pid[i].select);  // 第一段输出位置
        u8g2.drawPixel(125, 15 * (i + 1));
        u8g2.drawPixel(127, 15 * (i + 1));
    }

    u8g2.setDrawColor(2);
    u8g2.drawRBox(0, pid_box_y, pid_box_width, 16, 1);
    u8g2.setDrawColor(1);
    u8g2.drawVLine(125, pid_line_y, 14);
    u8g2.drawVLine(127, pid_line_y, 14);
}

void pid_edit_ui_show()//显示PID编辑
{
    u8g2.drawBox(16, 16, 96, 31);
    u8g2.setDrawColor(2);
    u8g2.drawBox(17, 17, 94, 29);
    u8g2.setDrawColor(1);

    //u8g2.drawBox(17,17,(int)(Kpid[pid_select]/PID_MAX)),30);
    u8g2.drawFrame(18, 36, 60, 8);
    u8g2.drawBox(20, 38, (uint8_t)(Kpid[pid_select] / PID_MAX * 56), 4);

    u8g2.setCursor(22, 30);
    switch (pid_select)
    {
        case 0:
            u8g2.print("Editing Kp");
            break;
        case 1:
            u8g2.print("Editing Ki");
            break;
        case 2:
            u8g2.print("Editing Kd");
            break;
        default:
            break;
    }

    u8g2.setCursor(81, 44);
    u8g2.print(Kpid[pid_select]);

}

void icon_ui_show(void)//显示icon
{

    move_icon(&icon_x, &icon_x_trg);
    move(&app_y, &app_y_trg);

    for (uint8_t i = 0; i < icon_num; ++i)
    {
        u8g2.drawXBMP(46 + icon_x + i * ICON_SPACE, 6, 36, icon_width[i], icon_pic[i]);
        u8g2.setClipWindow(0, 48, 128, 64);
        u8g2.drawStr((128 - u8g2.getStrWidth(icon[i].select)) / 2, 62 - app_y + i * 16, icon[i].select);
        u8g2.setMaxClipWindow();
    }

}
void chart_draw_frame()//chart界面
{

    u8g2.drawStr(4, 12, "Real time angle :");
    u8g2.drawRBox(4, 18, 120, 46, 8);
    u8g2.setDrawColor(2);
    u8g2.drawHLine(10, 58, 108);
    u8g2.drawVLine(10, 24, 34);
    //箭头
    u8g2.drawPixel(7, 27);
    u8g2.drawPixel(8, 26);
    u8g2.drawPixel(9, 25);

    u8g2.drawPixel(116, 59);
    u8g2.drawPixel(115, 60);
    u8g2.drawPixel(114, 61);
    u8g2.setDrawColor(1);

}
void chart_ui_show()//chart界面
{
    if (!frame_is_drawed)//框架只画一遍
    {
        u8g2.clearBuffer();
        chart_draw_frame();
        //  angle_last = 20.00 + (float)analogRead(READ) / 100.00;
        frame_is_drawed = true;
    }

    u8g2.drawBox(96, 0, 30, 14);

    u8g2.drawVLine(chart_x + 10, 59, 3);
    if (chart_x == 100) chart_x = 0;

    //u8g2.drawBox(chart_x+11,24,8,32);

    u8g2.drawVLine(chart_x + 11, 24, 34);
    u8g2.drawVLine(chart_x + 12, 24, 34);
    u8g2.drawVLine(chart_x + 13, 24, 34);
    u8g2.drawVLine(chart_x + 14, 24, 34);

    //异或绘制
    u8g2.setDrawColor(2);
    //  angle = 20.00 + (float)analogRead(READ) / 100.00;
    u8g2.drawLine(chart_x + 11, 58 - (int)angle_last / 2, chart_x + 12, 58 - (int)angle / 2);
    u8g2.drawVLine(chart_x + 12, 59, 3);
    angle_last = angle;
    chart_x += 2;
    u8g2.drawBox(96, 0, 30, 14);
    u8g2.setDrawColor(1);


    u8g2.setCursor(96, 12);
    u8g2.print(angle);

}



void text_edit_ui_show()
{
    u8g2.drawRFrame(4, 6, 120, 52, 8);
    u8g2.drawStr((128 - u8g2.getStrWidth(title)) / 2, 20, title);
    u8g2.drawStr(10, 38, name);
    u8g2.drawStr(80, 50, bt);

    uint8_t box_x = 9;

    //绘制光标
    if (edit_index < name_len)
    {
        if (blink_flag < BLINK_SPEED / 2)
        {
            for (uint8_t i = 0; i < edit_index; ++i)
            {
                char temp[2] = { name[i],'\0' };
                box_x += u8g2.getStrWidth(temp);
                if (name[i] != ' ')
                {
                    box_x++;
                }
            }
            char temp[2] = { name[edit_index],'\0' };
            u8g2.setDrawColor(2);
            u8g2.drawBox(box_x, 26, u8g2.getStrWidth(temp) + 2, 16);
            u8g2.setDrawColor(1);
        }
    }
    else
    {
        u8g2.setDrawColor(2);
        u8g2.drawRBox(78, 38, u8g2.getStrWidth(bt) + 4, 16, 1);
        u8g2.setDrawColor(1);
    }

    if (edit_flag)//处于编辑状态
    {
        if (blink_flag < BLINK_SPEED)
        {
            blink_flag += 1;
        }
        else
        {
            blink_flag = 0;
        }
    }
    else
    {
        blink_flag = 0;
    }

}
char* SiteIn;
char* UserIn;
void aboutdev_ui_show()//about界面
{
    //  std::string SiteStr = SiteIn;
 //   u8g2.drawStr(2, 10, "---Press to Driver---");
  //  u8g2.drawStr(2, 10, "---Press to Driver---");
  //  u8g2.drawStr(2, 10, "---Press to Driver---");
//u8g2.setContrast(10);
     u8g2.drawStr(2,12,"MCU      :       ESP32C3");
     u8g2.drawStr(2,28,"FLASH    :            4MB");
     u8g2.drawStr(2,44,"Site     :    yang17.site");
     u8g2.drawStr(2,60,"Firmware :          V3.0");

}
void about_ui_show()//about界面
{
    //  std::string SiteStr = SiteIn;
    Serial.println("PUSH---------");
    Serial.println(SiteIn);
    Serial.println(UserIn);
    Serial.println(passwordIn);
    std::string SiteStr = SiteIn;
    // Concatenate the strings
    std::string SiteString = "Site: " + SiteStr;
    std::string UserStr = UserIn;

    // Concatenate the strings
    std::string UserString = "User: " + UserStr;
    std::string PasswordStr = passwordIn;

    // Concatenate the strings
    std::string PasswordString = "Password: " + PasswordStr;
    u8g2.drawStr(2, 12, SiteString.c_str());
    u8g2.drawStr(2, 28, UserString.c_str());
    u8g2.drawStr(2, 44, PasswordString.c_str());
    u8g2.drawStr(2, 60, "---Press to Driver---");

    // u8g2.drawStr(2,12,"MCU : MEGA2560");
    // u8g2.drawStr(2,28,"FLASH : 256KB");
    // u8g2.drawStr(2,44,"SRAM : 8KB");
    // u8g2.drawStr(2,60,"EEPROM : 4KB");

}
void import_ui_show()//about界面
{


    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 10);

    u8g2.print("Hello, World!");


}
/**************************界面处理*******************************/

void logo_proc()//logo界面处理函数
{
    if (key_msg.pressed)
    {


            key_msg.pressed = false;

                    if(passwordst==0) {
                    ui_state = S_DISAPPEAR;
                    ui_index = M_ICON;
                    }else{
                        key_msg.pressed = false;
                        ui_state = S_DISAPPEAR;
                        ui_index = M_TEXT_EDIT;
                    }


    }
    logo_ui_show();
}

void pid_edit_proc(void)//pid编辑界面处理函数
{
    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        switch (key_msg.id)
        {
            case 0:
                if (Kpid[pid_select] > 0) Kpid[pid_select] -= 0.01;
                break;
            case 1:
                if (Kpid[pid_select] < PID_MAX) Kpid[pid_select] += 0.01;
                break;
            case 2:
                ui_index = M_PID;
                break;
            default:
                break;
        }
    }
    pid_ui_show();
    for (uint16_t i = 0; i < buf_len; ++i)
    {
        buf_ptr[i] = buf_ptr[i] & (i % 2 == 0 ? 0x55 : 0xAA);
    }
    pid_edit_ui_show();
}
void addPassword(char* mainsite,char* mainuser);
void pid_proc()//pid界面处理函数
{

    pid_ui_show();

    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        Serial.println("-------------------");
        Serial.println(pid_select);
        Serial.println(pidSize);
        switch (key_msg.id)
        {
            //  Serial.println(pid_select);
            case 0:
                if (pid_select != 0)
                {
                    pid_select -= 1;
                    pid_line_y_trg -= 15;
                    pid_box_y_trg -= 16;
                    break;
                }
                else
                {
                    break;
                }
            case 1:
                if (pid_select != pidSize-1)//修改pidSize-1，修复back导致数组溢出，下同
                {
                    pid_select += 1;
                    pid_line_y_trg += 15;
                    pid_box_y_trg += 16;
                }
                else
                {
                    break;
                }
                break;
            case 2:
                if (pid_select == pidSize-1)
                {
                    ui_index = M_SELECT;
                    ui_state = S_DISAPPEAR;
                    pid_select = 0;
                    pid_line_y = pid_line_y_trg = 1;
                    pid_box_y = pid_box_y_trg = 0;
                    pid_box_width = pid_box_width_trg = u8g2.getStrWidth(pid[pid_select].select) + x * 2;
                }
                else {

                    //json
                    Serial.println("json in");
                    UserIn = pid[pid_select].select;
                    addPassword(SiteIn, UserIn);
                    ui_index = M_ABOUT;

                }
                break;
            default:
                break;
        }
        pid_box_width_trg = u8g2.getStrWidth(pid[pid_select].select) + x * 2;
    }
}
// 函数原型声明
void addUser(char* mainnowdisplay);
int justonece=0;
void select_proc(void)//选择界面处理重要的
{
    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        switch (key_msg.id)
        {
            case 0:
                if (ui_select < 1) break;
                ui_select -= 1;
                line_y_trg -= single_line_length;
                if (ui_select < -(y / 16))
                {
                    y_trg += 16;
                }
                else
                {
                    box_y_trg -= 16;
                }

                break;
            case 1:
                if ((ui_select + 2) > listSize) break;//变更菜单数组个数3
                ui_select += 1;
                line_y_trg += single_line_length;
                if ((ui_select + 1) > (4 - y / 16))
                {
                    y_trg -= 16;
                }
                else
                {
                    box_y_trg += 16;
                }

                break;
            case 2:
                Serial.println("ui_select");
                switch (ui_select)
                {
                    case 0:     //return
                        ui_state = S_DISAPPEAR; //S_DISAPPEAR;
                        ui_index = M_LOGO;//M_LOGO;
                        break;
//                    case 1:     //pid
//                        addUser(list[ui_select].select);
//                        ui_state = S_DISAPPEAR;
//                        ui_index = M_PID;
//                        break;
//                    case 2:   //icon
//                        ui_state = S_DISAPPEAR;
//                        ui_index = M_ICON;
//                        break;
//                    case 3:   //chart
//                        ui_state = S_DISAPPEAR;
//                        ui_index = M_CHART;
//                        break;
//                    case 4:   //textedit
//                        ui_state = S_DISAPPEAR;
//                        ui_index = M_TEXT_EDIT;
//                        break;
//                    case 6:   //about
//                        ui_state = S_DISAPPEAR;
//                        ui_index = M_ABOUT;
//                        break;
//                    case 7:   //about
//                        ui_state = S_DISAPPEAR;
//                        ui_index = M_ABOUT;
//                        break;
//                    case 8:   //about
//                        ui_state = S_DISAPPEAR;
//                        ui_index = M_ABOUT;
//                        break;
                    default: {
                        Serial.println("modelchooese");
                        Serial.println(modelchooese);
                        if (modelchooese == 0) {
                            SiteIn = list[ui_select].select;
                            // if(justonece==0) {//防止重复向数组写入导致内存浪费溢出
                            addUser(list[ui_select].select);
                            //     justonece+=1;
                            //   }
                            pid_box_width = pid_box_width_trg = u8g2.getStrWidth(pid[pid_select].select) + x * 2;//两边各多2
                            ui_state = S_DISAPPEAR;
                            ui_index = M_PID;
                            break;
                        } else if (modelchooese == 1){
                            // list[ui_select].select
                            //txt
                            ui_state = S_DISAPPEAR;
                            File file = SD.open("/"+(String)list[ui_select].select+".txt");
                            if (file) {
                                Serial.println("txt on");
                                while (file.available()) {
                                    textContent += (char)file.read();
                                }
                                file.close();
                            } else {
                                Serial.println("无法打开文件");
                            }

                            // 计算总行数
                            totalLines = textContent.length() / (MAX_CHARS_PER_LINE + 1);  // 假设每行除了字符外还有换行符
                            if (textContent.length() % (MAX_CHARS_PER_LINE + 1) != 0) {
                                totalLines++;
                            }
                            Serial.println("txt connte");
                            Serial.println("txt in");

                            ui_index = M_TXTREAD;

                            break;
                        } else if (modelchooese == 2) {

                            Serial.print("NOW:");
                            Serial.println(list[ui_select].select);
                            ui_state = S_DISAPPEAR;
                            ui_index = M_SET;
                        }
                    }
                }

                //Serial.println("Btn2");
            default:
                break;
        }
        Serial.println("ui_select------------------test");
        Serial.println(ui_select);
        Serial.println(list[ui_select].select);
        box_width_trg = u8g2.getStrWidth(list[ui_select].select) + x * 2;
    }
    select_ui_show();
}
void setting(int select){
    switch (select) {
        case 1:
            ui_state = S_DISAPPEAR;
            ui_index = M_TEXT_EDIT;
            break;
        case 2:
            free(pid);
            pid = NULL;
            pidSize=5;//此值自增1才可出返回设置



            pid = (SELECT_LIST*)malloc(pidSize * sizeof(SELECT_LIST));
            pid[0].select =strdup("1min");
            pid[1].select =strdup("5min");
           // pid[2].select =strdup("10min");//未知，缺少size函数
            pid[2].select =strdup("Never");
            pid[2].select =strdup("back");
            pid_box_width = pid_box_width_trg = u8g2.getStrWidth(pid[pid_select].select) + x * 2;//两边各多2
            ui_state = S_DISAPPEAR;
            ui_index = M_PID;
            break;
        case 3:
           // ui_state = S_DISAPPEAR;
            u8g2.drawStr(2,12,"Not Find New DATA !");
          //  u8g2.drawStr(2,28,"FLASH    :            4MB");
            u8g2.drawStr(2,44,"Press any key go back");
           // u8g2.drawStr(2,60,"Firmware :          V3.0");
            break;
        case 4:
            ui_state = S_DISAPPEAR;
            ui_index = M_ABOUTDEV;
            break;
        default:
            Serial.println("now is default");
            break;
    }

}
void set_proc(){
    setting(ui_select);
    Serial.println("txgog11111111111111ogote");
    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        switch (key_msg.id)
        {
            case 2: {

                //  writeCSV();存在危险操作，建议提高延迟，否则易损卡
            }
            default:
                ui_state = S_DISAPPEAR;
                ui_index = M_SELECT;

                break;

        }
    }

}

void allcount();
void addSiteDataToArr();
void icon_proc(void)//icon界面处理
{
    icon_ui_show();
    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        switch (key_msg.id)
        {
            case 1:
                if (icon_select != (icon_num - 1))
                {
                    icon_select += 1;
                    app_y_trg += 16;
                    icon_x_trg -= ICON_SPACE;
                }
                break;
            case 0:

                if (icon_select != 0)
                {
                    icon_select -= 1;
                    app_y_trg -= 16;
                    icon_x_trg += ICON_SPACE;
                }
                break;
            case 2://2代表按下Mid按键


                switch (icon_select) {
                    case 0: ui_state = S_DISAPPEAR;
                        ui_index = M_LOGO;
                        break;
                    case 1: {
                        modelchooese=0;
                        allcount();
                        addSiteDataToArr();
                        list[0].select = strdup("HOME");

                        box_width = box_width_trg = u8g2.getStrWidth(list[ui_select].select) + x * 2;//两边各多2

                        //  pid = (SELECT_LIST*)malloc(pidSize * sizeof(SELECT_LIST));
                        //  pid[0].select = strdup("main");
                        // 动态分配内存以存储结构体数组

                        qsort(list + 1, listSize - 1, sizeof(SELECT_LIST), compare);

                        // 在串口上打印按照字母顺序排序后的结果
                        for (int i = 0; i < listSize; i++) {
                            Serial.println(list[i].select);
                        }
                        ui_state = S_DISAPPEAR;
                        ui_index = M_SELECT;

                        break;
                    }
                    case 2:
//                        ui_state = S_DISAPPEAR;
//                        ui_index = M_IMPORT;
                        modelchooese=1;
                        allcount();
                        addSiteDataToArr();
                        list[0].select = strdup("HOME");

                        box_width = box_width_trg = u8g2.getStrWidth(list[ui_select].select) + x * 2;//两边各多2

                        //  pid = (SELECT_LIST*)malloc(pidSize * sizeof(SELECT_LIST));
                        //  pid[0].select = strdup("main");
                        // 动态分配内存以存储结构体数组

                        qsort(list + 1, listSize - 1, sizeof(SELECT_LIST), compare);

                        // 在串口上打印按照字母顺序排序后的结果
                        for (int i = 0; i < listSize; i++) {
                            Serial.println(list[i].select);
                        }
                        ui_state = S_DISAPPEAR;
                        ui_index = M_SELECT;

                        break;
                    case 3:
//                        ui_state = S_DISAPPEAR;
//                        ui_index = M_IMPORT;
                        Serial.println("ui_select11");
                        modelchooese=2;
                        listSize=5;
                        Serial.println("ui_select22");
                        free(list);
                        list = NULL;
                        list = (SELECT_LIST*)malloc(listSize * sizeof(SELECT_LIST));
                        list[0].select = strdup("HOME");
                        list[1].select = strdup("Password");
                        list[2].select = strdup("Back light");
                        list[3].select = strdup("Updata");
                        list[4].select = strdup("About");
                        Serial.println("ui_selectsadad");
                        box_width = box_width_trg = u8g2.getStrWidth(list[ui_select].select) + x * 2;//两边各多2
                        Serial.println("ui_select33");
                        //  pid = (SELECT_LIST*)malloc(pidSize * sizeof(SELECT_LIST));
                        //  pid[0].select = strdup("main");
                        // 动态分配内存以存储结构体数组

                       // qsort(list + 1, listSize - 1, sizeof(SELECT_LIST), compare);
                        Serial.println("ui_select33");
                        // 在串口上打印按照字母顺序排序后的结果
                        for (int i = 0; i < listSize; i++) {
                            Serial.println(list[i].select);
                        }
                        ui_state = S_DISAPPEAR;
                        ui_index = M_SELECT;
                        Serial.println("ui_select3366");
                        break;
                    case 4:
                        if(Passwordis!=""){
                            passwordst=1;
                        }
                        ui_state = S_DISAPPEAR;
                        ui_index = M_LOGO;
                        break;
                }
                Serial.println("button press");
                Serial.println(icon_select);
                Serial.println(icon_x);
                Serial.println( app_y);
                Serial.println("num to zero");
                icon_select = 0;
                icon_x = icon_x_trg = 0;
                app_y = app_y_trg = 0;


                break;
            default:
                break;
        }
    }
}

void chart_proc()//chart界面处理函数
{
    chart_ui_show();
    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        ui_state = S_DISAPPEAR;
        ui_index = M_SELECT;
        frame_is_drawed = false;//退出后框架为未画状态
        chart_x = 0;
    }
}
void passwordmode(){
    for (int i = 0; i < 6; i++) {
        if (name[i] != '\0') {


            if (passwordst == 0) {
                Passwordis = name;
                passwordst=1;
                ui_state = S_DISAPPEAR;
                ui_index = M_LOGO;
            }else if (passwordst == 1&&Passwordis == name) {
                passwordst=0;
                ui_state = S_DISAPPEAR;
                ui_index = M_ICON;

            }else{
                ui_state = S_DISAPPEAR;
                ui_index = M_LOGO;
            }
            break;

        }else{


            if (passwordst == 0) {
                ui_state = S_DISAPPEAR;
                ui_index = M_LOGO;
            }

        }
    }
}
void text_edit_proc()
{
    text_edit_ui_show();
    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        switch (key_msg.id)
        {
            case 0:
                if (edit_flag)
                {
                    //编辑
                    text_edit(false, edit_index);
                }
                else
                {
                    if (edit_index == 0)
                    {
                        edit_index = name_len;
                    }
                    else
                    {
                        edit_index -= 1;
                    }
                }
                break;
            case 1:
                if (edit_flag)
                {
                    //编辑
                    text_edit(true, edit_index);
                }
                else
                {
                    if (edit_index == name_len)
                    {
                        edit_index = 0;
                    }
                    else
                    {
                        edit_index += 1;
                    }
                }
                break;
            case 2:
                if (edit_index == name_len) {

                    edit_index = 0;

                    passwordmode();

                    memset(name, '\0', sizeof(name));
                }
                else
                {
                    edit_flag = !edit_flag;
                }
                break;
            default:

                break;
        }
    }
}

void about_proc()//about界面处理函数
{
    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        ui_state = S_DISAPPEAR;
        ui_index = M_SELECT;
    }
    about_ui_show();
}
void aboutdev_proc()//about界面处理函数
{
    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        ui_state = S_DISAPPEAR;
        ui_index = M_SELECT;
    }
    aboutdev_ui_show();
}
void import_proc()//about界面处理函数
{
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 10);
    u8g2.drawStr(2,12,"PLEASE PUSH");
    u8g2.println("IF YOU ARE RIGHT");

    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        switch (key_msg.id)
        {
            case 2: {
                ui_state = S_DISAPPEAR;

                u8g2.firstPage();

                u8g2.setFont(u8g2_font_ncenB08_tr);
                u8g2.setCursor(0, 10);
                u8g2.println("Now WILL RUN");

                delay(5000); // 继续等待5秒
                //  writeCSV();存在危险操作，建议提高延迟，否则易损卡
            }
            default:
                ui_state = S_DISAPPEAR;
                ui_index = M_ICON;
                break;

        }
    }
    //  delay(5000); // 等待5秒


}
void txtred_proc(){

    if (currentPage >= totalLines / MAX_LINES_PER_PAGE) {
        currentPage = 0;
    }
    Serial.println("txt wait");
    drawPage(currentPage * MAX_LINES_PER_PAGE);
    Serial.print("当前页码: ");
    Serial.println(currentPage);
    // delay(200);

    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        switch (key_msg.id)
        {
            case 2: {
                currentPage++;
                //  writeCSV();存在危险操作，建议提高延迟，否则易损卡
            }
            default:
                ui_state = S_DISAPPEAR;
                ui_index = M_SELECT;
                totalLines=0;
                textContent="";

                break;

        }
    }

}

/********************************总的UI显示************************************/

void ui_proc()//总的UI进程
{
    switch (ui_state)
    {
        case S_NONE:
            if (ui_index != M_CHART) u8g2.clearBuffer();
            switch (ui_index)
            {
                case M_LOGO:
                    logo_proc();
                    break;
                case M_SELECT:
                    select_proc();
                    break;
                case M_PID:
                    pid_proc();
                    break;
                case M_ICON:
                    icon_proc();
                    break;
                case M_CHART:
                    chart_proc();
                    break;
                case M_TEXT_EDIT:
                    text_edit_proc();
                    break;
                case M_PID_EDIT:
                    pid_edit_proc();
                    break;
                case M_ABOUT:

                    about_proc();
                    break;
                case M_ABOUTDEV:

                    aboutdev_proc();
                    break;
                case M_IMPORT:
                    import_proc();
                    break;
                case M_TXTREAD:
                    txtred_proc();
                    break;
                case M_SET:
                    set_proc();
                    break;
                default:
                    break;
            }
            break;
        case S_DISAPPEAR:
            disappear();
            break;
        default:
            break;
    }
    u8g2.sendBuffer();
}


// 比较函数，用于对字符串进行排序


void addUser(char* mainnowdisplay){
    free(pid);
    pid = NULL;
    Serial.println("Btn2----------------------");
    Serial.println(mainnowdisplay);
    String filePath = String("/") + String(mainnowdisplay)+String(".json") ;
    // 打开 data.json 文件
    File jsonFile = SD.open(filePath);
    if (!jsonFile) {
        Serial.println("无法打开 data.json 文件");
        return;
    }

    String jsonStr;
    while (jsonFile.available()) {
        char c = jsonFile.read();
        jsonStr += c;
    }

    // 关闭文件
    jsonFile.close();

    // 解析JSON数据
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, jsonStr);

    if (error) {
        Serial.println("解析JSON失败");
        return;
    }
    int numx=0;
    // 仅输出JSON键而不输出值
    for (JsonPair keyValue : doc.as<JsonObject>()) {

        numx++;
    }
    Serial.print("numx------------xxx: ");
    Serial.println(numx);

    pidSize=numx+1;//此值自增1才可出返回设置



    pid = (SELECT_LIST*)malloc(pidSize * sizeof(SELECT_LIST));

    int num=0;
    // 仅输出JSON键而不输出值
    for (JsonPair keyValue : doc.as<JsonObject>()) {
        Serial.print("键: ");
        Serial.println(keyValue.key().c_str());
        pid[num].select =strdup(keyValue.key().c_str());
        num++;
    }
    pid[num].select =strdup("back");
    qsort(pid, pidSize, sizeof(SELECT_LIST), compare2);

    // 在串口上打印按照字母顺序排序后的结果
    //  for (int i = 0; i < pidSize; i++) {
    //     Serial.println(pid[i].select);
    // }

    //  pid[4].select =strdup("keyVal");
    Serial.println( "pid[3].select");
//    Serial.println( pid[2].select);
    //   Serial.println( pid[3].select);

}

void addPassword(char* mainsite,char* mainuser){


    String filePath = String("/") + String(mainsite)+String(".json");
    // 打开 data.json 文件
    File jsonFile = SD.open(filePath);

    if (!jsonFile) {
        Serial.println("无法打开文件");
        return;
    }

    // 读取文件内容
    String jsonStr;
    while (jsonFile.available()) {
        char c = jsonFile.read();
        jsonStr += c;
    }

    // 关闭文件
    jsonFile.close();

    // 解析 JSON 数据
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, jsonStr);

    if (error) {
        Serial.println("解析 JSON 失败");
        return;
    }
    Serial.println("read----");
    Serial.println(mainuser);
    // 提取并输出 location 字段的值
    const char* location = doc[mainuser];
    Serial.println("read----");
    Serial.println(location);
    // Convert String to char* and store in Site
    passwordIn = strdup(location);
    Serial.println("password--------");
    Serial.println(passwordIn);
    Serial.println("password--------");



}

void allcount(){
    char* model=".json";
    if(modelchooese==0){
        model=".json";
    }else{
        model=".txt";
    }

    File root = SD.open("/");
    int count=0;
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            // 没有更多文件
            break;
        }

        if (!entry.isDirectory() && entry.name()[0] != '.') {
            String filename = entry.name();
            if (filename.endsWith( model)) {
                count++;
            }
        }

        entry.close();
    }

    root.close();
    listSize=count+1;
}
void addSiteDataToArr() {//此函数提供方法使其全部网站保存在数组中，界面分类用
    free(list);
    list = NULL;
    char* model=".json";
    if(modelchooese==0){
        model=".json";
    }else{
        model=".txt";
    }
    Serial.println(model);
    Serial.println("read JSON Start:");
    File root = SD.open("/");
    int index = 0; // 用于追踪数组中的索引位置

    list = (SELECT_LIST*)malloc(listSize * sizeof(SELECT_LIST));
    int fileCount = 0;//此值传向list 数组长度
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            // 没有更多文件
            Serial.println("entry");
            break;
        }

        if (!entry.isDirectory() && entry.name()[0] != '.') {
            const char* filename = entry.name();
            if (strstr(filename,  model) != NULL) {
                Serial.println(filename);
                //  const char* filename = "example.txt"; // 您的文件名

                int length = strlen(filename);


                char* newFilename = new char[length]; // 创建动态分配的新字符串来存储较短的文件名
                if(modelchooese==0) {
                    strncpy(newFilename, filename, length - 5);
                    newFilename[length - 5] = '\0'; // 添加字符串终止符
                }else{
                    strncpy(newFilename, filename, length - 4);
                    newFilename[length - 4] = '\0'; // 添加字符串终止符
                }
                list[index+1].select = strdup(newFilename);
                index++;
            }
        }

        entry.close();
    }

}
void setup() {//加大审查，尽量关闭sd卡使用时间延长寿命
    Serial.begin(115200);




    // 写入数据到 data.json 文件
    // writeJSONToFile();

    // 读取并打印 SD 卡上的所有 JSON 文件
    // readAndPrintAllJSONFiles();
    // readJSONFile() ;

    Serial.println("entry");

    //Wire.begin(21,22,400000);
    pinMode(BTN0, INPUT_PULLUP);
    pinMode(BTN1, INPUT_PULLUP);
    pinMode(BTN2, INPUT_PULLUP);
    key_init();

    u8g2.setBusClock(800000);
    u8g2.begin();
    u8g2.setFont(u8g2_font_unifont_t_chinese1);
    //u8g2.setContrast(10);

    buf_ptr = u8g2.getBufferPtr();//拿到buffer首地址
    buf_len = 8 * u8g2.getBufferTileHeight() * u8g2.getBufferTileWidth();

    x = 4;
    y = y_trg = 0;
    line_y = line_y_trg = 1;
    pid_line_y = pid_line_y_trg = 1;
    ui_select = pid_select = icon_select = 0;
    icon_x = icon_x_trg = 0;
    app_y = app_y_trg = 0;

    u8g2.setFont(u8g2_font_ncenB08_tr);
    ui_index = M_LOGO;
   // ui_index=M_TEXT_EDIT;
    ui_state = S_NONE;
    Serial.println("entry2");
}

void loop() {
    Serial.println("entry3");
    key_scan();
    ui_proc();
}