#include "itn/itn-utils.h"
#include "base/kaldi-types.h"

namespace itn {

using namespace std;

void SplitByCNChar(const string& s, vector<string>& tokens, const string& delimiters = " ") {
  int last_pos = 0;
  for (int32 i = 0; i < s.length(); i += CHINESE_CHAR_LEN) {
    string cur_char = s.substr(i, CHINESE_CHAR_LEN);
    if (cur_char == delimiters) {
      tokens.emplace_back(s.substr(last_pos, i - last_pos));
      last_pos = i + CHINESE_CHAR_LEN;
    }
  }
  if (s.substr(s.length() - CHINESE_CHAR_LEN, CHINESE_CHAR_LEN) != delimiters)
    tokens.emplace_back(s.substr(last_pos, s.length() - last_pos));
  else
    tokens.emplace_back("");
}

}
