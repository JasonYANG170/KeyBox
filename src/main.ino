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

String OutPutString = "";
int keySize;
char* passwordIn;
struct key {
    String user;
    String password;
};

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
#define READ 2
#define BTN0 12
#define BTN1 18
#define BTN2 19

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ 5, /* data=*/ 4);   // ESP32 Thing, pure SW emulated I2C

PROGMEM const uint8_t icon_pic[][200]
        {
                {
                        0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x3E,
                        0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00,
                        0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00,
                        0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00,
                        0x7F, 0x00, 0x00, 0x00, 0x80, 0x7F, 0x00, 0x00,
                        0x00, 0x80, 0x7F, 0x00, 0x00, 0x00, 0xC0, 0x7F,
                        0x00, 0x00, 0x00, 0xE0, 0x7F, 0x00, 0x00, 0x00,
                        0xF8, 0x7F, 0x00, 0x00, 0xF0, 0xF8, 0xFF, 0xFF,
                        0x01, 0xFC, 0xF8, 0xFF, 0xFF, 0x07, 0xFC, 0xF8,
                        0xFF, 0xFF, 0x07, 0xFE, 0xF8, 0xFF, 0xFF, 0x07,
                        0xFE, 0xF8, 0xFF, 0xFF, 0x07, 0xFE, 0xF8, 0xFF,
                        0xFF, 0x07, 0xFE, 0xF8, 0xFF, 0xFF, 0x07, 0xFE,
                        0xF8, 0xFF, 0xFF, 0x07, 0xFE, 0xF8, 0xFF, 0xFF,
                        0x03, 0xFE, 0xF8, 0xFF, 0xFF, 0x03, 0xFE, 0xF8,
                        0xFF, 0xFF, 0x03, 0xFE, 0xF8, 0xFF, 0xFF, 0x03,
                        0xFE, 0xF8, 0xFF, 0xFF, 0x01, 0xFE, 0xF8, 0xFF,
                        0xFF, 0x01, 0xFE, 0xF8, 0xFF, 0xFF, 0x01, 0xFE,
                        0xF8, 0xFF, 0xFF, 0x01, 0xFE, 0xF8, 0xFF, 0xFF,
                        0x00, 0xFE, 0xF8, 0xFF, 0xFF, 0x00, 0xFC, 0xF8,
                        0xFF, 0x7F, 0x00, 0xFC, 0xF8, 0xFF, 0x3F, 0x00,
                        0xF8, 0xF8, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 /*"C:\Users\ROG\Desktop\三连\点赞.bmp",0*/
                        /* (36 X 35 )*/
                },
                {
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00,
                        0x00, 0x1F, 0x00, 0x00, 0x00, 0x80, 0x1F, 0x00,
                        0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0xC0,
                        0x3F, 0x00, 0x00, 0x00, 0xC0, 0x3F, 0x00, 0x00,
                        0x00, 0xC0, 0x7F, 0x00, 0x00, 0x00, 0xE0, 0xFF,
                        0x00, 0x00, 0x00, 0xF0, 0xFF, 0x01, 0x00, 0x00,
                        0xFC, 0xFF, 0x03, 0x00, 0xE0, 0xFF, 0xFF, 0xFF,
                        0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0x07, 0xFE, 0xFF,
                        0xFF, 0xFF, 0x07, 0xFC, 0xFF, 0xFF, 0xFF, 0x07,
                        0xFC, 0xFF, 0xFF, 0xFF, 0x03, 0xF8, 0xFF, 0xFF,
                        0xFF, 0x01, 0xF0, 0xFF, 0xFF, 0xFF, 0x00, 0xE0,
                        0xFF, 0xFF, 0x7F, 0x00, 0xC0, 0xFF, 0xFF, 0x3F,
                        0x00, 0x80, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0xFF,
                        0xFF, 0x1F, 0x00, 0x00, 0xFF, 0xFF, 0x1F, 0x00,
                        0x00, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0xFF, 0xFF,
                        0x0F, 0x00, 0x00, 0xFF, 0xFF, 0x0F, 0x00, 0x00,
                        0xFF, 0xFF, 0x0F, 0x00, 0x00, 0xFF, 0xFF, 0x0F,
                        0x00, 0x00, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0xFF,
                        0xFF, 0x1F, 0x00, 0x80, 0xFF, 0xF0, 0x1F, 0x00,
                        0x80, 0x3F, 0xC0, 0x1F, 0x00, 0x80, 0x1F, 0x00,
                        0x1F, 0x00, 0x00, 0x07, 0x00, 0x1C, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00/*"C:\Users\ROG\Desktop\三连\收藏.bmp",0*/
                        /* (36 X 37 )*/
                },
                {
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x1F,
                        0x00, 0x00, 0x00, 0xF0, 0xFF, 0x01, 0x00, 0x00,
                        0xFC, 0xFF, 0x07, 0x00, 0x00, 0xFF, 0xFF, 0x0F,
                        0x00, 0x80, 0xFF, 0xFF, 0x1F, 0x00, 0xC0, 0xFF,
                        0xFF, 0x7F, 0x00, 0xE0, 0x07, 0x00, 0x7C, 0x00,
                        0xF0, 0x03, 0x00, 0xFC, 0x00, 0xF0, 0x03, 0x00,
                        0xFC, 0x01, 0xF8, 0xFF, 0xF1, 0xFF, 0x01, 0xF8,
                        0xFF, 0xF1, 0xFF, 0x03, 0xF8, 0x7F, 0xC0, 0xFF,
                        0x03, 0xFC, 0x1F, 0x00, 0xFF, 0x03, 0xFC, 0x07,
                        0x00, 0xFE, 0x07, 0xFC, 0x07, 0x01, 0xFC, 0x07,
                        0xFC, 0xC3, 0x31, 0xF8, 0x07, 0xFC, 0xE1, 0xF1,
                        0xF8, 0x07, 0xFC, 0xF1, 0xF1, 0xF0, 0x07, 0xFC,
                        0xF1, 0xF1, 0xF0, 0x07, 0xFC, 0xF1, 0xF1, 0xF1,
                        0x07, 0xFC, 0xF1, 0xF1, 0xF1, 0x07, 0xFC, 0xF1,
                        0xF1, 0xF1, 0x03, 0xF8, 0xF1, 0xF1, 0xF1, 0x03,
                        0xF8, 0xFF, 0xF1, 0xFF, 0x03, 0xF8, 0xFF, 0xF1,
                        0xFF, 0x01, 0xF0, 0xFF, 0xF1, 0xFF, 0x01, 0xF0,
                        0xFF, 0xF1, 0xFF, 0x00, 0xE0, 0xFF, 0xF1, 0x7F,
                        0x00, 0xC0, 0xFF, 0xFF, 0x7F, 0x00, 0x80, 0xFF,
                        0xFF, 0x3F, 0x00, 0x00, 0xFF, 0xFF, 0x0F, 0x00,
                        0x00, 0xFC, 0xFF, 0x07, 0x00, 0x00, 0xF0, 0xFF,
                        0x01, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00 /*"C:\Users\ROG\Desktop\三连\投币.bmp",0*/
                        /* (36 X 36 )*/
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

uint8_t icon_width[] = { 35,37,36,36 };

//main界面图片
PROGMEM const uint8_t LOGO[] =
        {
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x10,0x01,0x24,0x00,0x44,0x04,0x80,0x20,0x00,0x00,0x10,0xF2,0x3C,0x20,0xE0,0x00,
                0x10,0x01,0x24,0x00,0xC4,0x04,0x80,0x20,0x00,0x00,0x10,0x11,0x24,0x30,0x10,0x01,
                0x10,0x71,0x24,0x0F,0xC4,0xE2,0x99,0x3C,0x00,0x00,0x20,0x11,0x24,0x20,0x10,0x01,
                0xF0,0x89,0xA4,0x10,0xA8,0x12,0x8A,0x22,0x00,0x00,0x20,0xF1,0x1C,0x20,0x10,0x01,
                0x10,0xF9,0xA4,0x10,0x98,0x13,0x8A,0x22,0x00,0x00,0xA0,0x10,0x24,0x20,0x10,0x01,
                0x10,0x09,0xA4,0x10,0x98,0x11,0x8A,0x22,0x00,0x00,0xA0,0x10,0x24,0x20,0x10,0x01,
                0x10,0xF1,0x24,0x0F,0x10,0xE1,0x89,0x3C,0x00,0x00,0x40,0xF0,0x44,0x20,0xE2,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,
                0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFF,0xFF,0xE7,0xFF,0xFB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0x0F,0xFE,0x99,0xFF,0xE4,0x1F,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xE7,0xFD,0xBD,0xFF,0xDE,0xDF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,0xFE,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xDF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xEF,0xFD,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xDF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xEF,0xFD,
                0xF7,0xFD,0x7D,0x7F,0xDF,0x1F,0xF0,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xEF,0xFD,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xFF,0xF7,0xFD,0xFF,0x1F,0xFE,0xFF,0x83,0xFF,0xE3,0xF1,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xFF,0xF7,0xFD,0xFF,0xE3,0xF1,0xFF,0x7C,0xF8,0xF9,0xEF,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xFF,0xF7,0xFD,0xFF,0xFD,0xEF,0x3F,0xFF,0xF3,0xFD,0xDF,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xFF,0xF7,0xFD,0xFF,0x1E,0xDC,0xBF,0x03,0xF7,0xE3,0xE1,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xFF,0xF7,0xFD,0x7F,0xEF,0xB9,0xDF,0xFB,0xF0,0xEF,0xFD,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xFF,0xF7,0xFD,0x7F,0xE7,0xBB,0xDF,0xF3,0xFF,0xEF,0xFD,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xFF,0xF7,0xFD,0xBF,0xF7,0x7B,0xDF,0x0F,0xFF,0xEF,0xFD,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xFF,0xF7,0xFD,0xBF,0x0F,0x7C,0xBF,0xFF,0xFC,0xEF,0xFD,
                0xF7,0xFD,0x7D,0x7F,0xDF,0xFF,0xF7,0xFD,0xBF,0xFF,0xBF,0x7F,0xFE,0xF3,0xEF,0xFD,
                0xF7,0xFD,0xBE,0x7F,0xDF,0xFF,0xF7,0xFD,0xBF,0x07,0xC0,0xFF,0xE1,0xF7,0xEF,0xFD,
                0xEF,0xFB,0xBE,0x7F,0xDF,0xFF,0xF7,0xFD,0xBF,0xEF,0xFF,0xFF,0x9F,0xEF,0xEF,0xFD,
                0xEF,0x07,0xBF,0x7F,0xDF,0xFF,0xF7,0xFD,0x7F,0xEF,0xC7,0x3F,0x3E,0xEF,0xEF,0xFD,
                0xDF,0xFF,0xDF,0x7F,0xDF,0xFF,0xF7,0xFD,0xFF,0x1E,0xB8,0xDF,0xB9,0xF7,0xEF,0xE3,
                0x9F,0xFF,0xEF,0x7F,0xDF,0xFF,0xF7,0xFD,0xFF,0xFC,0xBF,0x9F,0xC7,0xF7,0xDF,0xCF,
                0x7F,0xFF,0xF3,0xFF,0xDE,0xFF,0xEF,0xFD,0xFF,0xFB,0xCF,0x3F,0xFF,0xF9,0x9F,0xEF,
                0xFF,0x00,0xFC,0xFF,0xE1,0xFF,0x1F,0xFE,0xFF,0x07,0xF0,0xFF,0x00,0xFE,0x7F,0xF0,
                0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,
                0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x08,0x08,0x00,0x40,0x00,0x02,0x10,0x00,0x00,
                0x00,0x00,0x88,0x00,0x00,0x00,0x00,0x08,0x08,0x00,0x60,0x00,0x02,0x00,0x00,0x00,
                0x00,0x00,0x88,0x9C,0x24,0xC7,0x1C,0x0F,0x68,0x22,0xA0,0xCC,0x23,0x91,0xC7,0x01,
                0x00,0xFE,0x88,0xA2,0xAA,0x48,0xA2,0x08,0x98,0x14,0xB0,0x24,0x22,0x91,0x24,0x02,
                0x00,0x00,0x78,0xA2,0xAB,0x4F,0xBE,0x08,0x88,0x14,0xF0,0x24,0x22,0x91,0x24,0x02,
                0x00,0x00,0x08,0x22,0x9B,0x40,0x82,0x08,0x88,0x0C,0x08,0x25,0x22,0x91,0x24,0x02,
                0x00,0x00,0x08,0x1C,0x11,0x4F,0x3C,0x0F,0x78,0x08,0x08,0xC5,0xE3,0x91,0xC4,0x01,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
        };

const float PID_MAX = 10.00;//PID最大允许值
//PID变量
float Kpid[3] = { 9.97,0.2,0.01 };//Kp,Ki,Kd
// float Kp=8.96;
// float Ki=0.2;
// float Kd=0.01;


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

SELECT_LIST* list = NULL; // 初始化结构体数组指针

SELECT_LIST* pid = NULL; // 初始化结构体数组指针


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
                {"Main"},
                {"Authenticator"},
                {"Edit"},
                {"Settings"},
        };

//设备名称
char  name[] = "Hello World ";
//允许名字的最大长度
const uint8_t name_len = 12;//0-11for name  12 for return
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



/**************************界面显示*******************************/

void logo_ui_show()//显示logo
{
    u8g2.drawXBMP(0, 0, 128, 64, LOGO);

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
        angle_last = 20.00 + (float)analogRead(READ) / 100.00;
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
    angle = 20.00 + (float)analogRead(READ) / 100.00;
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
    u8g2.drawStr((128 - u8g2.getStrWidth("--Text Editor--")) / 2, 20, "--Text Editor--");
    u8g2.drawStr(10, 38, name);
    u8g2.drawStr(80, 50, "-Return");

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
        u8g2.drawRBox(78, 38, u8g2.getStrWidth("-Return") + 4, 16, 1);
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

/**************************界面处理*******************************/

void logo_proc()//logo界面处理函数
{
    if (key_msg.pressed)
    {
        key_msg.pressed = false;
        ui_state = S_DISAPPEAR;
        ui_index = M_ICON;
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
                else
                {
                    UserIn=pid[pid_select].select;
                    addPassword(SiteIn,UserIn);
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
                    default:
                        SiteIn=list[ui_select].select;
                        if(justonece==0) {//防止重复向数组写入导致内存浪费溢出
                            addUser(list[ui_select].select);
                            justonece+=1;
                        }
                        pid_box_width = pid_box_width_trg = u8g2.getStrWidth(pid[pid_select].select) + x * 2;//两边各多2
                        ui_state = S_DISAPPEAR;
                        ui_index = M_PID;
                        break;
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
                    case 1:ui_state = S_DISAPPEAR;
                        ui_index = M_SELECT;
                        break;
                    case 2:ui_state = S_DISAPPEAR;
                        ui_index = M_PID;
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
                if (edit_index == name_len)
                {
                    ui_state = S_DISAPPEAR;
                    ui_index = M_SELECT;
                    edit_index = 0;
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
int compare(const void *a, const void *b) {
    return strcmp(((SELECT_LIST*)a)->select, ((SELECT_LIST*)b)->select);
}
void addUser(char* mainnowdisplay){
    Serial.println("Btn2----------------------");
    Serial.println(mainnowdisplay);
    String filePath = String("/") + String(mainnowdisplay);
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
    qsort(pid, pidSize, sizeof(SELECT_LIST), compare);

    // 在串口上打印按照字母顺序排序后的结果
    for (int i = 0; i < pidSize; i++) {
        Serial.println(pid[i].select);
    }
    pid[num].select =strdup("back");
  //  pid[4].select =strdup("keyVal");
    Serial.println( "pid[3].select");
//    Serial.println( pid[2].select);
 //   Serial.println( pid[3].select);
}

void addPassword(char* mainsite,char* mainuser){


    String filePath = String("/") + String(mainsite);
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
            if (filename.endsWith(".json")) {
                count++;
            }
        }

        entry.close();
    }

    root.close();
   listSize=count+1;
}
void addSiteDataToArr() {//此函数提供方法使其全部网站保存在数组中，界面分类用
    Serial.println("read JSON Start:");
    File root = SD.open("/");
    int index = 0; // 用于追踪数组中的索引位置
    list = (SELECT_LIST*)malloc(listSize * sizeof(SELECT_LIST));
    int fileCount = 0;//此值传向list 数组长度
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            // 没有更多文件
            break;
        }

        if (!entry.isDirectory() && entry.name()[0] != '.') {
            const char* filename = entry.name();
            if (strstr(filename, ".json") != NULL) {
                list[index+1].select = strdup(filename);
                index++;
            }
        }

        entry.close();
    }

}
void setup() {
    Serial.begin(115200);


    if (!SD.begin(7)) {
        Serial.println("SD 卡初始化失败！");
        return;
    }
    allcount();
    // 写入数据到 data.json 文件
   // writeJSONToFile();

    // 读取并打印 SD 卡上的所有 JSON 文件
  //  readAndPrintAllJSONFiles();
   // readJSONFile() ;
    addSiteDataToArr();

    list[0].select = strdup("Main");
  //  pid = (SELECT_LIST*)malloc(pidSize * sizeof(SELECT_LIST));
  //  pid[0].select = strdup("main");
    // 动态分配内存以存储结构体数组

    qsort(list + 1, listSize - 1, sizeof(SELECT_LIST), compare);

    // 在串口上打印按照字母顺序排序后的结果
    for (int i = 0; i < listSize; i++) {
        Serial.println(list[i].select);
    }

    //Wire.begin(21,22,400000);
    pinMode(BTN0, INPUT_PULLUP);
    pinMode(BTN1, INPUT_PULLUP);
    pinMode(BTN2, INPUT_PULLUP);
    key_init();

    u8g2.setBusClock(800000);
    u8g2.begin();
    u8g2.setFont(u8g2_font_wqy12_t_chinese1);
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

    box_width = box_width_trg = u8g2.getStrWidth(list[ui_select].select) + x * 2;//两边各多2


    ui_index = M_LOGO;
    //ui_index=M_TEXT_EDIT;
    ui_state = S_NONE;

}

void loop() {
    key_scan();
    ui_proc();
}