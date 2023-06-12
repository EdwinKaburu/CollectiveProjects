/**
 * @file FunctionExtrapolation.cpp
 * @author Edwin Kaburu
 * @date 3/8/2023
 */

#include "FunctionExtrapolation.h"

bool FunctionExtrapolation::isValidExpression(string &expression, const set<char> &acceptableMarks) {
    vector<string> paramSplits = getStringSplits(expression, DEF_START_INDEX, expression.size());

    if (paramSplits.size() < SPLIT_SIZE_TOLERANCE) {
        return false;
    }

    for (const string &item: paramSplits) {
        bool hasPunctuation = doesContainPunctuation(item, acceptableMarks);
        if (hasPunctuation) {
            return false;
        }
    }
    return true;
}

bool FunctionExtrapolation::isValidFuncParams(string &funcParams, const set<char> &acceptableMarks) {
    int numOfRoundBrackets = 0;
    string variableWord;

    int i = 0;
    while (i < funcParams.size()) {
        char paramLetter = '\0';
        bool declStatus = true;

        paramLetter = funcParams.at(i);

        if (paramLetter == '(') {
            numOfRoundBrackets += 1;
        } else if (paramLetter == ')') {
            numOfRoundBrackets += 1;

            if (!variableWord.empty()) {
                declStatus = isValidExpression(variableWord, acceptableMarks);
                variableWord = "";
            }
        } else if (paramLetter == ',') {
            declStatus = isValidExpression(variableWord, acceptableMarks);
            variableWord = "";
        } else {
            variableWord += paramLetter;
        }

        if (!declStatus) {
            return false;
        }
        i += 1;
    }

    if (numOfRoundBrackets == 2 && variableWord.empty()) {
        return true;
    }
    return false;
}

bool FunctionExtrapolation::isValidFuncName(string &funcName) {
    return isValidExpression(funcName);
}

bool FunctionExtrapolation::extractFuncName(string &funcName, string &lineOfCode) {
    // Get
    unsigned int i = DEF_START_INDEX;
    while (i < lineOfCode.size()) {
        if (lineOfCode.at(i) == '(') {
            break;
        }
        funcName.push_back(lineOfCode.at(i));
        i += 1;
    }
    // Verify
    funcName = spaceStrip(funcName);
    return isValidFuncName(funcName);
}

bool FunctionExtrapolation::extractFuncParams(unsigned int startIndex, string &funcParams, string &lineOfCode) {
    set<char> paramsAcceptableMarks = {'=', '&', '*'};
    // GET
    unsigned int i = startIndex;
    while (i < lineOfCode.size()) {
        if (lineOfCode.at(i) == '{') {
            break;
        }
        funcParams.push_back(lineOfCode.at(i));
        i += 1;
    }
    funcParams = spaceStrip(funcParams);
    // Verify
    return isValidFuncParams(funcParams, paramsAcceptableMarks);
}

void FunctionExtrapolation::extractFuncBody(unsigned int startIndex, string &funcBody, string &lineOfCode) {
    // GET
    unsigned int i = startIndex;
    while (i < lineOfCode.size()) {
        funcBody.push_back(lineOfCode.at(i));
        i += 1;
    }
    spaceStrip(funcBody);
}

bool FunctionExtrapolation::isDuplicateCode(const FunctionExpression &funcExpress1,
                                            const FunctionExpression &funcExpress2) {

    vector<string> funCompose1 = {};
    vector<string> funCompose2 = {};

    funcExpress1.getExtrapolatedComposition(funCompose1);
    funcExpress2.getExtrapolatedComposition(funCompose2);

    double similarIndex = getJaccardSimilarIndex(funCompose1, funCompose2);

    if (similarIndex >= JACCARD_INDEX_THRESHOLD) {
        return true;
    }
    return false;
}

void FunctionExtrapolation::printFuncExpressions() {
    cout << "\n--List Of Functions From Source Code:--\n";
    for (const FunctionExpression &funcExpr: funcExpressions) {
        cout << "\t" << (funcExpr.funcName + funcExpr.funcParams) << "\n";
    }
}

