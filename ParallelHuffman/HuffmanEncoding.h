/**
 * @file : HuffmanEncoding.h
 * @author : Edwin Kaburu
 * @date : 3/13/2022
 *
 * Perform Data compression on string by encoding and decode the compressed data. Huffman Encoding
 * Implementation
 */
#ifndef EKHUFFMANPROJECT_HUFFMANENCODING_H
#define EKHUFFMANPROJECT_HUFFMANENCODING_H

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <future>
#include <iomanip>

using namespace std;

/**
 * @struct Character Representation Data Type
 */
struct CharacterTypeInfo {
    char symbol = '\0'; // Default Character Symbol
    int count = -1; // Default count value
    string codeword = ""; // Default codeword
    CharacterTypeInfo *left = nullptr; // Default pointer
    CharacterTypeInfo *right = nullptr; // Default pointer
};

/**
 * @class HuffmanEncoding . Class with functionality to decompress a string value.
 */
class HuffmanEncoding {
public:

    /**
     * HuffmanEncoding() Default constructor to create instance of HuffmanEncoding
     * @param input1 String Data
     * @param threshold Integer Threshold
     */
    HuffmanEncoding(const string &input1, const int threshold = 4) : WORD_DATA(input1), THRESHOLD(threshold) {

    }

    /**
     * conditionCheck() Compare Two CharacterTypeInfo Struct
     * @param a CharacterTypeInfo Object
     * @param b CharacterTypeInfo Object
     * @return Boolean Condition
     */
    static bool conditionCheck(CharacterTypeInfo &a, CharacterTypeInfo &b) {
        // return true is a's count is less than b
        return a.count < b.count;
    }

    /**
     * ViewLetterTable() Display a Frequency Table
     */
    void ViewLetterTable() {
        if (!LETTER_TABLE.empty()) {
            cout << left << setw(8) << "Symbol" << left << setw(8)
                 << "Counts" << left << setw(8) << "Code" << left << endl;
            for (int i = 0; i < LETTER_TABLE.size(); i++) {
                cout << left << setw(8) << LETTER_TABLE.at(i).symbol << left << setw(8) << LETTER_TABLE.at(i).count
                     << left << setw(8) << LETTER_TABLE.at(i).codeword << left << endl;
            }
        }
    }

    /**
     * GenerateLetterTable() Constructs a Letter or Frequency Table
     */
    void GenerateLetterTable() {
        // Count Frequencies of Character
        CountFrequencies(0, WORD_DATA.size());
        // Sort Ascending By Counts
        sort(LETTER_TABLE.begin(), LETTER_TABLE.end(), conditionCheck);
    }

    /**
     * GenerateHuffManTree() Constructs Huffman Tree and update character codes based on its traversal
     */
    void GenerateHuffManTree() {
        // Get Size of frequency Table
        int totalSize = LETTER_TABLE.size();

        // Start on Clean Slate
        HUFFMAN_TREE_NODES.clear();
        // Resize Huffman Tree Interior Nodes
        IncreaseSize((totalSize - 1));

        // Constructor Huffman Tree
        OptimalHuffmanTree(totalSize);
        // Update Character Codes
        WriteEncodes(&HUFFMAN_TREE_NODES[0], "");
    }

    /**
     * EncodeWord() - Encodes Data and write result to output
     * @param output String Output
     */
    void EncodeWord(string &output) {
        // Write Output the compressed data
        output = LettersEncode(0, WORD_DATA.size(), THRESHOLD);
    }

    /**
     * DecodeWord() Decodes a Compressed String, Write and returns back uncompressed string
     * @param input1 String Compressed Input
     * @param output String UnCompressed Output
     */
    void DecodeWord(string input1, string &output) {
        // Write Output, decompressed result
        output = GetLetters(&HUFFMAN_TREE_NODES[0], input1);
    }

