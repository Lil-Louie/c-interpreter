//
// Created by xmani on 2/2/2023.
//
//
// Created by Ali Kooshesh on 1/22/23.
//

#ifndef PROJECT1_PHASE2_TOKENIZER_HPP
#define PROJECT1_PHASE2_TOKENIZER_HPP


#include <string>
#include <fstream>
#include "Token.hpp"

class Tokenizer {

public:
    Tokenizer(std::string);
    Token getToken();
    std::string getTokenTagName(char& c);

private:
    int lineNumber, charPosition, state = 0;
    std::string inputFileName;
    std::ifstream inputStream;

    bool charOfInterest(char c);
};


#endif //PROJECT1_PHASE2_TOKENIZER_HPP
