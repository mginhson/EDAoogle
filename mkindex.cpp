/**
 * @file mkindex.cpp
 * @author Marc S. Ressl
 * @brief Makes a database index
 * @version 0.3
 *
 * @copyright Copyright (c) 2022-2024 Marc S. Ressl.
 */


#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sqlite3.h>
#include <sqlite3-vcpkg-config.h>
#include <regex>

using namespace std;

static int onDatabaseEntry(void *userdata,
                           int argc,
                           char **argv,
                           char **azColName)
{
    cout << "--- Entry" << endl;
    for (int i = 0; i < argc; i++)
    {
        if (argv[i])
            cout << azColName[i] << ": " << argv[i] << endl;
        else
            cout << azColName[i] << ": " << "NULL" << endl;
    }

    return 0;
}

int main(int argc,
         const char *argv[])
{
    char *databaseFile = "index.db";
    sqlite3 *database;
    char *databaseErrorMessage;
    string databaseName = "prettyEyes";
    string placeholder;
    
    if (sqlite3_open(databaseFile, &database) != SQLITE_OK)
    {
        cout << "Failure on sqlite_open()" << endl;
        return 1;
    }
    
    placeholder = "CREATE VIRTUAL TABLE " + databaseName + 
    " USING fts5(title, path, body);";

    if (sqlite3_exec(database,
        placeholder.c_str(),
        NULL,
        0,
        &databaseErrorMessage) != SQLITE_OK)
    {
        cout << sqlite3_errmsg(database) << endl;
    }

    
    cout << "Deleting previous entries from the table." << endl;
    placeholder = "DELETE FROM " + databaseName;
    if (sqlite3_exec(database,
                     placeholder.c_str(),
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
    {
         cout << "Error: " << sqlite3_errmsg(database) << endl;
    }   

    

    std::ifstream file;
    std::string pathName = "/home/mginhson/Desktop/EDA/EDAoogle/www/wiki"; 
    std::string reader, body, fileName;


    for (const auto & entry : filesystem::directory_iterator(pathName))
    {
        reader.clear();
        body.clear();

       
        fileName = entry.path();
        
        cout << fileName << endl;
        file.open(entry.path());
        
        
        
        while(!file.eof())
        {
            getline(file,reader,'<');
            body.append(reader);
            getline(file,reader,'>');
        }
        std::regex pattern1("&#[0-9]{3};");
        body = std::regex_replace(body, pattern1, "");

        std::regex pattern2("&#[0-9]{4};");
        body = std::regex_replace(body, pattern2, "");

        std::regex regexToRemove1(std::string(1, '\''));

        // Usar std::regex_replace para reemplazar las coincidencias con una cadena vacía
        body = std::regex_replace(body, regexToRemove1, "");
        //body.erase(std::remove(body.begin(), body.end(), '\''), body.end());
        //body.erase(std::remove(body.begin(), body.end(), '\"'), body.end());
        std::regex regexToRemove2(std::string(1, '\"'));
        body = std::regex_replace(body, regexToRemove2, "");

        cout << body << endl;
        string delim = " ";
        delim[0]='"';
        placeholder = "INSERT INTO " + databaseName + " (title, path, body)" + 
        " VALUES("  +delim +fileName.substr(fileName.find_last_of('/')+1)+delim + ", " +
        delim + entry.path().c_str()+ delim + " ," + "'" +body+ delim + " );";
        
        
        
        if (sqlite3_exec(database,
            placeholder.c_str(), 
            NULL, 0, &databaseErrorMessage) != SQLITE_OK)
        {
            cout << "Error while inserting tokens into the table" << endl;
            cout << databaseErrorMessage << endl;
            return 1;
        }
        
        
        file.close();
        
    }
    
    

  
    /*
    // Open database file
    cout << "Opening database..." << endl;
    if (sqlite3_open(databaseFile, &database) != SQLITE_OK)
    {
        cout << "Can't open database: " << sqlite3_errmsg(database) << endl;

        return 1;
    }

    // Create a sample table
    cout << "Creating table..." << endl;
    if (sqlite3_exec(database,
                     "CREATE TABLE room_occupation "
                     "(id INTEGER PRIMARY KEY,"
                     "room varchar DEFAULT NULL,"
                     "reserved_from DATETIME,"
                     "reserved_until DATETIME);",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
        cout << "Error: " << sqlite3_errmsg(database) << endl;

    // Delete previous entries if table already existed
    cout << "Deleting previous entries..." << endl;
    if (sqlite3_exec(database,
                     "DELETE FROM room_occupation;",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
        cout << "Error: " << sqlite3_errmsg(database) << endl;

    // Create sample entries
    cout << "Creating sample entries..." << endl;
    if (sqlite3_exec(database,
                     "INSERT INTO room_occupation (room, reserved_from, reserved_until) VALUES "
                     "('401F','2022-03-03 15:00:00','2022-03-03 16:00:00');",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
        cout << "Error: " << sqlite3_errmsg(database) << endl;
    if (sqlite3_exec(database,
                     "INSERT INTO room_occupation (room, reserved_from, reserved_until) VALUES "
                     "('501F','2022-03-03 15:00:00','2022-03-03 17:00:00');",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
        cout << "Error: " << sqlite3_errmsg(database) << endl;
    if (sqlite3_exec(database,
                     "INSERT INTO room_occupation (room, reserved_from, reserved_until) VALUES "
                     "('001R','2022-03-03 15:00:00','2022-03-03 16:30:00');",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
        cout << "Error: " << sqlite3_errmsg(database) << endl;
    if (sqlite3_exec(database,
                     "INSERT INTO room_occupation (room, reserved_from, reserved_until) VALUES "
                     "('1001F','2022-03-03 15:30:00','2022-03-03 16:30:00');",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
        cout << "Error: " << sqlite3_errmsg(database) << endl;

    // Fetch entries
    cout << "Fetching entries..." << endl;
    if (sqlite3_exec(database,
                     "SELECT * from room_occupation;",
                     onDatabaseEntry,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
        cout << "Error: " << sqlite3_errmsg(database) << endl;

    // Close database
    cout << "Closing database..." << endl;
    sqlite3_close(database);

    */
}