    /**
     * ~HuffmanEncoding() Destructor for HuffmanEncoding Object
     */
    ~HuffmanEncoding() {
        // Free Up Memory
        HUFFMAN_TREE_NODES.clear();
        LETTER_TABLE.clear();
    }

private:
    // String Word Data
    string WORD_DATA;
    // Frequency Table or Letter Table
    vector<CharacterTypeInfo> LETTER_TABLE;
    // Huffman Tree
    vector<CharacterTypeInfo> HUFFMAN_TREE_NODES;
    // Threshold Limit
    int THRESHOLD;
    // Mutex Critical Section
    mutex CRITICAL_SECTION;

    /**
     * CountFrequencies() Count Number of Duplicate Occurrences, Updates Frequency or Letter Table
     * @param start Integer Starting Index
     * @param end Integer End Index
     * @return Boolean Condition
     */
    bool CountFrequencies(int start, int end) {
        // Difference between start and end
        int dif = (end - start);
        if (dif > THRESHOLD) {
            // Calculate Mid Point
            int mid = (end + start) / 2;

            // Left Thread Fork
            auto leftForkThread = async(&HuffmanEncoding::CountFrequencies, this, start, mid);
            // Right Frequencies in Second Half
            CountFrequencies(mid, end);

            // Wait for Left Fork Thread
            leftForkThread.wait();
            return true;
        }

        // Acquire Lock
        CRITICAL_SECTION.lock();
        for (int i = start; i < end; i++) {
            // Get Character from WORD_DATA
            char letter = WORD_DATA.at(i);
            // Find This Letter, Looking for Duplicate
            int index = FindCharacter(letter);
            if (index == -1) {
                // Does Not Exist. Create New CharacterTypeInfo data type
                CharacterTypeInfo newLetter;
                newLetter.symbol = letter;
                newLetter.count = 1;
                newLetter.codeword = "";
                // Push Back
                LETTER_TABLE.push_back(newLetter);
            } else {
                // Letter Exists, Increment The Count
                LETTER_TABLE.at(index).count = LETTER_TABLE.at(index).count + 1;
            }
        }
        // Release Lock
        CRITICAL_SECTION.unlock();

        return true;
    }

    /**
     * WriteEncodes() Update Letter's codeword or encoded in the Letter/Frequency Table,
     * based on Huffman Tree, traversal Accumulation from left to right, vice-versa
     * @param root CharacterTypeInfo Root Node
     * @param prevCode Accumulated String Encoding
     * @return Boolean Condition
     */
    bool WriteEncodes(CharacterTypeInfo *root, string prevCode) {
        // Boolean Condition
        if (!isNodeLeaf(*root)) {
            // Not A Leaf
            // Add 0 for left side traversal
            string leftCode = prevCode + "0";
            // Fork Left Child
            auto leftChildFork = async(&HuffmanEncoding::WriteEncodes, this, root->left, leftCode);

            // Add 1 for right side traversal
            string rightCode = prevCode + "1";
            // Fork Right Child
            bool rightChildFork = WriteEncodes(root->right, rightCode);

            // Wait for Left Thread
            leftChildFork.wait();
            return rightChildFork;
        }

        // A Leaf
        // Update codeword with accumulated encoding
        root->codeword = prevCode;
        return true;
    }

    /**
     * LettersEncode() Update Entire Uncompressed Word with encoding using the Frequency/Letter Table
     * @param start Integer Start
     * @param end Integer End
     * @param threshold Integer Threshold
     * @return String Encoded or Compressed Word/String/Information/Data
     */
    string LettersEncode(int start, int end, int threshold) {
        // Calculate Difference
        int dif = end - start;
        if (dif > threshold) {
            // Find Midpoint
            int mid = (end + start) / 2;

            // Fork the Left Thread
            auto leftChild = async(&HuffmanEncoding::LettersEncode, this, start, mid, threshold);
            // Run Second Halves
            string rightChild = LettersEncode(mid, end, threshold);

            // Wait for Left Child
            leftChild.wait();

            // Combine and Return Results
            return leftChild.get() + rightChild;
        }

        // result string
        string result = "";
        for (int i = start; i < end; i++) {
            // Get Letter/Character Encoding
            string getEncode = GetLetterEncode(0, WORD_DATA.at(i));
            // Combine Encodings
            result += getEncode;
        }
        return result;
    }

protected:
    /**
     * leftIndex() Returns the Left Index based on Parent Index, Heap
     * @param parentIndex Integer Parent Index
     * @return Integer Left Index
     */
    virtual int leftIndex(int parentIndex) {
        return (2 * parentIndex) + 1;
    }

