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
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sstream>
#include <vector>
#include "SD.h"
//配置数据结构
using namespace std;
int OutPutTimes;

String OutPutString = "";
int keySize;

struct key {
    String user;
    String password;
};

const char* data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    Serial.printf("%s: ", (const char*)data);
    OutPutTimes++;

    Serial.println("----------------------");
    for (i = 0; i<argc; i++){
        // 初始化一个空的字符串


// 使用sprintf将输出格式化
       char buffer[50];
       sprintf(buffer, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        OutPutString+=buffer;
// 将格式化后的字符串连接到OutPutString
     //   OutPutString = const_cast<char *>( "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

   //     Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
// 现在OutPutString包含了格式化后的字符串

    }

    Serial.printf("\n");
    return 0;
}

int openDb(const char *filename, sqlite3 **db) {
    int rc = sqlite3_open(filename, db);
    if (rc) {
        Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    } else {
        Serial.printf("Opened database successfully\n");
    }
    return rc;
}

char *zErrMsg = 0;
int db_exec(sqlite3 *db, const char *sql) {
    Serial.println(sql);
    long start = micros();
    int rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        Serial.printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        Serial.printf("Operation done successfully\n");
    }
    Serial.print(F("Time taken:"));
    Serial.println(micros()-start);

    return rc;
}
void removeDuplicates(String arr[], int& n) {//此函数提供方法删除数组内重复的值
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n;) {
            if (arr[i] == arr[j]) {
                for (int k = j; k < n - 1; k++) {
                    arr[k] = arr[k + 1];
                }
                n--;
            } else {
                j++;
            }
        }
    }
}
void addSiteDataToArr() {//此函数提供方法使其全部网站保存在数组中，界面分类用
    sqlite3 *db1;

    char *zErrMsg = 0;
    int rc;

    SPI.begin();
    SD.begin();

    sqlite3_initialize();

    // Open database 1
    if (openDb("/sd/key.db", &db1))
        return;
    rc = db_exec(db1, "SELECT * FROM site");
    if (rc != SQLITE_OK) {
        sqlite3_close(db1);

        return;
    }
    Serial.println(OutPutString);

    Serial.println("All Have Data Times:");
    const int SiteSize = OutPutTimes;

    String Site[SiteSize];//根据数据长度确认数组长度
   // std::string OutPutString = "id = 1\nsite = example_site_2\nuser = user1\npassword = password1\nid = 2\nsite = example_site_1\nuser = user3\npassword = password1\nid = 3\nsite = example.com\nuser = john_doe\npassword = password123";
    String inputString = OutPutString; // 输入的字符串
    int startIndex = 0;
    int endIndex = 0;

        for(int i=0;i<OutPutTimes;i++) {
            startIndex = inputString.indexOf("=", endIndex);
            if (startIndex == -1) {
                break; // 如果没有找到等号，退出循环
            }

            endIndex = inputString.indexOf("\n", startIndex); // 查找换行符的位置

            if (endIndex == -1) {
                endIndex = inputString.length(); // 如果没有找到换行符，则使用字符串的末尾作为终点
            }

            String value = inputString.substring(startIndex + 2, endIndex); // 提取等号后面的内容，包括空格

            Serial.println(value);
            Site[i] = value;

        }
    removeDuplicates(Site, OutPutTimes);
    Serial.println("--------分割-----------");
    for (int i = 0; i < OutPutTimes; i++) {
        Serial.println(Site[i]);
    }

\
    //   OutPutString.
//
//    char *token;
//
//    token = strtok( OutPutString, "\n");
//    for (int i = 0; i < OutPutTimes; i++) {
//        char *value = strchr(token, '=');
//
//        if (value != NULL) {
//            value++; // 移动到等号后面的值
//            Site[i] = value;
//        }
//
//        token = strtok(NULL, "\n");
//    }




    Serial.println(OutPutTimes);
    //初始化
    OutPutString = "";
    OutPutTimes = 0;
    sqlite3_close(db1);
}
void setup() {
    Serial.begin(115200);
    addSiteDataToArr();

    sqlite3 *db1;

    char *zErrMsg = 0;
    int rc;

    SPI.begin();
    SD.begin();

    sqlite3_initialize();

    // Open database 1
    if (openDb("/sd/key.db", &db1))
        return;

    rc = db_exec(db1, "SELECT * FROM key;");
    if (rc != SQLITE_OK) {
        sqlite3_close(db1);

        return;
    }
    Serial.println(OutPutString);
    OutPutString="";
    Serial.println("All Have Data Times2:");
    Serial.println(OutPutTimes);

    sqlite3_close(db1);


}

void loop() {
}