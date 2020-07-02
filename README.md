# inverse_chinese_text_normalization
将normalize过的中文文本，做逆向normalize。具体功能即实现 [chinese_text_normalization](https://github.com/speechio/chinese_text_normalization)的逆向版本。

代码思路可部分参考我之前项目的README.md部分：[chinese_number_recognition](https://github.com/Ajyy/chinese_number_recognition)。

测试文件 [itn-test.cc](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/itn-test.cc) 和 [itn-utils-test.cc](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/itn-utils-test.cc) , 请结合 [kaldi](https://github.com/kaldi-asr/kaldi) 和 [cppjieba](https://github.com/yanyiwu/cppjieba) 一起使用。

## 代码规范:

遵循Google c++代码规范。

## 文件结构

[itn.cc](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/itn.cc) / [itn.h](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/itn.h): 代码主体部分。

[itn-utils.cc](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/itn-utils.cc) / [itn-utils.h](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/itn-utils.h): 通用代码。

[itn-test.cc](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/itn-test.cc) / [itn-utils-test.cc](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/itn-utils-test.cc) : 测试代码。

[some-test-cases-result](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/some-test-cases-result
): 一些测试样例的结果。

[data/idiom-and-ci.txt](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/data/idiom-and-ci.txt): 从 [chinese-xinhua](https://github.com/pwxcoo/chinese-xinhua) 收集的有关数字的成语与词语。

[data/susie-test-cases.txt](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/data/susie-test-cases.txt) / [data/junyi-test-cases.txt](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/data/junyi-test-cases.txt) / [data/aishell-data.txt](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/data/aishell-data.txt): 一些测试样例。其中aishell-data是 [aishell1](http://www.aishelltech.com/kysjcp) 中抽取的包含数字的句子。

## 已完成部分

|NSW type|normalized|inverse normalized|
|-|-|-|
|cardinal|这块黄金重达三百二十四点七五克|这块黄金重达324.75克|
|date|她出生于八六年八月十八日她弟弟出生于一九九五年三月一日|她出生于86年8月18日她弟弟出生于1995年3月1日|
|digit|电影中梁朝伟扮演的陈永仁的编号二七一四九|电影中梁朝伟扮演的陈永仁的编号27149|
|money|随便来几个价格十二块五和三十四点五元和二十点一万|随便来几个价格12块5和34.5元和20.1万|
|percentage|明天有百分之六十二的概率降雨|明天有62％的概率降雨|
|telephone|这是固话零四二一三三四四一一二二<br>这是手机八六一八五四四一三九一二一|这是固话042133441122<br>这是手机8618544139121|
|fraction|现场有十二分之七的观众投出了赞成票|现场有7/12的观众投出了赞成票|

更多测试样例和分词后的句子结果，请参考 [some-test-cases-result](https://github.com/Ajyy/inverse_chinese_text_normalization/blob/master/some-test-cases-result
) 。

## 待完成部分

1. 更多单位支持，包括兆京垓秭穰沟涧正载。
2. 支持例如一千千的写法。
3. 支持标点符号。