    /**
     * rightIndex() Returns the Right Index based on Parent Index, Heap
     * @param parentIndex Integer Parent Index
     * @return Integer Right Index
     */
    virtual int rightIndex(int parentIndex) {
        return (2 * parentIndex) + 2;
    }

    /**
     * isNodeLeaf() Check if Root is a Leaf
     * @param root CharacterTypeInfo Root
     * @return Boolean Condition based on criteria
     */
    bool isNodeLeaf(CharacterTypeInfo &root) {
        // Check is Both Left and Right Pointers are Null
        if (root.left == nullptr && root.right == nullptr) {
            // Returns True
            return true;
        }
        // Returns False
        return false;
    }

    /**
     * IncreaseSize() Increase/Adds more capacity for the HuffmanTreeNodes
     * @param newSize Integer New Size
     */
    virtual void IncreaseSize(int newSize) {
        for (int i = 0; i < newSize; i++) {
            // Push Back Empty CharacterTypeInfo's
            CharacterTypeInfo sp;
            HUFFMAN_TREE_NODES.push_back(sp);
        }
    }

    /**
     * FindCharacter() Finds a letter from the Frequency/Letter Table
     * @param letter Character
     * @return Integer Index Position
     */
    virtual int FindCharacter(char letter) {
        if (!LETTER_TABLE.empty()) {
            // Starting Root
            int root = 0;
            while (root <= (LETTER_TABLE.size() / 2)) {
                // Get Left and Right Child Indexes
                int rightT = rightIndex(root);
                int leftT = leftIndex(root);

                // Check is root, or if Both Left and Right Index, values equal to letter.
                if (LETTER_TABLE.at(root).symbol == letter) {
                    return root;
                }
                if (rightT < LETTER_TABLE.size() && LETTER_TABLE.at(rightT).symbol == letter) {
                    return rightT;
                }
                if (leftT < LETTER_TABLE.size() && LETTER_TABLE.at(leftT).symbol == letter) {
                    return leftT;
                }
                root++;
            }
        }
        return -1;
    }

