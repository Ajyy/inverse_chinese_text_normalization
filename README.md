# inverse_chinese_text_normalization
将normalize过的中文文本，做逆向normalize。具体功能参考自https://github.com/speechio/chinese_text_normalization.

代码思路参考我之前的项目：https://github.com/Ajyy/chinese_number_recognition.

## 代码规范与环境:

遵循Google c++代码规范，参考[kaldi](https://github.com/kaldi-asr/kaldi)环境配置。

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


可参考itn-test.cc文件内UnitTestInverseNormalize方法的测试用例。

## 待完成部分

1. 更多单位支持，包括兆京垓秭穰沟涧正载。
2. 支持标点符号。
