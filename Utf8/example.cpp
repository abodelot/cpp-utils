#include "Utf8.hpp"
#include <iostream>


void decode(const std::string& content)
{
    std::wstring wcontent = utf8::decode(content);
    std::cout << "'" << content << "': utf8 string size = " << content.size()
        << ", character count = " << wcontent.size() << std::endl;
}


int main()
{
    std::cout << "Decode utf-8" << std::endl;
    decode("foobar");
    decode("éléphant");
    decode("攻殻機動隊");

    std::cout << std::endl << "Encode utf-8" << std::endl;
    std::wstring unicode;
    unicode += 224;
    unicode += 233;
    unicode += 249;
    unicode += 27231;
    std::cout << utf8::encode(unicode) << std::endl;
    return 0;
}