    /**
     * OptimalHuffmanTree() Construct a Huffman Tree
     * @param totalSize Integer Frequency/Letter Table Size
     */
    virtual void OptimalHuffmanTree(int totalSize) {
        // Index of letter Table
        int i = 0;
        // For Huffman Tree
        int p = totalSize - 2, k = p, l = k;

        while (p >= 0) {
            if (i < LETTER_TABLE.size() - 1) {
                if (p == k && k == l) {
                    HUFFMAN_TREE_NODES.at(p).count = LETTER_TABLE.at(i).count + LETTER_TABLE.at((i + 1)).count;
                    HUFFMAN_TREE_NODES.at(p).left = &LETTER_TABLE.at(i);
                    HUFFMAN_TREE_NODES.at(p).right = &LETTER_TABLE.at((i + 1));

                    p--;
                    i += 2;
                } else {
                    // Check if L, count is greater Than I, count
                    if (HUFFMAN_TREE_NODES.at(l).count > LETTER_TABLE.at(i).count) {
                        // Insert Both I, and I + 1
                        HUFFMAN_TREE_NODES.at(p).count = LETTER_TABLE.at(i).count + LETTER_TABLE.at((i + 1)).count;
                        HUFFMAN_TREE_NODES.at(p).left = &LETTER_TABLE.at(i);
                        HUFFMAN_TREE_NODES.at(p).right = &LETTER_TABLE.at((i + 1));

                        p--;
                        k--;
                        i += 2;
                    } else {
                        // Insert Both I, and Interior Node Value
                        HUFFMAN_TREE_NODES.at(p).count = LETTER_TABLE.at(i).count + HUFFMAN_TREE_NODES.at(l).count;
                        HUFFMAN_TREE_NODES.at(p).left = &HUFFMAN_TREE_NODES.at(l);
                        HUFFMAN_TREE_NODES.at(p).right = &LETTER_TABLE.at(i);

                        p--;
                        l--;
                        k--;
                        i++;
                    }
                }
            } else if (i == LETTER_TABLE.size() - 1) {
                // Insert Both I, and Interior Node Value
                HUFFMAN_TREE_NODES.at(p).count = LETTER_TABLE.at(i).count + HUFFMAN_TREE_NODES.at(l).count;
                HUFFMAN_TREE_NODES.at(p).left = &HUFFMAN_TREE_NODES.at(l);
                HUFFMAN_TREE_NODES.at(p).right = &LETTER_TABLE.at(i);

                if (HUFFMAN_TREE_NODES.at(l).count > LETTER_TABLE.at(i).count) {
                    HUFFMAN_TREE_NODES.at(p).left = &LETTER_TABLE.at(i);
                    HUFFMAN_TREE_NODES.at(p).right = &HUFFMAN_TREE_NODES.at(l);
                }

                p--;
                l--;
                k--;
                i++;
            } else {
                // combine k and l
                if (l >= k) {
                    int sum = HUFFMAN_TREE_NODES.at(l).count + HUFFMAN_TREE_NODES.at((l - 1)).count;
                    // Insert Both I, and Interior Node Value
                    HUFFMAN_TREE_NODES.at(p).count = sum;
                    HUFFMAN_TREE_NODES.at(p).left = &HUFFMAN_TREE_NODES.at(l);
                    HUFFMAN_TREE_NODES.at(p).right = &HUFFMAN_TREE_NODES.at((l - 1));

                    if (HUFFMAN_TREE_NODES.at(l).count > HUFFMAN_TREE_NODES.at((l - 1)).count) {
                        HUFFMAN_TREE_NODES.at(p).left = &HUFFMAN_TREE_NODES.at((l - 1));

                        HUFFMAN_TREE_NODES.at(p).right = &HUFFMAN_TREE_NODES.at(l);
                    }

                    p--;
                    l -= 2;
                    k--;
                }

            }

        }
    }

    /**
     * GetLetterEncoding() Find a encoding for letter from Frequency/Letter Table
     * @param root Integer Root
     * @param letter Character Letter
     * @return String Encoding
     */
    string GetLetterEncode(int root, char letter) {
        while (root < LETTER_TABLE.size() / 2) {
            // Get Left and Right Index
            int rightI = rightIndex(root);
            int leftI = leftIndex(root);

            // Compares, if Root or Right/left values are equal to Letter
            if (LETTER_TABLE.at(root).symbol == letter) {
                // Return Encoded Entry
                return LETTER_TABLE.at(root).codeword;
            }
            if (rightI < LETTER_TABLE.size() && LETTER_TABLE.at(rightI).symbol == letter) {
                return LETTER_TABLE.at(rightI).codeword;
            }
            if (leftI < LETTER_TABLE.size() && LETTER_TABLE.at(leftI).symbol == letter) {
                return LETTER_TABLE.at(leftI).codeword;
            }
            root++;
        }
        return "";
    }

    /**
     * GetLetters() Get the Leaf Node on traversal based on encoding
     * @param root CharacterTypeInfo root
     * @param input String Coded Input
     * @return String Result
     */
    virtual string GetLetters(CharacterTypeInfo *root, string &input) {
        //  reset root
        string result = "";
        CharacterTypeInfo *find = root;

        int index = 0;
        while (index <= input.size()) {
            if (isNodeLeaf(*find)) {
                result += find->symbol;
                find = root;
            }
            if (index < input.size() && input.at(index) == '0') {
                find = find->left;
            }

            if (index < input.size() && input.at(index) == '1') {
                find = find->right;
            }
            index++;
        }
        return result;
    }
};

#endif //EKHUFFMANPROJECT_HUFFMANENCODING_H
