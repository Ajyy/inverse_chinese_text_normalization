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
 * Test whether the input cn_num is a valid chinese number
 * @param cn_num
 * @return
 */
bool IsValid(const std::string& cn_num);

/***
 * Extract the chinese number in the input string
 * @param sent input sentence
 * @param nums_info store the result, its element is a pair (chinese number, it's index)
 * Todo ic_index
 */
void FindCNNums(const std::string& sent, std::vector<std::pair<std::string, int32>> &nums_info, std::vector<int32> &ic_index);

/***
 *
 * @param sent input
 * @param ic_index
 */
void CheckIdiomAndCi(const std::string& sent, std::vector<int32> &ic_index);

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
unsigned ConvertNormalCNNum(std::string sub_num);

/***
 * Find the max unit in the string sub_num
 * @param num
 * @return
 */
std::string FindMaxUnit(const std::string& num);

/***
 * Preprocess the input sentence for big to small and some special cases
 * @param sent input sentence
 * Todo ic_index
 * @return processed sentence
 */
std::string ProcessSent(const std::string& sent, const std::string& order, std::vector<int32> &ic_index);

/***
 * Convert the normalized sentence to the original one, this is the main function
 * @param sent input sentence
 * @return the processed sentence
 */
std::string InverseNormalize(const std::string& sent);

}

#endif //KALDI_SRC_ITN_ITN_H_
