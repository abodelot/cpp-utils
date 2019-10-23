/*
Original implementation:
https://altctrlbackspace.blogspot.com/2009/03/c-utf8-to-wstring-conversion-routine.html

Copyright (c) 2009 SegFault aka "ErV" (altctrlbackspace.blogspot.com)

Redistribution and use of this source code, with or without modification, is
permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Utf8.hpp"

namespace utf8 {

std::wstring decode(const std::string& src)
{
    std::wstring dest;
    // Result is at least the size of input
    dest.reserve(src.size());
    wchar_t w = 0;
    int bytes = 0;
    wchar_t err = 65533; // Special error character
    for (size_t i = 0; i < src.size(); ++i) {
        unsigned char c = src[i];
        if (c <= 0x7f) { // first byte
            if (bytes) {
                dest.push_back(err);
                bytes = 0;
            }
            dest.push_back(c);
        }
        else if (c <= 0xbf) { // second/third/etc byte
            if (bytes) {
                w = (w << 6) | (c & 0x3f);
                --bytes;
                if (bytes == 0) {
                    dest.push_back(w);
                }
            }
            else {
                dest.push_back(err);
            }
        }
        else if (c <= 0xdf) { // 2 byte sequence start
            bytes = 1;
            w = c & 0x1f;
        }
        else if (c <= 0xef) { // 3 byte sequence start
            bytes = 2;
            w = c & 0x0f;
        }
        else if (c <= 0xf7) { // 3 byte sequence start
            bytes = 3;
            w = c & 0x07;
        }
        else {
            dest.push_back(err);
            bytes = 0;
        }
    }
    if (bytes) {
        dest.push_back(err);
    }
    return dest;
}


std::string encode(const std::wstring& src)
{
    std::string dest;
    for (size_t i = 0; i < src.size(); ++i) {
        wchar_t w = src[i];
        if (w <= 0x7f) {
            dest.push_back(w);
        }
        else if (w <= 0x7ff) {
            dest.push_back(0xc0 | ((w >> 6) & 0x1f));
            dest.push_back(0x80 | (w & 0x3f));
        }
        else if (w <= 0xffff) {
            dest.push_back(0xe0 | ((w >> 12) & 0x0f));
            dest.push_back(0x80 | ((w >> 6) & 0x3f));
            dest.push_back(0x80 | (w & 0x3f));
        }
        else if (w <= 0x10ffff) {
            dest.push_back(0xf0 | ((w >> 18) & 0x07));
            dest.push_back(0x80 | ((w >> 12) & 0x3f));
            dest.push_back(0x80 | ((w >> 6) & 0x3f));
            dest.push_back(0x80 | (w & 0x3f));
        }
        else {
            dest.push_back('?');
        }
    }
    return dest;
}

}
