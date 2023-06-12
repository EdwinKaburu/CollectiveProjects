/**
 * @file : RandomWordGenerator.h
 * @author : Edwin Kaburu
 * @date : 3/13/2022
 *
 * Generate A Random Sentence with Random Letters
 */
#ifndef EKHUFFMANPROJECT_RANDOMWORDGENERATOR_H
#define EKHUFFMANPROJECT_RANDOMWORDGENERATOR_H

#include <iostream>
#include <vector>
#include <future>
#include <random>

using namespace std;

/**
 * @class RandomWordGenerator Class With functionality to generate a Random Sentence with Random Characters.
 */
class RandomWordGenerator {
private:
    // Minimum Size for Sentence
    int SIZE;
    // Minimum Length for Word
    const int SPACE_AFTER = 5;
    // Number of Alphabet
    const int RD_MAX = 26;
    // Boolean Condition
    bool ALL_SMALL_LETTER = true;
    // Random Engine Generator
    default_random_engine generator;

    /**
     * GenerateParagraph()  Generate A Random Paragraph
     *
     * @param start  Integer Starting Point
     * @param end  Integer End Point
     * @return  String Paragraph
     */
    string GenerateParagraph(int start, int end) {
        string result = "";
        // Calculate Difference
        int dif = (end - start);
        if (dif > SPACE_AFTER) {
            // Find Mid Point
            int mid = (end + start) / 2;
            // Fork Left Side
            auto LeftFork = async(&RandomWordGenerator::GenerateParagraph, this, start, mid);
            // Work On Right Side
            string rightFork = GenerateParagraph(mid, end);
            // Wait for Left
            LeftFork.wait();

            // Combine Results
            return LeftFork.get() + ' ' + rightFork;
        }

        // Get Distribution
        uniform_int_distribution<int> distribution(0, RD_MAX);

        char letter = 'a';
        if (ALL_SMALL_LETTER) {
            letter = 'A';
        }
        for (int i = start; i < end; i++) {
            // Get Random Character
            result += char(letter + distribution(generator) % RD_MAX);
        }
        // Return Result
        return result;
    }

public:
    /**
     * RandomWordGenerator() Constructor To Create Instance of RandomWordGenerator
     * @param sizeInput Integer Size
     * @param isCapLetter Boolean Condition for Capitalization
     */
    RandomWordGenerator(int sizeInput, bool isCapLetter) : SIZE(sizeInput), ALL_SMALL_LETTER(isCapLetter) {

    }

    /**
     * GetRandomParagraph() - Return a Random Paragraph
     * @return
     */
    string GetRandomParagraph() {
        // Return Paragraph
        return GenerateParagraph(0, SIZE);
    }

    /**
     * Destructor() Free Up Resources
     */
    ~RandomWordGenerator() {

    }
};

#endif //EKHUFFMANPROJECT_RANDOMWORDGENERATOR_H
