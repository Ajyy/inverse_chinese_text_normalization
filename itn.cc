#include "base/kaldi-types.h"
#include "itn/itn.h"
#include "itn/itn-utils.h"

namespace itn {

using namespace std;

// Todo 星期
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
vector<string> idiom_and_ci;
// Used for preprocess sentences
const map<string, string> BIG_TO_SMALL = {
    {"壹", "一"}, {"贰", "二"}, {"叁", "三"}, {"肆", "四"}, {"伍", "五"},
    {"陆", "六"}, {"柒", "七"}, {"捌", "八"}, {"玖", "九"}, // Simplify number
    {"貳", "二"}, {"參", "三"}, {"陸", "六"}, // Traditional number, others are same as the simplify one
    {"拾", "十"}, {"佰", "百"}, {"仟", "千"}, {"萬", "万"}, {"億", "亿"} // Units
};
const map<string, string> PRE_SPACIAL_MAP_TABLE = {
    {"两", "二"}, {"兩", "二"}, {"幺", "一"}
};
const map<string, string> LAST_SPACIAL_MAP_TABLE = {
    {"正", "+"}, {"负", "-"}, {"負", "-"}, {"點", "."}, {"点", "."}, {"比", ":"},
};
// For date
const vector<string> DATE_KEYWORD = {
    "时间", "上午", "中午", "下午", "晚上", "凌晨", "分", "日",
};

bool IsValid(const std::string& cn_num) {
  bool has_cont_base_num = false;
  bool has_unit = false;
  string last_char;
  string cur_cn_char;
  for (int32 i = 0; i < cn_num.length(); i += CHINESE_CHAR_LEN) {
    last_char = cur_cn_char;
    cur_cn_char = cn_num.substr(i, CHINESE_CHAR_LEN);
    if (last_char != "零" &&BASE_NUMBER_MAP_TABLE.count(cur_cn_char) && BASE_NUMBER_MAP_TABLE.count(last_char))
      has_cont_base_num = true;
    if (UNIT_MAP_TABLE.count(cur_cn_char))
      has_unit = true;
  }

  return !(has_unit && has_cont_base_num);
}

void CheckIdiomAndCi(const string& sent, vector<int32> &ic_index) {
  for (const auto& ic: idiom_and_ci) {
    int32 ic_idx = -1;
    if ((ic_idx = sent.find(ic)) != string::npos) {
      for (int i = ic_idx; i < ic_idx + ic.length(); i += CHINESE_CHAR_LEN)
        ic_index.emplace_back(i);
    }
  }
}

bool CheckDateKeyword(const string& sent, int index) {
  int32 start_index = 0;
  if (index - 5 * CHINESE_CHAR_LEN >= 0)
    start_index = index - 5 * CHINESE_CHAR_LEN;
  int32 end_index = sent.length();
  if (index + 5 * CHINESE_CHAR_LEN <= sent.length())
    end_index = index + 5 * CHINESE_CHAR_LEN;
  string check_str = sent.substr(start_index, end_index - start_index);
  for (const auto& keyword: DATE_KEYWORD)
    if (check_str.find(keyword) != string::npos)
      return true;
  return false;
}

void FindCNNums(const string& sent, vector<pair<string, int32>> &nums_info, vector<int32> &ic_index) {
  int32 i = 0;
  string cn_num;
  string last_char;
  bool check_dian = false;
  while (i + CHINESE_CHAR_LEN - 1 < sent.length()) {
    if (find(ic_index.begin(), ic_index.end(), i) != ic_index.end()) {
      i += CHINESE_CHAR_LEN;
      continue;
    }

    string cur_cn_char = sent.substr(i, CHINESE_CHAR_LEN);

    // Test for 点
    if (cur_cn_char == "点" && i + CHINESE_CHAR_LEN < sent.length()
        && find(ic_index.begin(), ic_index.end(), i + CHINESE_CHAR_LEN) == ic_index.end()
        && (BASE_NUMBER_MAP_TABLE.count(sent.substr(i + CHINESE_CHAR_LEN, CHINESE_CHAR_LEN))
            || sent.substr(i + CHINESE_CHAR_LEN, CHINESE_CHAR_LEN) == "十"))
      check_dian = CheckDateKeyword(sent, i);

    while (BASE_NUMBER_MAP_TABLE.count(cur_cn_char) || (UNIT_MAP_TABLE.count(cur_cn_char) && (last_char != "点" || check_dian))) {
      cn_num += cur_cn_char;
      i += CHINESE_CHAR_LEN;
      if (i + CHINESE_CHAR_LEN - 1 < sent.length())
        cur_cn_char = sent.substr(i, CHINESE_CHAR_LEN);
      else
        break;
    }

    if (!cn_num.empty()) {
      if (check_dian)
        check_dian = false;
      if (IsValid(cn_num) && cn_num != "百") {
        nums_info.emplace_back(pair<string, int32>(cn_num, i - cn_num.length()));
      }
      cn_num = "";

      if (UNIT_MAP_TABLE.count(cur_cn_char) == 0) {
        last_char = sent.substr(i - CHINESE_CHAR_LEN, CHINESE_CHAR_LEN);
        continue;
      }
    }

    i += CHINESE_CHAR_LEN;
    last_char = cur_cn_char;
  }
}

string CNNumTranslation(string cn_num) {
  if (UNIT_MAP_TABLE.count(cn_num))
    cn_num = "一" + cn_num;
  string no_unit_result = ConvertNoUnitCNNum(cn_num);
  if (!no_unit_result.empty())
    return no_unit_result;

  return to_string(ConvertNormalCNNum(cn_num));
}

string ConvertNoUnitCNNum(const string& cn_num) {
  string arabic_num;
  for (int32 i = 0; i < cn_num.length(); i += CHINESE_CHAR_LEN) {
    string cur_cn_char = cn_num.substr(i, CHINESE_CHAR_LEN);
    if (UNIT_MAP_TABLE.count(cur_cn_char))
      return "";
    arabic_num += to_string(BASE_NUMBER_MAP_TABLE.find(cur_cn_char)->second);
  }

  return arabic_num;
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
      return ConvertNormalCNNum(sub_nums[0]) * unit_value + ConvertNormalCNNum(sub_nums[1]);
  }
}

