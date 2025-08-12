#ifndef OPENSSLRANDOM_HPP
#define OPENSSLRANDOM_HPP

#include "Noncopyable.hpp"
#include <openssl/rand.h>
#include <vector>
#include <string>
using std::string;
using std::vector;

class OpenSSLRandom : public Noncopyable{
public:
    static OpenSSLRandom &getInstance();    // 获取实例

    // 基础随机数的生成
    bool GenerateRandomBytes(unsigned char* buf, size_t len);
    vector<unsigned char> GenerateRandomBytes(size_t len);

    // 专用盐值生成
    string GenerateSalt(size_t length);     // 生成盐值
    string GenerateSaltWithCharset(size_t length, const string &charset);   // 生成自定义字符集的盐值
    
    bool IsSecure() const; // 状态检查

private:
    OpenSSLRandom();
    ~OpenSSLRandom() = default;
    void ThrowIfNotSecure() const;
};

#endif