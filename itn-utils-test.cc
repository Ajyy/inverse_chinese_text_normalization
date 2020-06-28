#include "base/kaldi-common.h"
#include "itn/itn-utils.h"

namespace itn {

using namespace std;

void UnitTestSplitByCNChar() {
  vector<string> tokens;
  SplitByCNChar("千一千二十千三千", tokens, "千");
  KALDI_ASSERT(tokens[0].empty());
  KALDI_ASSERT(tokens[1] == "一");
  KALDI_ASSERT(tokens[2] == "二十");
  KALDI_ASSERT(tokens[3] == "三");
  KALDI_ASSERT(tokens[4].empty());
}

}  // end namespace kaldi.

int main() {
  using namespace itn;
  UnitTestSplitByCNChar();
  return 0;
}
