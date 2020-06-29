#include "base/kaldi-common.h"
#include "itn/itn.h"
#include "itn/itn-utils.h"

namespace itn {

using namespace std;

void UnitTestConvertNoUnitCNNum() {
  KALDI_ASSERT(ConvertNoUnitCNNum("一二三四五六七八") == "12345678");
  KALDI_ASSERT(ConvertNoUnitCNNum("八六一三五三零四九") == "861353049");
  KALDI_ASSERT(ConvertNoUnitCNNum("二零二零六二五") == "2020625");
}

void UnitTestFindMaxUnit() {
  KALDI_ASSERT(FindMaxUnit("一千零一") == "千");
  KALDI_ASSERT(FindMaxUnit("一亿两千万") == "亿");
  KALDI_ASSERT(FindMaxUnit("一") == "个");
}

void UnitTestFindCNNums() {
  vector<pair<string, int32>> nums_info;
  FindCNNums("二零一九年五月二十一", nums_info);
  KALDI_ASSERT(nums_info[0].first == "二零一九");
  KALDI_ASSERT(nums_info[1].first == "五");
  KALDI_ASSERT(nums_info[2].first == "二十一");
  KALDI_ASSERT(nums_info[0].second == 0);
  KALDI_ASSERT(nums_info[1].second == 15);
  KALDI_ASSERT(nums_info[2].second == 21);
}

void UnitTestPreprocessSent() {
  // Test for pre order
  KALDI_ASSERT(ProcessSent("壹贰叁肆伍陆柒捌玖貳參陸拾佰仟萬", "pre") == "一二三四五六七八九二三六十百千万");
  KALDI_ASSERT(ProcessSent("两兩幺", "pre") == "二二一");
  // Test for last order
  KALDI_ASSERT(ProcessSent("正负負點点", "last") == "+--..");
}

void UnitTestConvertNormalCNNum() {
  KALDI_ASSERT(ConvertNormalCNNum("一千一十") == 1010);
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
  KALDI_ASSERT(CNNumTranslation("一千一十") == "1010");
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
  ReadTestCases(file_name, test_cases);
  for (const string& test_case: test_cases) {
    if (test_case == "时间测试二零一八年五月二十三号上午十点十分") {
      cout << "111" << endl;
    }
    cout << test_case << endl;
    cout << InverseNormalize(test_case) << endl;
    cout << endl;
  }
}
}  // end namespace itn.

int main() {
  using namespace itn;
  UnitTestFindCNNums();
  UnitTestConvertNoUnitCNNum();
  UnitTestFindMaxUnit();
  UnitTestPreprocessSent();
  UnitTestConvertNormalCNNum();
  UnitTestCNNumTranslation();
  UnitTestInverseNormalize();
  UnitTestInverseNormalizeByInputFile("susie-test-cases.txt");
  return 0;
}
