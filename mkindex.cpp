/**
 * @file mkindex.cpp
 * @author Marc S. Ressl
 * @modifiers Sosa, Mateo and Ginhson, Matteo
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

//USER SHOULD MODIFY THIS!!!!
std::string pathName = "C:\\Users\\mateo\\Desktop\\level5\\EDAoogle\\www\\wiki";

int main(int argc,
    const char* argv[])
{
    char* databaseFile = "index.db";
    sqlite3* database;
    char* databaseErrorMessage;
    string databaseName = "prettyEyes";
    string placeholder;

    //First, open the database.
    if (sqlite3_open(databaseFile, &database) != SQLITE_OK)
    {
        cout << "Failure on sqlite_open()" << endl;
        return 1;
    }
    
    /**
    * We create the virtual table using the command on the Placeholder.
    * If it was already created, we delete all the previous entries.
    */
    placeholder = "CREATE VIRTUAL TABLE " + databaseName +
        " USING fts5(title, path, body);";


    if (sqlite3_exec(database,
        placeholder.c_str(),
        NULL,
        0,
        &databaseErrorMessage) != SQLITE_OK)
    {
        cout << sqlite3_errmsg(database) << endl;

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
    }

    
    



    std::ifstream file;
    
    std::string reader, body, fileName, filePath;

    /**
    * For every file on the directory, read the whole body, extracting the HTML characters, "<>".
    * Then, with Regular Expressions, delete the special characters, that were parsed
    * with the format "&#   ;". After that, again with Regular Expressions, strip the 
    * single and double quotes, since they are interpreted as Command starts on any string passed
    * to SQLite3. 
    * Finally, concatenate everything into a single Command string, 
    */
    for (const auto& entry : filesystem::directory_iterator(pathName))
    {
        reader.clear();
        body.clear();


        filePath = entry.path().string();


        fileName = filePath.substr(filePath.find_last_of('\\') + 1);
        fileName.erase(fileName.find_last_of('.'));
        
        regex guionBajo("_");
        fileName = regex_replace(fileName, guionBajo, " ");
        file.open(entry.path());
        
        
        //Extract while stripping the HTML bodies.      
        while(!file.eof())
        {
            getline(file,reader,'<');
            body.append(reader);
            getline(file,reader,'>');
        }

        // Eliminación de caractéres UTF mediante regex ()
        std::regex pattern1("&#[0-9]{3};");
        body = std::regex_replace(body, pattern1, "");

        std::regex pattern2("&#[0-9]{4};");
        body = std::regex_replace(body, pattern2, "");

        std::regex regexToRemove1(std::string(1, '\''));

        
        body = std::regex_replace(body, regexToRemove1, "");
    
        std::regex regexToRemove2(std::string(1, '\"'));
        body = std::regex_replace(body, regexToRemove2, "");

        
        cout << fileName << endl;
        string delim = " ";
        delim[0]='"';
        placeholder = "INSERT INTO " + databaseName + " (title, path, body)" + 
        " VALUES( "  +delim +fileName +delim + ", " +
        delim + filePath + delim + " , " + delim + body + delim + " );";
        
        
        
        if (sqlite3_exec(database,
            placeholder.c_str(), 
            NULL, 0, &databaseErrorMessage) != SQLITE_OK)
        {
            cout << "Error while inserting tokens into the table" << endl;
            cout << databaseErrorMessage << endl;
            
        }
       
        
        
        file.close();
        
    }
    
    

  
    cout << "Terminamos de crear la base de datos";
}

