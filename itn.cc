#include "itn/itn.h"
#include "itn/itn-utils.h"

namespace itn {

using namespace std;

// Todo 星期
const map<string, int> BASE_NUMBER_MAP_TABLE = {
    {"零", 0}, {"一", 1}, {"二", 2}, {"三", 3}, {"四", 4},
    {"五", 5}, {"六", 6}, {"七", 7}, {"八", 8}, {"九", 9}
};
// Todo: 支持 兆京垓秭穰沟涧正载 一千千
const map<string, int> UNIT_MAP_TABLE = {
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
const map<string, string> LAST_SPACIAL_MAP_TABLE = { //点的处理放在代码中
    {"正", "+"}, {"负", "-"}, {"負", "-"}, {"比", ":"},
};
// For date
const vector<string> DATE_KEYWORD_BEFORE = {
    "时间", "上午", "中午", "下午", "晚上", "凌晨", "日",
};
const vector<string> DATE_KEYWORD_AFTER = {
    "分", "钟"
};

string ProcessForSegment(const string& sent) {
  string processed_sent;
  int last_index = 0;
  for (int i = 0; i < sent.length(); i++) {
    if (sent[i] == ' ') {
      bool delete_space = false;
      if (i < sent.length() - CHINESE_CHAR_LEN && i > CHINESE_CHAR_LEN - 1) {
        string pre_char = sent.substr(i - CHINESE_CHAR_LEN, CHINESE_CHAR_LEN);
        string after_char = sent.substr(i + 1, CHINESE_CHAR_LEN);
        if (((BASE_NUMBER_MAP_TABLE.count(pre_char) || UNIT_MAP_TABLE.count(pre_char) || pre_char == "点" || pre_char == "點" || BIG_TO_SMALL.count(pre_char) || PRE_SPACIAL_MAP_TABLE.count(pre_char))
            && (BASE_NUMBER_MAP_TABLE.count(after_char) || UNIT_MAP_TABLE.count(after_char) || after_char == "点" || after_char == "點") || BIG_TO_SMALL.count(after_char) || PRE_SPACIAL_MAP_TABLE.count(after_char))
            || (pre_char == "分" && after_char == "之")
            || i < sent.length() - 2 * CHINESE_CHAR_LEN && sent.substr(i + 1, 2 * CHINESE_CHAR_LEN) == "分之"
            || i < sent.length() - 2 * CHINESE_CHAR_LEN - 1 && sent.substr(i + 1, CHINESE_CHAR_LEN) + sent.substr(i + 2 + CHINESE_CHAR_LEN, CHINESE_CHAR_LEN) == "分之"
            || i > 2 * CHINESE_CHAR_LEN - 1 && sent.substr(i - 2 * CHINESE_CHAR_LEN, 2 * CHINESE_CHAR_LEN) == "分之"
            || i > 2 * CHINESE_CHAR_LEN && sent.substr(i - 2 * CHINESE_CHAR_LEN - 1, CHINESE_CHAR_LEN) + sent.substr(i - CHINESE_CHAR_LEN, CHINESE_CHAR_LEN) == "分之") {
          processed_sent += sent.substr(last_index, i - last_index);
          last_index = i + 1;
          delete_space = true;
        }
      }

      if (!delete_space) {
        processed_sent += sent.substr(last_index, i - last_index + 1);
        last_index = i + 1;
      }
    }
  }

  if (last_index < sent.length())
    processed_sent += sent.substr(last_index, sent.length() - last_index);

  return processed_sent;
}

bool IsValid(const std::string& cn_num) {
  // If the base number appears continuously and there is at least one unit, then it is not valid
  bool has_cont_base_num = false;
  bool has_unit = false;
  string last_char;
  string cur_cn_char;
  for (int i = 0; i < cn_num.length(); i += CHINESE_CHAR_LEN) {
    last_char = cur_cn_char;
    cur_cn_char = cn_num.substr(i, CHINESE_CHAR_LEN);
    if (last_char != "零" && BASE_NUMBER_MAP_TABLE.count(cur_cn_char) && BASE_NUMBER_MAP_TABLE.count(last_char))
      has_cont_base_num = true;
    if (UNIT_MAP_TABLE.count(cur_cn_char))
      has_unit = true;
  }

  return !(has_unit && has_cont_base_num);
}

void CheckIdiomAndCi(const string& sent, vector<int> &ic_index) {
  for (const auto& ic: idiom_and_ci) {
    int ic_idx;
    if ((ic_idx = sent.find(ic)) != string::npos) {
        // // Junyi mod
      // If the character around the idiom or ci is number or unit, then not save it
//      if (ic_idx - CHINESE_CHAR_LEN >= 0) {
//        string pre_cn_char = sent.substr(ic_idx - CHINESE_CHAR_LEN, CHINESE_CHAR_LEN);
//        if (BASE_NUMBER_MAP_TABLE.count(pre_cn_char) || UNIT_MAP_TABLE.count(pre_cn_char))
//          continue;
//      }
//
//      if (ic_idx + ic.length() + CHINESE_CHAR_LEN <= sent.length()) {
//        string after_cn_char = sent.substr(ic_idx + ic.length(), CHINESE_CHAR_LEN);
//        if (BASE_NUMBER_MAP_TABLE.count(after_cn_char) || UNIT_MAP_TABLE.count(after_cn_char))
//          continue;
//      }

      // Save the index information
      for (int i = ic_idx; i < ic_idx + ic.length(); i += CHINESE_CHAR_LEN)
        ic_index.emplace_back(i);
    }
  }
}

bool CheckDateKeyword(const string& sent, int index) {
  int start_index = 0;
  int end_index = sent.length();

  string last_cn_char;
  if (index >= 3)
    last_cn_char = sent.substr(index - CHINESE_CHAR_LEN, CHINESE_CHAR_LEN);
  if (index - CHINESE_CHAR_LEN >= 0 && (BASE_NUMBER_MAP_TABLE.count(last_cn_char) || UNIT_MAP_TABLE.count(last_cn_char))) {
    if (index - 5 * CHINESE_CHAR_LEN >= 0)
      start_index = index - 5 * CHINESE_CHAR_LEN;
    if (index + 5 * CHINESE_CHAR_LEN <= sent.length())
      end_index = index + 5 * CHINESE_CHAR_LEN;
  } else if (index - 1 >= 0 && isdigit(sent[index - 1])) {
    if (index - 2 * CHINESE_CHAR_LEN - 3 >= 0)
      start_index = index - 2 * CHINESE_CHAR_LEN - 3;
    if (index + 2 * CHINESE_CHAR_LEN + 3 <= sent.length())
      end_index = index + 2 * CHINESE_CHAR_LEN + 3;
  }

  string check_str_before = sent.substr(start_index, index - start_index);
  for (const auto& keyword: DATE_KEYWORD_BEFORE)
    if (check_str_before.find(keyword) != string::npos)
      return true;
  string check_str_after = sent.substr(index + CHINESE_CHAR_LEN, end_index - index - CHINESE_CHAR_LEN);
  for (const auto& keyword: DATE_KEYWORD_AFTER)
    if (check_str_after.find(keyword) != string::npos)
      return true;
  return false;
}

void FindCNNums(const string& sent, vector<pair<string, int>> &nums_info, vector<int> &ic_index) {
  int i = 0;
  string cn_num;
  string last_char;
  bool check_dian = false;
  while (i + CHINESE_CHAR_LEN - 1 < sent.length()) {
    while (sent[i] == ' ')
      i++;

    while (find(ic_index.begin(), ic_index.end(), i) != ic_index.end())
      i += CHINESE_CHAR_LEN;

    string cur_cn_char = sent.substr(i, CHINESE_CHAR_LEN);
    // Test for 点
    if ((cur_cn_char == "点" || cur_cn_char == "點") && i + CHINESE_CHAR_LEN < sent.length()
    && find(ic_index.begin(), ic_index.end(), i + CHINESE_CHAR_LEN) == ic_index.end()
    && (BASE_NUMBER_MAP_TABLE.count(sent.substr(i + CHINESE_CHAR_LEN, CHINESE_CHAR_LEN))
    || sent.substr(i + CHINESE_CHAR_LEN, CHINESE_CHAR_LEN) == "十"))
      check_dian = CheckDateKeyword(sent, i);

    while (find(ic_index.begin(), ic_index.end(), i) == ic_index.end() && (BASE_NUMBER_MAP_TABLE.count(cur_cn_char) || (UNIT_MAP_TABLE.count(cur_cn_char)
    && ((last_char != "点" && last_char != "點") || check_dian)))) {
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
      if (IsValid(cn_num) && cn_num != "百" && (!UNIT_MAP_TABLE.count(cn_num)
      || cn_num == "十" || i + 2 * CHINESE_CHAR_LEN <= sent.length() && sent.substr(i, 2 * CHINESE_CHAR_LEN) == "分之")) {
        nums_info.emplace_back(pair<string, int>(cn_num, i - cn_num.length()));
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
  // No unit one
  bool exist_num = false;
  for (int i = 0; i < cn_num.length(); i += CHINESE_CHAR_LEN) {
      if (!UNIT_MAP_TABLE.count(cn_num.substr(i, CHINESE_CHAR_LEN))) {
          exist_num = true;
          break;
      }
  }
  if (!exist_num)
    cn_num = "一" + cn_num;
  string no_unit_result = ConvertNoUnitCNNum(cn_num);
  if (!no_unit_result.empty())
    return no_unit_result;

  // Normal one
  return to_string(ConvertNormalCNNum(cn_num));
}

string ConvertNoUnitCNNum(const string& cn_num) {
  string arabic_num;
  for (int i = 0; i < cn_num.length(); i += CHINESE_CHAR_LEN) {
    string cur_cn_char = cn_num.substr(i, CHINESE_CHAR_LEN);
    if (UNIT_MAP_TABLE.count(cur_cn_char))
      return "";
    arabic_num += to_string(BASE_NUMBER_MAP_TABLE.find(cur_cn_char)->second);
  }

  return arabic_num;
}

long ConvertNormalCNNum(string sub_num) {
  if (sub_num.empty())
    return 0;

  string max_unit = FindMaxUnit(sub_num);
  int unit_value = UNIT_MAP_TABLE.find(max_unit)->second;
  // Process 个 and 十
  if (max_unit == "个")
    if (sub_num.length() != CHINESE_CHAR_LEN)
      return BASE_NUMBER_MAP_TABLE.find(sub_num.substr(CHINESE_CHAR_LEN, CHINESE_CHAR_LEN))->second;
    else
      return BASE_NUMBER_MAP_TABLE.find(sub_num)->second;
  else if (sub_num == "十")
    return 10;
  else {
    if (sub_num.substr(0, CHINESE_CHAR_LEN) == "十")
      sub_num = "一" + sub_num;
    vector<string> sub_nums;
    SplitByCNChar(sub_num, sub_nums, max_unit);

    // Recursive processing
    if (sub_nums[1].length() == CHINESE_CHAR_LEN)
      return ConvertNormalCNNum(sub_nums[0]) * unit_value + ConvertNormalCNNum(sub_nums[1]) * int(unit_value / 10);
    else {
      return ConvertNormalCNNum(sub_nums[0]) * unit_value + ConvertNormalCNNum(sub_nums[1]);
    }
  }
}

string FindMaxUnit(const string& num) {
  for(const auto & unit : UNITS)
    if (num.find(unit) != string::npos)
      return unit;
  return "个";
}

string ProcessSent(const string &sent, const string& order, vector<int> &ic_index) {
  string processed_sent;
  string last_num;
  for (int i = 0; i < sent.length(); i += CHINESE_CHAR_LEN) {
    // For space
    while (sent[i] == ' ') {
      i++;
      processed_sent += " ";
    }

    // Ignore the idiom and ci (词语)
    if (find(ic_index.begin(), ic_index.end(), i) != ic_index.end()) {
      processed_sent += sent.substr(i, CHINESE_CHAR_LEN);
      continue;
    }

    // Test for percentage and fraction when the order is last
    bool is_percentage = false;
    bool is_fraction = false;
    string first_num;
    if (order == "last") {
      if (i >= 3 * CHINESE_CHAR_LEN && sent.substr(i - 3 * CHINESE_CHAR_LEN, 3 * CHINESE_CHAR_LEN) == "百分之") {
        is_percentage = true;
        processed_sent = processed_sent.substr(0, processed_sent.length() - 3 * CHINESE_CHAR_LEN);
      } else if (isdigit(sent[i]) && i >= 1 + 2 * CHINESE_CHAR_LEN && sent.substr(i - 2 * CHINESE_CHAR_LEN, 2 * CHINESE_CHAR_LEN) == "分之"
      && isdigit(sent[i - 2 * CHINESE_CHAR_LEN - 1])) {
        is_fraction = true;
        first_num = last_num;
      }
    }

    // Process the digit in the sentence
    if (isdigit(sent[i]))
      last_num = "";
    while (isdigit(sent[i])) {
      processed_sent += sent[i];
      last_num += sent[i];
      i++;
      if (i + CHINESE_CHAR_LEN * 2 <= sent.length() && isdigit(sent[i + CHINESE_CHAR_LEN])
      && (sent.substr(i, CHINESE_CHAR_LEN) == "点" || sent.substr(i, CHINESE_CHAR_LEN) == "點")){
        if (CheckDateKeyword(sent, i))
          break;
        else {
          processed_sent += ".";
          i += CHINESE_CHAR_LEN;
        }
      }
    }

    // Process for percentage and fraction
    string cur_char = sent.substr(i, CHINESE_CHAR_LEN);
    if (is_percentage)
      processed_sent += "%";
    else if (is_fraction) {
      processed_sent = processed_sent.substr(0, processed_sent.length() - first_num.length() - 2 * CHINESE_CHAR_LEN - last_num.length());
      processed_sent += (last_num.append( "/" + first_num));
    }

    // Alternate some special characters
    if (order == "pre") {
      if (BIG_TO_SMALL.count(cur_char)) { // Big number to small number
        processed_sent += BIG_TO_SMALL.find(cur_char)->second;
      } else if (PRE_SPACIAL_MAP_TABLE.count(cur_char)) { // For special case
        if ((cur_char == "两" || cur_char == "兩")) {
          if (i + CHINESE_CHAR_LEN * 2 < sent.length()
            && UNIT_MAP_TABLE.count(sent.substr(i + CHINESE_CHAR_LEN, CHINESE_CHAR_LEN)))
            processed_sent += PRE_SPACIAL_MAP_TABLE.find(cur_char)->second;
          else
            processed_sent += cur_char;
        } else {
          processed_sent += PRE_SPACIAL_MAP_TABLE.find(cur_char)->second;
        }
      } else {
        processed_sent += cur_char;
      }
    } else {
      if (LAST_SPACIAL_MAP_TABLE.count(cur_char)) { // For special case
        if ((cur_char != "比" || (i > 0 && isdigit(sent[i - 1]))) && i < sent.length() - CHINESE_CHAR_LEN && isdigit(sent[i + CHINESE_CHAR_LEN]))
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
  // Initialize idiom_and_ci
  if (idiom_and_ci.empty())
    ReadFileByLine("data/idiom-and-ci.txt", idiom_and_ci);

  // Process for segmenting
  string processed_sent = ProcessForSegment(sent);

  // Get the information of idiom and ci (词语) in the sentence
  vector<int> ic_index;
  CheckIdiomAndCi(processed_sent, ic_index);

  // Process the sentence to translate from traditional to simplified, etc
  processed_sent = ProcessSent(processed_sent, "pre", ic_index);
  vector<pair<string, int>> nums_info;

  // Get the number information in the sentence, including the start index and the number string
  FindCNNums(processed_sent, nums_info, ic_index);

  // Translate the chinese number to arabic number
  int left_idx = 0;
  string result;
  for (const pair<string, int>& num_info: nums_info) {
    result += processed_sent.substr(left_idx, num_info.second - left_idx);
    string translation_result = CNNumTranslation(num_info.first);
    // Junyi mod
    if  ((translation_result.length() > num_info.first.length() / CHINESE_CHAR_LEN && translation_result.length() > 8) ||
    result.length() >= 2 * CHINESE_CHAR_LEN && result.substr(result.length() - CHINESE_CHAR_LEN, CHINESE_CHAR_LEN) == "点"
    && (BASE_NUMBER_MAP_TABLE.count(result.substr(result.length() - 2 * CHINESE_CHAR_LEN, CHINESE_CHAR_LEN))
    || UNIT_MAP_TABLE.count(result.substr(result.length() - 2 * CHINESE_CHAR_LEN, CHINESE_CHAR_LEN))))
      result += num_info.first;
    else
      result += translation_result;

    left_idx = num_info.second + num_info.first.length();
  }

  if (left_idx != processed_sent.length())
    result += processed_sent.substr(left_idx, processed_sent.length() - left_idx);

  // Last process the sentence and return
  return ProcessSent(result, "last", ic_index);
}

}