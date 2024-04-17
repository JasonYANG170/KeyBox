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
#include "SD.h"
//配置数据结构
String keySize="";
const int siteSize = 5;
String site[siteSize];
struct key {
    String user;
    String password;
};

const char* data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    Serial.printf("%s: ", (const char*)data);
    for (i = 0; i<argc; i++){
        // 初始化一个空的字符串
        String outputString = "";

// 使用sprintf将输出格式化
        char buffer[50];
        sprintf(buffer, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

// 将格式化后的字符串连接到outputString
        outputString += buffer;

// 现在outputString包含了格式化后的字符串
        Serial.println(outputString);
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

void setup() {
    Serial.begin(115200);
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

    rc = db_exec(db1, "SELECT * FROM key;");
    if (rc != SQLITE_OK) {
        sqlite3_close(db1);

        return;
    }


    sqlite3_close(db1);


}

void loop() {
}