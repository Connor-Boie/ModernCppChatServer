#include "TextUtils.h"

namespace text_utils
{

void removeLineEnding(std::string& text)
{
    while (
        !text.empty()
        && (text.back() == '\n' || text.back() == '\r'))
    {
        text.pop_back();
    }
}

}