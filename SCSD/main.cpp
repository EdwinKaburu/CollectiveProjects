/**
 * @file main.cpp
 * @author Edwin Kaburu
 * @date 3/8/2023
 */

#include <iostream>
#include <limits>
#include <vector>
#include <fstream>
#include "FunctionExtrapolation.h"

using namespace std;

enum SmellCommands {
    DISP_FUNCS, DISP_LONG_FUNCS, DISP_LONG_PARAMS, DISP_DUP_FUNCS
};

/**
 * readFile() File Main Source File and Save Data To String Vector
 *
 * @param filePath string
 * @param linesOfCodes vector<string>
 * @return Boolean
 */
bool readFile(const string &filePath, vector<string> &linesOfCodes) {
    ifstream inputFile(filePath);
    if (inputFile) {
        cout << "File is Open\n";
        string line;
        while (getline(inputFile, line)) {
            if (!line.empty()) {
                line = spaceStrip(line);
                linesOfCodes.push_back(line);
            }
        }

        inputFile.close();
        return true;
    }
    cout << "File Does Not Exists\n";
    return false;
}

/**
 * displayCommands() Print All Available Commands
 */
void displayCommands() {
    cout << "Enter Num: 0 -> Display Functions\n"
            "Enter Num: 1 -> Display Long Functions\n"
            "Enter Num: 2 -> Display Long Parameters List\n"
            "Enter Num: 3 -> Display Duplicate Functions\n"
            "Enter Num: 4 -> Exit\n";
}

/**
 * userCommandsHandler() Handle User Commands Key Binding
 *
 * @param command SmellCommands
 * @param functionExtrapolation FunctionExtrapolation
 * @return Boolean
 */
bool userCommandsHandler(SmellCommands command, FunctionExtrapolation &functionExtrapolation) {
    switch (command) {
        case DISP_FUNCS:
            functionExtrapolation.printFuncExpressions();
            break;
        case DISP_LONG_FUNCS:
            functionExtrapolation.printLongFunction();
            break;
        case DISP_LONG_PARAMS:
            functionExtrapolation.printLongParams();
            break;
        case DISP_DUP_FUNCS:
            functionExtrapolation.printDuplicateFunc();
            break;
        default:
            return false;
    }
    return true;
}

/**
 * main() Main Execution
 * @return Int
 */
int main() {
    string filePath;

    cout << "Enter Source File Path Location:\n";
    getline(cin, filePath);

    cout << "Attempt To Read File At: " << filePath << "\n";

    vector<string> fileSourceCode = {};
    bool readFileStatus = readFile(filePath, fileSourceCode);

    if (readFileStatus) {

        bool contListening = true;
        bool actionCompleted = true;
        int userCommands;

        cout << "Read File Success\n";
        FunctionExtrapolation functionExtrapolation(fileSourceCode);

        while (contListening) {
            if (actionCompleted) {
                cout << "\n---------------COMMANDS---------------\n";
                displayCommands();
                cout << "--------------------------------------\n";
                actionCompleted = false;
            }

            cout << "What Are Your Commands:\n";
            cin >> userCommands;

            if (!cin.fail()) {
                cout << "Your Commands is: " << userCommands << "\n";
                contListening = userCommandsHandler(static_cast<SmellCommands>(userCommands), functionExtrapolation);
                actionCompleted = true;
            } else {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Wrong Command Binding !!!\n";
            }
        }
    }
    cout << "\nEnd Of Program";
    return 0;
}