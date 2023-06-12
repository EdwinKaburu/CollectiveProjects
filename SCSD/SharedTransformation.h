/**
 * @file SharedTransformation.h
 * @author Edwin Kaburu
 * @date 3/8/2023
 *
 * Transformation Capabilities Of String
 */

#ifndef SHARED_TRANSFORMATION_H
#define SHARED_TRANSFORMATION_H

#include <iostream>
#include <set>

using namespace std;

const int DEF_START_INDEX = 0;
const set<char> delimiterBaseMarks = {',', '=', '&', '*', '.', ';', '(', ')', '{', '}', '/'};

/**
 * leadingSpaceStrip() Remove leading Spaces from String
 *
 * @param wordStructure String
 * @return String
 */
static string leadingSpaceStrip(string &wordStructure) {
    string cleanWord = wordStructure;
    size_t leadingSpaceLoc = wordStructure.find_first_not_of(' ');
    if (leadingSpaceLoc != string::npos) {
        // Clipping from position to N
        cleanWord = wordStructure.substr(leadingSpaceLoc, wordStructure.size());
    }
    return cleanWord;
}

/**
 * trailingSpaceStrip() Remove Trailing Space from String
 *
 * @param wordStructure String
 * @return String
 */
static string trailingSpaceStrip(string &wordStructure) {
    string cleanWord = wordStructure;
    size_t trailingSpaceLoc = wordStructure.find_last_not_of(' ');
    if (trailingSpaceLoc != string::npos) {
        // Clipping from position to N
        cleanWord = wordStructure.substr(DEF_START_INDEX, (trailingSpaceLoc + 1));
    }
    return cleanWord;
}

/**
 * spaceStrip() Remove Leading And Trailing Spaces from String
 *
 * @param wordStructure String
 * @return String
 */
static string spaceStrip(string &wordStructure) {
    string stripResult = leadingSpaceStrip(wordStructure);
    stripResult = trailingSpaceStrip(stripResult);
    return stripResult;
}

/**
 * getStringSplits() Split String based on delimiter character
 *
 * @param word String
 * @param start Unsigned Integer
 * @param end Unsigned Integer
 * @return Vector<String>
 */
static vector<string> getStringSplits(string word, unsigned int start, unsigned int end) {
    vector<string> returnSplits = {};
    string split;
    for (unsigned int i = start; i < end; i++) {
        if (word.at(i) == ' ' and !split.empty()) {
            returnSplits.push_back(split);
            split = "";
        } else {
            split += word.at(i);
        }
    }
    returnSplits.push_back(split);
    return returnSplits;
}

/**
 * doesContainPunctuation() Validate if string contains a delimiter punctuation, ignore others if specified
 *
 * @param word String
 * @param ignoreMarks set<char>
 * @return Boolean
 */
static bool doesContainPunctuation(const string &word, const set<char> &ignoreMarks = {}) {
    for (char letter: word) {
        auto findPunctual = ignoreMarks.find(letter);
        if (findPunctual == ignoreMarks.end()) {
            findPunctual = delimiterBaseMarks.find(letter);
            if (findPunctual != delimiterBaseMarks.end()) {
                return true;
            }
        }
    }
    return false;
}

/**
 * getJaccardSimilarIndex() Get Similarity Index Between Two Vector Strings
 * @param fragment1  vector<string>
 * @param fragment2  vector<string>
 * @return Double
 */
static double getJaccardSimilarIndex(const vector<string> &fragment1, const vector<string> &fragment2) {
    set<string> unionCharacters = {};
    set<string> intersectionCharacters = {};
    for (const string &frag: fragment1) {
        unionCharacters.insert(frag);
    }
    for (const string &items: fragment2) {
        if (unionCharacters.contains(items)) {
            intersectionCharacters.insert(items);
        } else {
            unionCharacters.insert(items);
        }
    }
    return double(intersectionCharacters.size()) / double(unionCharacters.size());
}

#endif //SHARED_TRANSFORMATION_H
