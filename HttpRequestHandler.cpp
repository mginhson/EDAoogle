/**
 * @file HttpRequestHandler.h
 * @author Marc S. Ressl
 * @brief EDAoggle search engine
 * @version 0.3
 *
 * @copyright Copyright (c) 2022-2024 Marc S. Ressl
 */

#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include "HttpRequestHandler.h"
#include <sqlite3.h>
#include <regex>
using namespace std;


int selectPages(void* pointer, int columnSize, char** columns, char**columnsName) {
    //for (int i = 0; i < columnSize; i++) {
       // cout << columns[0];
        static_cast<vector<string>*>(pointer)->push_back(columns[0]);
    
    return 0;
}


HttpRequestHandler::HttpRequestHandler(string homePath)
{
    this->homePath = homePath;
}

/**
 * @brief Serves a webpage from file
 *
 * @param url The URL
 * @param response The HTTP response
 * @return true URL valid
 * @return false URL invalid
 */
bool HttpRequestHandler::serve(string url, vector<char> &response)
{
    // Blocks directory traversal
    // e.g. https://www.example.com/show_file.php?file=../../MyFile
    // * Builds absolute local path from url
    // * Checks if absolute local path is within home path
    auto homeAbsolutePath = filesystem::absolute(homePath);
    auto relativePath = homeAbsolutePath / url.substr(1);
    string path = filesystem::absolute(relativePath.make_preferred()).string();

    if (path.substr(0, homeAbsolutePath.string().size()) != homeAbsolutePath)
        return false;

    // Serves file
    ifstream file(path);
    if (file.fail())
        return false;

    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, ios::beg);

    response.resize(fileSize);
    file.read(response.data(), fileSize);

    return true;
}

bool HttpRequestHandler::handleRequest(string url,
                                               HttpArguments arguments,
                                               vector<char> &response)
{
    string searchPage = "/search";
    if (url.substr(0, searchPage.size()) == searchPage)
    {
        string searchString;
        if (arguments.find("q") != arguments.end())
            searchString = arguments["q"];

        // Header
        string responseString = string("<!DOCTYPE html>\
<html>\
\
<head>\
    <meta charset=\"utf-8\" />\
    <title>EDAoogle</title>\
    <link rel=\"preload\" href=\"https://fonts.googleapis.com\" />\
    <link rel=\"preload\" href=\"https://fonts.gstatic.com\" crossorigin />\
    <link href=\"https://fonts.googleapis.com/css2?family=Inter:wght@400;800&display=swap\" rel=\"stylesheet\" />\
    <link rel=\"preload\" href=\"../css/style.css\" />\
    <link rel=\"stylesheet\" href=\"../css/style.css\" />\
</head>\
\
<body>\
    <article class=\"edaoogle\">\
        <div class=\"title\"><a href=\"/\">EDAoogle</a></div>\
        <div class=\"search\">\
            <form action=\"/search\" method=\"get\">\
                <input type=\"text\" name=\"q\" value=\"" +
                                       searchString + "\" autofocus>\
            </form>\
        </div>\
        ");
        
        // YOUR JOB: fill in results
        float searchTime = 0.1F;
        vector<string> results;

        char* databaseFile = "index.db";
        sqlite3* database;
        char* databaseErrorMessage;
        
        // Open database file
        cout << "Opening database..." << endl;
        if (sqlite3_open(databaseFile, &database) != SQLITE_OK)
        {
            cout << "Can't open database: " << sqlite3_errmsg(database) << endl;

            return 1;
        }
        
        //std::regex espacio_regex(" ");
        //std::string cadena_final = std::regex_replace(searchString, espacio_regex, " OR ");


        auto start = chrono::system_clock::now();
        //string searchArgument = "SELECT title, url bm25(nombre_base_de_datos) AS rank FROM libros WHERE libros MATCH '" + searchString + "' ORDER BY rank;";
        string searchArgument = "SELECT title, path, bm25(prettyEyes) AS rank FROM prettyEyes WHERE body MATCH '" + searchString + "' ORDER BY rank;";

       
        vector <string> queryData;
            // pop_back();
        if(sqlite3_exec(database,
            searchArgument.c_str(),
            selectPages,
            &results,
            &databaseErrorMessage) != SQLITE_OK)
            cout << "Error: " << sqlite3_errmsg(database) << endl;
        
        auto end = chrono::system_clock::now();
        chrono::duration<long double> duration = end - start;
        searchTime = duration.count();
        // Print search results
        responseString += "<div class=\"results\">" + to_string(results.size()) +
                          " results (" + to_string(searchTime) + " seconds):</div>";
        for (auto& result : results) {
            responseString += "<div class=\"result\"><a href=\"" +
                result + "\">" + result + "</a></div>";
            cout << result;
        }

        // Trailer
        responseString += "    </article>\
</body>\
</html>";

        response.assign(responseString.begin(), responseString.end());

        return true;
    }
    else
        return serve(url, response);

    return false;
}