void FunctionExtrapolation::printDuplicateFunc() {
    cout << "\n--Duplicate Functions Results: [INDEX-THRESHOLD " << JACCARD_INDEX_THRESHOLD << "]--\n";
    bool duplicateExists = false;
    for (int i = 0; i < funcExpressions.size() - 1; i++) {
        const FunctionExpression &function1 = funcExpressions.at(i);
        for (int j = i + 1; j < funcExpressions.size(); j++) {
            if (i != j) {
                const FunctionExpression &function2 = funcExpressions.at(j);
                bool isDuplicate = isDuplicateCode(function1, function2);
                if (isDuplicate) {
                    cout << "\t" << function1.funcName << " & " << function2.funcName << " are duplicate\n";
                    duplicateExists = true;
                }
            }
        }
    }
    if (!duplicateExists) {
        cout << "\tNo Functions are Duplicate\n";
    }
}

void FunctionExtrapolation::printLongParams() {
    cout << "\n--Long Function Parameters Results: [THRESHOLD " << FUN_PARAM_THRESHOLD << "]--\n";

    bool longParamExists = false;
    for (const FunctionExpression &funcExpr: funcExpressions) {
        vector<string> paramsList = {};
        funcExpr.extrapolateParams(paramsList);

        if (paramsList.size() > FUN_PARAM_THRESHOLD) {
            cout << "\t" << funcExpr.funcName << "\t---> Num Of Parameters: " << paramsList.size() << "\n";
            longParamExists = true;
        }
    }
    if (!longParamExists) {
        cout << "\tNo Functions With Long Parameters Exists\n";
    }
}

void FunctionExtrapolation::printLongFunction() {
    cout << "\n--Long Sized Functions Results: [THRESHOLD " << FUN_SIZE_MAX << "]--\n";
    bool longFuncExists = false;
    int funcSize = 0;
    for (const FunctionExpression &funcExpr: funcExpressions) {
        funcSize = funcExpr.endLine - funcExpr.stLine;
        if (funcSize > FUN_SIZE_MAX) {
            cout << "\t" << funcExpr.funcName << "\t---> Func Length Size: " << funcSize << "\n";
            longFuncExists = true;
        }
    }
    if (!longFuncExists) {
        cout << "\tNo Long Functions Exists \n";
    }
}

void FunctionExtrapolation::formulateFuncExpressions(const vector<string> &sourceCodeList) {
    int numOfLoc = 0;

    unsigned int startIndex = DEF_START_INDEX;
    bool nameExtractRes = false, paramExtractRes = false;
    string funcName, funcParams, funcBody;

    for (string loc: sourceCodeList) {
        numOfLoc += 1;

        startIndex = DEF_START_INDEX;
        nameExtractRes = false, paramExtractRes = false;
        funcName = "", funcParams = "", funcBody = "";

        nameExtractRes = extractFuncName(funcName, loc);
        if (nameExtractRes) {
            startIndex = funcName.size();
            paramExtractRes = extractFuncParams(startIndex, funcParams, loc);
        }
        startIndex = funcName.size() + funcParams.size();

        if (nameExtractRes && paramExtractRes) {
            extractFuncBody(startIndex, funcBody, loc);

            FunctionExpression newFunc(funcName, funcParams, funcBody, numOfLoc);

            funcExpressions.push_back(newFunc);

            if (numOfFunc > 0) {
                funcExpressions.at((numOfFunc - 1)).endLine = numOfLoc;
            }

            numOfFunc += 1;
        } else {
            funcBody = funcName + funcParams;
            extractFuncBody(startIndex, funcBody, loc);
            if (numOfFunc > 0) {
                funcExpressions.at((numOfFunc - 1)).funcBody.push_back(funcBody);
            }
        }
    }
    funcExpressions.at((numOfFunc - 1)).endLine = numOfLoc + 1;
}


FunctionExtrapolation::FunctionExtrapolation(const vector<string> &sourceCode) {
    formulateFuncExpressions(sourceCode);
    printFuncExpressions();
}