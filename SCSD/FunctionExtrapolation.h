/**
 * @file FunctionExtrapolation.h
 * @author Edwin Kaburu
 * @date 3/8/2023
 */

#ifndef FUNCTION_EXTRAPOLATION_H
#define FUNCTION_EXTRAPOLATION_H

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "SharedTransformation.h"

using namespace std;

const unsigned short int SPLIT_SIZE_TOLERANCE = 2;
const unsigned short int FUN_PARAM_THRESHOLD = 3;
const unsigned short int FUN_SIZE_MAX = 15;
const double JACCARD_INDEX_THRESHOLD = 0.75;

/**
 * @struct FunctionExpression
 * @details A Function/Method Structure Decomposition
 */
struct FunctionExpression {
    string funcName;
    string funcParams;
    vector<string> funcBody;
    int stLine = 0;
    int endLine = 0;

    /**
     * FunctionExpression() Constructor To Create FunctionExpression Instance
     *
     * @param name String
     * @param params String
     * @param body String
     * @param start Int
     */
    FunctionExpression(const string &name, const string &params, const string &body, int start) {
        this->funcName = name;
        this->funcParams = params;
        this->funcBody.push_back(body);
        this->stLine = start;
    }

    /**
     * getExtrapolatedComposition() Convert FunctionExpression Decomposition To a Vector
     *
     * @param funCompose vector<string>
     */
    void getExtrapolatedComposition(vector<string> &funCompose) const {
        funCompose.push_back(funcName);
        extrapolateParams(funCompose);
        copy(funcBody.begin(), funcBody.end(), back_inserter(funCompose));
    }

    /**
     * extrapolateParams() Fetch parameters from strings representation to vector.
     *
     * @param paramsCompo vector<string>
     */
    void extrapolateParams(vector<string> &paramsCompo) const {
        string param;
        char cParam;
        for (int i = DEF_START_INDEX; i < funcParams.size(); i++) {

            cParam = funcParams.at(i);

            if (cParam == '(') {
                continue;
            } else if ((cParam == ',' || cParam == ')') && !param.empty()) {
                paramsCompo.push_back(spaceStrip(param));
                param = "";
            } else {
                param += cParam;
            }
        }
    }
};

/**
 * @class FunctionExtrapolation
 * @details Analyze Source Lines Of Code, Extract Functions abstractions And Print Code Smells
 */
class FunctionExtrapolation {
private:
    // FunctionExpression Holder
    vector<FunctionExpression> funcExpressions = {};
    // Total Number of Functions
    int numOfFunc = 0;

    /**
     * isValidExpression() Validate expression definition.
     *
     * @param expression String
     * @param acceptableMarks set<char>
     * @return Boolean
     */
    bool isValidExpression(string &expression, const set<char> &acceptableMarks = {});

    /**
     * isValidFuncParams Validate function parameter definition.
     *
     * @param funcParams String
     * @param acceptableMarks set<char>
     * @return Boolean
     */
    bool isValidFuncParams(string &funcParams, const set<char> &acceptableMarks = {});

    /**
     * isValidFuncName() Validate function Name definition.
     *
     * @param funcName String
     * @return Boolean
     */
    bool isValidFuncName(string &funcName);

    /**
     * extractFuncName() Extracts a Function Name from a Line of Code
     *
     * @param funcName String
     * @param lineOfCode String
     * @return Boolean
     */
    bool extractFuncName(string &funcName, string &lineOfCode);

    /**
     * extractFuncParams() Extracts Functions Parameters From Line of Code
     *
     * @param startIndex Int
     * @param funcParams String
     * @param lineOfCode String
     * @return Boolean
     */
    bool extractFuncParams(unsigned int startIndex, string &funcParams, string &lineOfCode);

    /**
     * extractFuncBody() Extract Function Body from Line of Code
     *
     * @param startIndex Int
     * @param funcBody String
     * @param lineOfCode String
     */
    void extractFuncBody(unsigned int startIndex, string &funcBody, string &lineOfCode);
    
    /**
     * isDuplicateCode() Validates If Two FunctionExpressions Are Similar
     *
     * @param funcExpress1  FunctionExpression
     * @param funcExpress2 FunctionExpression
     * @return Boolean
     */
    bool isDuplicateCode(const FunctionExpression &funcExpress1, const FunctionExpression &funcExpress2);

public:
    /**
     * FunctionExtrapolation() Constructor To Create Instance of FunctionExtrapolation
     *
     * @param sourceCode vector<string>
     */
    explicit FunctionExtrapolation(const vector<string> &sourceCode);

    /**
     * formulateFuncExpressions() Formulate the FunctionExpression Abstraction List
     *
     * @param sourceCodeList vector<string>
     */
    void formulateFuncExpressions(const vector<string> &sourceCodeList);

    /**
     * printFuncExpressions() Print All Functions within SLOC
     */
    void printFuncExpressions();

    /**
     * printDuplicateFunc() Print All Duplicate Functions within SLOC
     */
    void printDuplicateFunc();

    /**
     * printLongParams() Print All Functions with Long Parameter within SLOC
     */
    void printLongParams();

    /**
     * printLongFunction() Print All Long Sized Functions within SLOC
     */
    void printLongFunction();
};


#endif //FUNCTION_EXTRAPOLATION_H
