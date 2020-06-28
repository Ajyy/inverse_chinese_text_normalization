#include "base/kaldi-types.h"
#include "itn/itn.h"
#include "itn/itn-utils.h"

namespace itn {

using namespace std;

const map<string, int32> BASE_NUMBER_MAP_TABLE = {
    {"零", 0}, {"一", 1}, {"二", 2}, {"三", 3}, {"四", 4},
    {"五", 5}, {"六", 6}, {"七", 7}, {"八", 8}, {"九", 9}
};
// Todo: 支持 兆京垓秭穰沟涧正载 一千千
const map<string, int32> UNIT_MAP_TABLE = {
    {"亿", 100000000}, {"万", 10000}, {"千", 1000},
    {"百", 100}, {"十", 10}
};
const vector<string> BASE_NUMBER = {
    "零", "一", "二", "三", "四", "五",
    "六", "七", "八", "九", "幺", "两"
};
const vector<string> UNITS = {
    "亿", "万", "千", "百", "十"
};
// Used for preprocess sentences
const map<string, string> BIG_TO_SMALL = {
    {"壹", "一"}, {"贰", "二"}, {"叁", "三"}, {"肆", "四"}, {"伍", "五"},
    {"陆", "六"}, {"柒", "七"}, {"捌", "八"}, {"玖", "九"}, // Simplify number
    {"貳", "二"}, {"參", "三"}, {"陸", "六"}, // Traditional number, others are same as the simplify one
    {"拾", "十"}, {"佰", "百"}, {"仟", "千"}, {"萬", "万"}, // Units
};
const map<string, string> PRE_SPACIAL_MAP_TABLE = {
    {"两", "二"}, {"兩", "二"}, {"幺", "一"}
};

const map<string, string> LAST_SPACIAL_MAP_TABLE = {
    {"正", "+"}, {"负", "-"}, {"負", "-"}, {"點", "."}, {"点", "."},
};

void FindCNNums(const string& sent, vector<pair<string, int32>> &nums_info) {
  int32 i = 0;
  string cn_num;
  string last_char;
  while (i + CHINESE_CHAR_LEN - 1 < sent.length()) {
    string cur_cn_char = sent.substr(i, CHINESE_CHAR_LEN);
    while (BASE_NUMBER_MAP_TABLE.count(cur_cn_char) || (UNIT_MAP_TABLE.count(cur_cn_char) && last_char != "点")) {
      cn_num += cur_cn_char;
      i += CHINESE_CHAR_LEN;
      if (i + CHINESE_CHAR_LEN - 1 < sent.length())
        cur_cn_char = sent.substr(i, CHINESE_CHAR_LEN);
      else
        break;
    }

    if (!cn_num.empty()) {
      nums_info.emplace_back(pair<string, int32>(cn_num, i - cn_num.length()));
      cn_num = "";
    }
    i += CHINESE_CHAR_LEN;
    last_char = cur_cn_char;
  }
}

string CNNumTranslation(const string& cn_num) {
  string no_unit_result = ConvertNoUnitCNNum(cn_num);
  if (!no_unit_result.empty())
    return no_unit_result;

  return to_string(ConvertNormalCNNum(cn_num));
}

string ConvertNoUnitCNNum(const string& cn_num) {
  int32 arabic_num = 0;
  int32 base = 1;
  for (int i = int(cn_num.length()) - CHINESE_CHAR_LEN; i >= 0; i -= CHINESE_CHAR_LEN) {
    string cur_cn_char = cn_num.substr(i, CHINESE_CHAR_LEN);
    if (UNIT_MAP_TABLE.count(cur_cn_char))
      return "";
    arabic_num += (BASE_NUMBER_MAP_TABLE.find(cur_cn_char)->second * base);
    base *= 10;
  }

  return to_string(arabic_num);
}

unsigned ConvertNormalCNNum(string sub_num) {
  if (sub_num.empty())
    return 0;

  vector<string> sub_nums;
  SplitByCNChar(sub_num, sub_nums, "零");
  if (sub_nums.size() != 1) {
    int32 arabic_num = 0;
    for (const auto& sub_sub_num: sub_nums)
      arabic_num += ConvertNormalCNNum(sub_sub_num);
    return arabic_num;
  }

  string max_unit = FindMaxUnit(sub_num);
  int32 unit_value = UNIT_MAP_TABLE.find(max_unit)->second;
  if (max_unit == "个")
    return BASE_NUMBER_MAP_TABLE.find(sub_num)->second;
  else if (sub_num == "十")
    return 10;
  else {
    if (sub_num.substr(0, CHINESE_CHAR_LEN) == "十")
      sub_num = "一" + sub_num;
    sub_nums = {};
    SplitByCNChar(sub_num, sub_nums, max_unit);
    if (sub_nums[1].length() == CHINESE_CHAR_LEN)
      return ConvertNormalCNNum(sub_nums[0]) * unit_value + ConvertNormalCNNum(sub_nums[1]) * int(unit_value / 10);
    else
      // Todo: 一十
      return ConvertNormalCNNum(sub_nums[0]) * unit_value + ConvertNormalCNNum(sub_nums[1]);
  }
}

string FindMaxUnit(const string& num) {
  for(const auto & unit : UNITS)
    if (num.find(unit) != string::npos)
      return unit;
  return "个";
}

string ProcessSent(const string &sent, const string& order) {
  string processed_sent;
  for (int32 i = 0; i < sent.length(); i += CHINESE_CHAR_LEN) {
    while (isdigit(sent[i])) {
      processed_sent += sent[i];
      i++;
    }
    string cur_char = sent.substr(i, CHINESE_CHAR_LEN);
    if (order == "pre") {
      if (BIG_TO_SMALL.count(cur_char)) { // Big number to small number
        processed_sent += BIG_TO_SMALL.find(cur_char)->second;
      } else if (PRE_SPACIAL_MAP_TABLE.count(cur_char)) { // For special case
        processed_sent += PRE_SPACIAL_MAP_TABLE.find(cur_char)->second;
      } else {
        processed_sent += cur_char;
      }
    } else {
      if (LAST_SPACIAL_MAP_TABLE.count(cur_char)) { // For special case
        processed_sent += LAST_SPACIAL_MAP_TABLE.find(cur_char)->second;
      } else {
        processed_sent += cur_char;
      }
    }
  }

  return processed_sent;
}

string InverseNormalize(const string& sent) {
  string processed_sent = ProcessSent(sent, "pre");
  vector<pair<string, int32>> nums_info;
  FindCNNums(processed_sent, nums_info);

  int32 left_idx = 0;
  string result;
  for (const pair<string, int32>& num_info: nums_info) {
    result += (processed_sent.substr(left_idx, num_info.second - left_idx) + CNNumTranslation(num_info.first));
    left_idx = num_info.second + num_info.first.length();
  }

  if (left_idx != processed_sent.length())
    result += processed_sent.substr(left_idx, processed_sent.length() - left_idx);
  return ProcessSent(result, "last");
}

}