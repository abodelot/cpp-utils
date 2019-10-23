#ifndef UTILS_UTF8_HPP
#define UTILS_UTF8_HPP

#include <string>

namespace utf8 {

std::string encode(const std::wstring& src);

std::wstring decode(const std::string& str);

}

#endif
