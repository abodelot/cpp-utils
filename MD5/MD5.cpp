// This code implements the MD5 message-digest algorithm.
// The algorithm is due to Ron Rivest.  This code was
// written by Colin Plumb in 1993, no copyright is claimed.
// This code is in the public domain; do with it what you wish.

// Equivalent code is available from RSA Data Security, Inc. This code has been
// tested against that, and is equivalent, except that you don't need to include
// two pages of legalese with every copy.

// This code was modified in 1997 by Jim Kingdon of Cyclic Software to not
// require an integer type which is exactly 32 bits. This work draws on the
// changes for the same purpose by Tatu Ylonen <ylo@cs.hut.fi> as part of SSH,
// but since I didn't actually use that code, there is no copyright issue.
// I hereby disclaim copyright in any changes I have made; this code remains in
// the public domain.

// 2019 - Alexandre Bodelot
// Converted to C++ with a class interface and added hexdigest method.
// Original C implementation is available at:
// https://opensource.apple.com/source/cvs/cvs-19/cvs/lib/md5.[c|h]

#include "md5.hpp"
#include <cstring>

/*
 * Little-endian byte-swapping routines. Note that these do not depend on the
 * size of datatypes such as uint32_t, nor do they require us to detect the
 * endianness of the machine we are running on. It is possible they should be
 * macros for speed, but I would be surprised if they were a performance
 * bottleneck for MD5.
 */
static uint32_t getu32(const unsigned char *addr)
{
    return (((((unsigned long)addr[3] << 8) | addr[2]) << 8)
        | addr[1]) << 8 | addr[0];
}

static void putu32(uint32_t data, unsigned char* addr)
{
    addr[0] = (unsigned char) data;
    addr[1] = (unsigned char) (data >> 8);
    addr[2] = (unsigned char) (data >> 16);
    addr[3] = (unsigned char) (data >> 24);
}

/*
 * Start MD5 accumulation. Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
MD5::MD5()
{
    buffer_[0] = 0x67452301;
    buffer_[1] = 0xefcdab89;
    buffer_[2] = 0x98badcfe;
    buffer_[3] = 0x10325476;

    bits_[0] = 0;
    bits_[1] = 0;
}

MD5::MD5(const char* data, size_t len)
{
    update(data, len);
}

void MD5::update(const char* _data, size_t len)
{
    const unsigned char* data = (const unsigned char*) _data;

    /* Update bitcount */
    uint32_t t = bits_[0];
    if ((bits_[0] = (t + (len << 3)) & 0xffffffff) < t) {
        bits_[1]++;    /* Carry from low to high */
    }
    bits_[1] += len >> 29;

    /* Bytes already in shsInfo->data */
    t = (t >> 3) & 0x3f;

    /* Handle any leading odd-sized chunks */
    if (t) {
        unsigned char *p = in_ + t;

        t = 64-t;
        if (len < t) {
            memcpy(p, data, len);
            return;
        }
        memcpy(p, data, t);
        transform();
        data += t;
        len -= t;
    }

    /* Process data in 64-byte chunks */
    while (len >= 64) {
        memcpy(in_, data, 64);
        transform();
        data += 64;
        len -= 64;
    }

    /* Handle any remaining bytes of data. */
    memcpy(in_, data, len);

    finalize();
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
void MD5::finalize()
{
    /* Compute number of bytes mod 64 */
    unsigned int count = (bits_[0] >> 3) & 0x3F;

    /* Set the first char of padding to 0x80.  This is safe since there is
       always at least one byte free */
    unsigned char* p = in_ + count;
    *p++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if (count < 8) {
        /* Two lots of padding:  Pad the first block to 64 bytes */
        std::memset(p, 0, count);
        transform();

        /* Now fill the next block with 56 bytes */
        std::memset(in_, 0, 56);
    }
    else {
        /* Pad block to 56 bytes */
        std::memset(p, 0, count - 8);
    }

    /* Append length in bits and transform */
    putu32(bits_[0], in_ + 56);
    putu32(bits_[1], in_ + 60);

    transform();
    putu32(buffer_[0], digest_);
    putu32(buffer_[1], digest_ + 4);
    putu32(buffer_[2], digest_ + 8);
    putu32(buffer_[3], digest_ + 12);
}

/* The four core functions - F1 is optimized somewhat */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
    (w += f(x, y, z) + data, w &= 0xffffffff, w = w << s | w >> (32-s), w += x)

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
void MD5::transform()
{
    uint32_t in[16];

    for (int i = 0; i < 16; ++i) {
        in[i] = getu32(in_ + 4 * i);
    }

    uint32_t a = buffer_[0];
    uint32_t b = buffer_[1];
    uint32_t c = buffer_[2];
    uint32_t d = buffer_[3];

    MD5STEP(F1, a, b, c, d, in[ 0] + 0xd76aa478,  7);
    MD5STEP(F1, d, a, b, c, in[ 1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[ 2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[ 3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[ 4] + 0xf57c0faf,  7);
    MD5STEP(F1, d, a, b, c, in[ 5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[ 6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[ 7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[ 8] + 0x698098d8,  7);
    MD5STEP(F1, d, a, b, c, in[ 9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122,  7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[ 1] + 0xf61e2562,  5);
    MD5STEP(F2, d, a, b, c, in[ 6] + 0xc040b340,  9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[ 0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[ 5] + 0xd62f105d,  5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453,  9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[ 4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[ 9] + 0x21e1cde6,  5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6,  9);
    MD5STEP(F2, c, d, a, b, in[ 3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[ 8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905,  5);
    MD5STEP(F2, d, a, b, c, in[ 2] + 0xfcefa3f8,  9);
    MD5STEP(F2, c, d, a, b, in[ 7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[ 5] + 0xfffa3942,  4);
    MD5STEP(F3, d, a, b, c, in[ 8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[ 1] + 0xa4beea44,  4);
    MD5STEP(F3, d, a, b, c, in[ 4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[ 7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6,  4);
    MD5STEP(F3, d, a, b, c, in[ 0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[ 3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[ 6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[ 9] + 0xd9d4d039,  4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[ 2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[ 0] + 0xf4292244,  6);
    MD5STEP(F4, d, a, b, c, in[ 7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[ 5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3,  6);
    MD5STEP(F4, d, a, b, c, in[ 3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[ 1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[ 8] + 0x6fa87e4f,  6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[ 6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[ 4] + 0xf7537e82,  6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[ 2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[ 9] + 0xeb86d391, 21);

    buffer_[0] += a;
    buffer_[1] += b;
    buffer_[2] += c;
    buffer_[3] += d;
}

std::string MD5::hexdigest() const
{
    std::string hex;
    hex.reserve(32);
    char buffer[3]; // 2 digits + '\0'
    for (int i = 0; i < 16; ++i) {
        std::snprintf(buffer, 3, "%02x", digest_[i]);
        hex += buffer;
    }
    return hex;
}
