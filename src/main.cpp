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

void addSiteDataToArr() {
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
    std::string OutPutString = "id = 1\nsite = example_site_2\nuser = user1\npassword = password1\nid = 2\nsite = example_site_1\nuser = user3\npassword = password1\nid = 3\nsite = example.com\nuser = john_doe\npassword = password123";

    std::istringstream ss(OutPutString);
    std::string line;

    while (std::getline(ss, line, '\n')) {
        // Split the line based on '='
        size_t found = line.find('=');
        if (found != std::string::npos) {
            std::string key = line.substr(0, found);
            std::string value = line.substr(found + 1);

            // Trim any leading/trailing spaces
            key.erase(0, key.find_first_not_of(" \n\r\t"));
            key.erase(key.find_last_not_of(" \n\r\t") + 1);
            value.erase(0, value.find_first_not_of(" \n\r\t"));
            value.erase(value.find_last_not_of(" \n\r\t") + 1);
            const char* cString = key.c_str();

            // 输出C风格的字符串
            Serial.println(cString);
            Serial.println("+");
            const char* cString2 = value.c_str();

            // 输出C风格的字符串
            Serial.println(cString2);
            //std::cout << key << " = " << value << std::endl;
        }
    }

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