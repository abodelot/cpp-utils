#ifndef MD5_HPP
#define MD5_HPP

#include <string>
#include <cstdint>

class MD5 {
public:
    MD5();

    MD5(const char* data, size_t len);

    void update(const char* data, size_t len);

    /**
     * Get digest as a 32 hexadecimal characters string
     */
    std::string hexdigest() const;

private:
    void transform();

    void finalize();

    uint32_t buffer_[4];
	uint32_t bits_[2];
	unsigned char in_[64];
    unsigned char digest_[16];
};

#endif
