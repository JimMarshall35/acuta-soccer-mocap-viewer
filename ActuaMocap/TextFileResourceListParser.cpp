#include "TextFileResourceListParser.h"
#include <fstream>
#include <iostream>

void ParseResourcesTextFile(std::string filePath, ParseResourcesTextFileCallback callback)
{
	using namespace std;
    ifstream spritesFile(filePath);
    string line;
    int onLine = 0;
    while (getline(spritesFile, line)) {
        onLine++;
        const auto lineBufferSize = 300;
        char lineCopy[lineBufferSize];
        strcpy_s(lineCopy, line.c_str());
        rsize_t strMax = line.length();
        char* next_token;
        char* ptr = strtok_s(lineCopy, " ", &next_token);
        std::string pathAndIdentifierName[2];
        int numOnThisLine = 0;
        bool errored = false;
        while (ptr != NULL) {
            pathAndIdentifierName[numOnThisLine] = string(ptr);
            if (++numOnThisLine > 2) {
                std::cerr << "more than two space separated entries on line " << onLine << '\n';
                errored = true;
                break;
            }
            ptr = strtok_s(NULL, " ", &next_token);
        }
        if (numOnThisLine < 2) {
            std::cerr << "less than two space separated entries on line " << onLine << '\n';
            errored = true;
        }

        if (errored) {
            // no point in trying to load a wrongly written line
            continue;
        }

        callback(pathAndIdentifierName[0], pathAndIdentifierName[1]);
    }

}
