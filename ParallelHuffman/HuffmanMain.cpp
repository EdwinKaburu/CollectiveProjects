/**
 * @file : HuffmanMain.cpp
 * @author : Edwin Kaburu
 * @date : 3/13/2022
 *
 * Starting Point for Huffman Encoding
 */
#include <iostream>
#include "RandomWordGenerator.h"
#include "HuffmanEncoding.h"

using namespace std;

/**
 * InterfaceEncoding() Print Messages
 * @param input String Input
 */
void InterfaceEncoding(const string& input)
{
    cout << "\n------------------------------------Word Input:------------------------------------\n" << input;
    cout << "\n------------Huffman Encoding Compression Portion------------\n";

    HuffmanEncoding encoding(input);

    // Build Letter Table
    encoding.GenerateLetterTable();
    // Create Optimal Huffman Tree
    encoding.GenerateHuffManTree();

    // Display Letter Table
    cout << "---- Letter Table ----\n";

    encoding.ViewLetterTable();

    cout << "\n---- Encoded Word:----\n";

    string encoded = "";
    encoding.EncodeWord(encoded);

    cout << encoded << "\n";

    cout << "\n------------Huffman Encoding Complete------------";
    cout << "\n---- Decode Word:----\n";

    string decoded = "";
    encoding.DecodeWord(encoded, decoded);

    cout << decoded << "\n";
    cout << "\n--------------------------------End----------------------------------------\n";
}

/**
 * main() Entry Point or Starting Point
 * @return
 */
int main() {

    const int MINIMUM_CHARACTERS = 20; // Will be Higher to Added spaces between
    const string TEST_WORD = "What if the confident courage ate the win?";

    RandomWordGenerator randWord (MINIMUM_CHARACTERS, false);
    string input = randWord.GetRandomParagraph();

    InterfaceEncoding(input);

    InterfaceEncoding(TEST_WORD);

    return 0;
}
