/*
    This creates two empty databases, populates values, and retrieves them back
    from the LITTLEFS file system
*/
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <SPI.h>
#include <FS.h>
#include "LittleFS.h"
#include <Arduino.h>

/* You only need to format LITTLEFS the first time you run a
   test or else use the LITTLEFS plugin to create a partition
   https://github.com/lorol/arduino-esp32fs-plugin/releases */
#define FORMAT_LITTLEFS_IF_FAILED true

const char* data = "Callback function called";
static int callback(void *data, int argc, char **argv, char **azColName) {
   int i;
   Serial.printf("%s: ", (const char*)data);
   for (i = 0; i<argc; i++){
       Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   Serial.printf("\n");
   return 0;
}

int db_open(const char *filename, sqlite3 **db) {
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

sqlite3 *db1;
sqlite3 *db2;

void setup() {

   Serial.begin(115200);

   int rc;

   if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED, "/littlefs")) {
       Serial.println("Failed to mount file system");
       return;
   }

   // list LITTLEFS contents
   File root = LittleFS.open("/");
   if (!root) {
       Serial.println("- failed to open directory");
       return;
   }
   if (!root.isDirectory()) {
       Serial.println(" - not a directory");
       return;
   }
   File file = root.openNextFile();
   while (file) {
       if (file.isDirectory()) {
           Serial.print("  DIR : ");
           Serial.println(file.name());
       } else {
           Serial.print("  FILE: ");
           Serial.print(file.name());
           Serial.print("\tSIZE: ");
           Serial.println(file.size());
       }
       file = root.openNextFile();
   }

   // remove existing file
   LittleFS.remove("/test1.db");
   LittleFS.remove("/test2.db");

   sqlite3_initialize();

   if (db_open("/littlefs/test1.db", &db1))
       return;
   if (db_open("/littlefs/test2.db", &db2))
       return;

   rc = db_exec(db1, "CREATE TABLE test1 (id INTEGER PRIMARY KEY, content);");
   if (rc != SQLITE_OK) {
       sqlite3_close(db1);
       sqlite3_close(db2);
       return;
   }
   rc = db_exec(db2, "CREATE TABLE test2 (id INTEGER, content);");
   if (rc != SQLITE_OK) {
       sqlite3_close(db1);
       sqlite3_close(db2);
       return;
   }

   // Begin transaction
    rc = db_exec(db1, "BEGIN TRANSACTION;");
    if (rc != SQLITE_OK) {
        sqlite3_close(db1);
        return;
    }

    for (int i = 0; i < 500; i++) {
        char sql[100];
        sprintf(sql, "INSERT INTO test1 (content) VALUES ('Entry number %d');", i);
        rc = db_exec(db1, sql);
        if (rc != SQLITE_OK) {
            sqlite3_close(db1);
            return;
        }
    }

    // Commit transaction
    rc = db_exec(db1, "END TRANSACTION;");
    if (rc != SQLITE_OK) {
        sqlite3_close(db1);
        return;
    }



  //  sqlite3_close(db1);
  //  sqlite3_close(db2);

}

void loop() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim(); // Remove any leading/trailing whitespace

        if (input == "select test1") {
            Serial.println("Selecting data from test1...");
            int rc = db_exec(db1, "SELECT * FROM test1 LIMIT 100");
            if (rc != SQLITE_OK) {
                Serial.println("Failed to select data from test1");
            }
        } else if (input == "select test2") {
            Serial.println("Selecting data from test2...");
            int rc = db_exec(db2, "SELECT * FROM test2");
            if (rc != SQLITE_OK) {
                Serial.println("Failed to select data from test2");
            }
        } else {
            Serial.println("Unknown command.");
        }
    }
}