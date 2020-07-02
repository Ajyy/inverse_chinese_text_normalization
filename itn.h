#ifndef KALDI_SRC_ITN_ITN_H_
#define KALDI_SRC_ITN_ITN_H_

#if defined(_WIN32) || defined(_WIN64)
#pragma execution_character_set("utf8")
#endif

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

namespace itn {

#define CHINESE_CHAR_LEN 3  //utf-8

/***
 * Combine the space between chinese number or units
 * @param input sentence
 * @return the processed sentence
 */
std::string ProcessForSegment(const std::string& sent);

/***
 * Test whether the input cn_num is a valid chinese number
 * @param cn_num
 * @return true if valid
 */
bool IsValid(const std::string& cn_num);

/***
 * Extract the chinese number in the input string
 * @param sent input sentence
 * @param nums_info store the result, its element is a pair (chinese number, it's index)
 * @param ic_index the param to check idioms and ci, which is from CheckIdiomAndCi
 */
void FindCNNums(const std::string& sent, std::vector<std::pair<std::string, int>> &nums_info, std::vector<int> &ic_index);

/***
 * Check the idiom and ci (词语) in the input sentence
 * @param sent the input sentence
 * @param ic_index the result which save the index of the idiom and ci in the sentence
 */
void CheckIdiomAndCi(const std::string& sent, std::vector<int> &ic_index);

/***
 * Convert the chinese number to arabic number
 * @param cn_num chinese number
 * @return arabic number
 */
std::string CNNumTranslation(std::string cn_num);

/***
 * Convert chinese number which does not has units to arabic number like 二零二零，一三五
 * @param cn_num chinese number
 * @return arabic number
 */
std::string ConvertNoUnitCNNum(const std::string& cn_num);

/***
 * Convert the normal chinese number (compared to the number which does not have units) to arabic number
 * @param sub_num normal chinese number
 * @return arabic number
 */
long ConvertNormalCNNum(std::string sub_num);

/***
 * Find the max unit in the string sub_num
 * @param num the input chinese number
 * @return max unit
 */
std::string FindMaxUnit(const std::string& num);

/***
 * Preprocess the input sentence for big to small and some special cases
 * @param sent input sentence
 * @param order the time to process the sentence, only to choice: last and pre
 * @param ic_index the param to check idioms and ci, which is from CheckIdiomAndCi
 * @return processed sentence
 */
std::string ProcessSent(const std::string& sent, const std::string& order, std::vector<int> &ic_index);

/***
 * Convert the normalized sentence to the original one, this is the main function
 * @param sent input sentence
 * @return the processed sentence
 */
std::string InverseNormalize(const std::string& sent);

/***
 * Check whether the date keyword exists for the characters around the chinese character, which located at the input index.
 * Only check the previous five characters and the four characters after the character at the input index
 * @param sent the input sentence
 * @param index the index of the chinese character which needs to be checked
 * @return true if keywords exists
 */
bool CheckDateKeyword(const std::string& sent, int index);

}

#endif //KALDI_SRC_ITN_ITN_H_
