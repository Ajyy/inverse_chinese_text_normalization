#include "base/kaldi-common.h"
#include "itn/itn.h"
#include "itn/itn-utils.h"
#include "cppjieba/Jieba.hpp"
#include "limonp/Config.hpp"

namespace itn {

using namespace std;

const char* const DICT_PATH = "jieba-dict/jieba.dict.utf8";
const char* const HMM_PATH = "jieba-dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "jieba-dict/user.dict.utf8";
const char* const IDF_PATH = "jieba-dict/idf.utf8";
const char* const STOP_WORD_PATH = "jieba-dict/stop_words.utf8";

cppjieba::Jieba jieba(DICT_PATH,
                      HMM_PATH,
                      USER_DICT_PATH,
                      IDF_PATH,
                      STOP_WORD_PATH);

void UnitTestConvertNoUnitCNNum() {
  KALDI_ASSERT(ConvertNoUnitCNNum("一二三四五六七八") == "12345678");
  KALDI_ASSERT(ConvertNoUnitCNNum("八六一三五三零四九") == "861353049");
  KALDI_ASSERT(ConvertNoUnitCNNum("二零二零六二五") == "2020625");
}

void UnitTestProcessForSegment() {
  KALDI_ASSERT(ProcessForSegment("这是 手机 八六 一八 五四 四一 三九 一二一") == "这是 手机 八六一八五四四一三九一二一");
  KALDI_ASSERT(ProcessForSegment("明天 有 百分之六十 二 的 概率 降雨") == "明天 有 百分之六十二 的 概率 降雨");
  KALDI_ASSERT(ProcessForSegment("随便 来 几个 价格 十二块 五 和 三十四点 五元 和 二十点 一万") == "随便 来 几个 价格 十二块 五 和 三十四点五元 和 二十点一万");
  KALDI_ASSERT(ProcessForSegment("十二 分 之 七") == "十二分之七");
  KALDI_ASSERT(ProcessForSegment("三十 二 点 七 四") == "三十二点七四");
  KALDI_ASSERT(ProcessForSegment("一点 一") == "一点一");
}

void UnitTestIsValid() {
  KALDI_ASSERT(!IsValid("八八十"));
  KALDI_ASSERT(IsValid("八百八十八"));
  KALDI_ASSERT(!IsValid("八八百"));
}

void UnitTestFindMaxUnit() {
  KALDI_ASSERT(FindMaxUnit("一千零一") == "千");
  KALDI_ASSERT(FindMaxUnit("一亿两千万") == "亿");
  KALDI_ASSERT(FindMaxUnit("一") == "个");
}

void UnitTestCheckDateKeyword() {
  KALDI_ASSERT(CheckDateKeyword("十点十分", 3));
  KALDI_ASSERT(!CheckDateKeyword("十点零四", 3));
}

void UnitTestFindCNNums() {
  vector<pair<string, int32>> nums_info;
  vector<int32> ic_index;
  CheckIdiomAndCi("二零一九年五月二十一", ic_index);
  FindCNNums("二零一九年五月二十一", nums_info, ic_index);
  KALDI_ASSERT(nums_info[0].first == "二零一九");
  KALDI_ASSERT(nums_info[1].first == "五");
  KALDI_ASSERT(nums_info[2].first == "二十一");
  KALDI_ASSERT(nums_info[0].second == 0);
  KALDI_ASSERT(nums_info[1].second == 15);
  KALDI_ASSERT(nums_info[2].second == 21);
}

void UnitTestPreprocessSent() {
  // Test for pre order
  vector<int32> ic_index;
  CheckIdiomAndCi("壹贰叁肆伍陆柒捌玖貳參陸拾佰仟萬", ic_index);
  KALDI_ASSERT(ProcessSent("壹贰叁肆伍陆柒捌玖貳參陸拾佰仟萬", "pre", ic_index) == "一二三四五六七八九二三六十百千万");
  ic_index = {};
  CheckIdiomAndCi("幺", ic_index);
  KALDI_ASSERT(ProcessSent("幺", "pre", ic_index) == "一");
  // Test for last order
  ic_index = {};
  CheckIdiomAndCi("正负負", ic_index);
  KALDI_ASSERT(ProcessSent("正负負", "last", ic_index) == "正负負");
}

void UnitTestConvertNormalCNNum() {
  KALDI_ASSERT(ConvertNormalCNNum("一千零一十") == 1010);
  KALDI_ASSERT(ConvertNormalCNNum("一百一十一") == 111);
  KALDI_ASSERT(ConvertNormalCNNum("一百一") == 110);
  KALDI_ASSERT(ConvertNormalCNNum("一亿一千万") == 110000000);
}

void UnitTestCNNumTranslation() {
  // Test for no unit chinese number
  KALDI_ASSERT(CNNumTranslation("一二三四五六七八") == "12345678");
  KALDI_ASSERT(CNNumTranslation("八六一三五三零四九") == "861353049");
  KALDI_ASSERT(CNNumTranslation("二零二零六二五") == "2020625");

  // Test for normal number
  KALDI_ASSERT(CNNumTranslation("一千零一十") == "1010");
  KALDI_ASSERT(CNNumTranslation("一百一十一") == "111");
  KALDI_ASSERT(CNNumTranslation("一百一") == "110");
  KALDI_ASSERT(CNNumTranslation("一亿一千万") == "110000000");
}

void UnitTestInverseNormalize() {
  // Cardinal
  KALDI_ASSERT(InverseNormalize("这块黄金重达三百二十四点七五克") == "这块黄金重达324.75克");
  // Date
  KALDI_ASSERT(InverseNormalize("她出生于八六年八月十八日她弟弟出生于一九九五年三月一日") == "她出生于86年8月18日她弟弟出生于1995年3月1日");
  // Digit
  KALDI_ASSERT(InverseNormalize("电影中梁朝伟扮演的陈永仁的编号二七一四九") == "电影中梁朝伟扮演的陈永仁的编号27149");
  // Fraction
  KALDI_ASSERT(InverseNormalize("现场有十二分之七的观众投出了赞成票") == "现场有7/12的观众投出了赞成票");
  // Money
  KALDI_ASSERT(InverseNormalize("随便来几个价格十二块五和三十四点五元和二十点一万") == "随便来几个价格12块5和34.5元和20.1万");
  // Percentage
  KALDI_ASSERT(InverseNormalize("明天有百分之六十二的概率降雨") == "明天有62%的概率降雨");
  // Telephone
  KALDI_ASSERT(InverseNormalize("这是固话零四二一三三四四一一二二") == "这是固话042133441122");
  KALDI_ASSERT(InverseNormalize("这是手机八六一八五四四一三九一二一") == "这是手机8618544139121");
}

void UnitTestInverseNormalizeByInputFile(const string& file_name) {
  vector<string> test_cases;
  ReadFileByLine(file_name, test_cases);
  for (const string& test_case: test_cases) {
    cout << "Original:         " << test_case << endl;
    cout << "Result:           " << InverseNormalize(test_case) << endl;

    vector<string> words;
    jieba.Cut(test_case, words, true);
    string seg_test_case = limonp::Join(words.begin(), words.end(), " ");
    cout << "After Segmenting: " << seg_test_case << endl;
    cout << "Result:           " << InverseNormalize(seg_test_case) << endl;
    cout << endl;
  }
}

void UnitTestSpecialCase() {
    KALDI_ASSERT(InverseNormalize("四川") == "四川");
    KALDI_ASSERT(InverseNormalize("三亚") == "三亚");
    KALDI_ASSERT(InverseNormalize("第五、六两层") == "第5、6两层");
    KALDI_ASSERT(InverseNormalize("收获千万粒") == "收获千万粒");
    KALDI_ASSERT(InverseNormalize("七七八八") == "7788");
    KALDI_ASSERT(InverseNormalize("十千卡") == "10千卡");
    KALDI_ASSERT(InverseNormalize("三千千瓦") == "3000千瓦");
    KALDI_ASSERT(InverseNormalize("还差一刻钟到五点") == "还差一刻钟到5点");
    KALDI_ASSERT(InverseNormalize("帮我称二两的决明子") == "帮我称2两的决明子");
    KALDI_ASSERT(InverseNormalize("长苞冰杉、丽江铁杉、桃儿七、八角莲、四川牡丹、金铁锁等，特别是地狱谷中的情人树（铁杉、高山栎同根）堪称植物界奇观。")
    == "长苞冰杉、丽江铁杉、桃儿七、八角莲、四川牡丹、金铁锁等，特别是地狱谷中的情人树（铁杉、高山栎同根）堪称植物界奇观。");
    KALDI_ASSERT(InverseNormalize("四两棉花一张弓") == "4两棉花1张弓");
//    KALDI_ASSERT(InverseNormalize("当地人依次称其中一些地方一道桥、二道桥、八道桥。一道桥、二道桥和七道桥是理想的胡杨林观光区，八道桥的尽头即是巴丹吉林沙漠。")
//    == "当地人依次称其中一些地方一道桥、二道桥、八道桥。一道桥、二道桥和七道桥是理想的胡杨林观光区，八道桥的尽头即是巴丹吉林沙漠。");
    KALDI_ASSERT(InverseNormalize("不怕一万，就怕万一") == "不怕一万，就怕万一");
}

}  // end namespace itn.

int main() {
  using namespace itn;
  UnitTestFindCNNums();
  UnitTestConvertNoUnitCNNum();
  UnitTestIsValid();
  UnitTestCheckDateKeyword();
  UnitTestFindMaxUnit();
  UnitTestPreprocessSent();
  UnitTestConvertNormalCNNum();
  UnitTestCNNumTranslation();
  UnitTestProcessForSegment();
  UnitTestInverseNormalize();
  UnitTestInverseNormalizeByInputFile("data/susie-test-cases.txt");
  UnitTestInverseNormalizeByInputFile("data/junyi-test-cases.txt");
  UnitTestSpecialCase();
  return 0;
}
