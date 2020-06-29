#ifndef KALDI_SRC_ITN_ITN_UTILS_H_
#define KALDI_SRC_ITN_ITN_UTILS_H_

#if defined(_WIN32) || defined(_WIN64)
#pragma execution_character_set("utf8")
#endif

#include <string>
#include <vector>

namespace itn {

#define CHINESE_CHAR_LEN 3  //utf-8

/**
 * Split the string s by delimiters and store the results in the vector tokens
 * @param s input string
 * @param tokens store the results
 * @param delimiters the one which is used to split the string, note this should be a chinese character
 */
void SplitByCNChar(const std::string& s, std::vector<std::string>& tokens, const std::string& delimiters);

/**
 * read test cases from a file
 * @param file_name the file which stores test cases
 * @param test_cases the vector which has pair elements, Todo: and they store the test cases
 */
void ReadTestCases(const std::string& file_name, std::vector<std::string>& test_cases);

}

#endif //KALDI_SRC_ITN_ITN_UTILS_H_