string FindMaxUnit(const string& num) {
  for(const auto & unit : UNITS)
    if (num.find(unit) != string::npos)
      return unit;
  return "个";
}

string ProcessSent(const string &sent, const string& order, vector<int32> &ic_index) {
  string processed_sent;
  string last_num;
  for (int32 i = 0; i < sent.length(); i += CHINESE_CHAR_LEN) {
    if (find(ic_index.begin(), ic_index.end(), i) != ic_index.end()) {
      processed_sent += sent.substr(i, CHINESE_CHAR_LEN);;
      continue;
    }

    // Test for percentage and fraction
    bool is_percentage = false;
    bool is_fraction = false;
    string first_num;
    if (order == "last") {
      if (i >= 3 * CHINESE_CHAR_LEN && sent.substr(i - 3 * CHINESE_CHAR_LEN, 3 * CHINESE_CHAR_LEN) == "百分之") {
        is_percentage = true;
        processed_sent = processed_sent.substr(0, processed_sent.length() - 3 * CHINESE_CHAR_LEN);
      } else if (i >= 2 * CHINESE_CHAR_LEN && sent.substr(i - 2 * CHINESE_CHAR_LEN, 2 * CHINESE_CHAR_LEN) == "分之") {
        is_fraction = true;
        first_num = last_num;
      }
    }

    if (isdigit(sent[i]))
      last_num = "";
    while (isdigit(sent[i])) {
      processed_sent += sent[i];
      last_num += sent[i];
      i++;
    }

    string cur_char = sent.substr(i, CHINESE_CHAR_LEN);
    if (is_percentage)
      processed_sent += "%";
    else if (is_fraction) {
      processed_sent = processed_sent.substr(0, processed_sent.length() - first_num.length() - 2 * CHINESE_CHAR_LEN - last_num.length());
      processed_sent += (last_num.append( "/" + first_num));
    }

    if (order == "pre") {
      if (BIG_TO_SMALL.count(cur_char)) { // Big number to small number
        processed_sent += BIG_TO_SMALL.find(cur_char)->second;
      } else if (PRE_SPACIAL_MAP_TABLE.count(cur_char)) { // For special case
        processed_sent += PRE_SPACIAL_MAP_TABLE.find(cur_char)->second;
      } else {
        processed_sent += cur_char;
      }
    } else {
      bool check_dian = false;
      if (cur_char == "点")
        check_dian = CheckDateKeyword(sent, i);

      if (LAST_SPACIAL_MAP_TABLE.count(cur_char) && !check_dian) { // For special case
        if (cur_char != "比" || (i > 0 && isdigit(sent[i - 1])
            && i < sent.length() - CHINESE_CHAR_LEN && isdigit(sent[i + CHINESE_CHAR_LEN])))
          processed_sent += LAST_SPACIAL_MAP_TABLE.find(cur_char)->second;
        else
          processed_sent += cur_char;
      } else
        processed_sent += cur_char;
    }
  }

  return processed_sent;
}

string InverseNormalize(const string& sent) {
  // Initialize IDIOMS_AND_CI
  if (idiom_and_ci.empty())
    ReadFileByLine("data/idiom-and-ci.txt", idiom_and_ci);
  vector<int32> ic_index;
  CheckIdiomAndCi(sent, ic_index);
  string processed_sent = ProcessSent(sent, "pre", ic_index);
  vector<pair<string, int32>> nums_info;
  FindCNNums(processed_sent, nums_info, ic_index);

  int32 left_idx = 0;
  string result;
  for (const pair<string, int32>& num_info: nums_info) {
    result += (processed_sent.substr(left_idx, num_info.second - left_idx) + CNNumTranslation(num_info.first));
    left_idx = num_info.second + num_info.first.length();
  }

  if (left_idx != processed_sent.length())
    result += processed_sent.substr(left_idx, processed_sent.length() - left_idx);
  return ProcessSent(result, "last", ic_index);
}

}