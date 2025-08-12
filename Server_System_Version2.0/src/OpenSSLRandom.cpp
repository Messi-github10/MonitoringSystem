#include "OpenSSLRandom.hpp"
#include <openssl/err.h>
#include <stdexcept>

OpenSSLRandom &OpenSSLRandom::getInstance()
{
    static OpenSSLRandom instance;
    return instance;
}

bool OpenSSLRandom::GenerateRandomBytes(unsigned char *buf, size_t len)
{
    if (len == 0)
    {
        return true;
    }
    if (!buf)
    {
        return false;
    }
    ThrowIfNotSecure();
    return RAND_bytes(buf, static_cast<int>(len)) == 1;
}

vector<unsigned char> OpenSSLRandom::GenerateRandomBytes(size_t len)
{
    vector<unsigned char> buffer(len);
    if (len > 0)
    {
        ThrowIfNotSecure();
        if (RAND_bytes(buffer.data(), static_cast<int>(len)) != 1)
        {
            throw std::runtime_error("OpenSSL RAND_bytes failed");
        }
    }
    return buffer;
}

string OpenSSLRandom::GenerateSalt(size_t length)
{
    static const std::string DEFAULT_CHARSET = "0123456789"
                                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                               "abcdefghijklmnopqrstuvwxyz";
    return GenerateSaltWithCharset(length, DEFAULT_CHARSET);
}

string OpenSSLRandom::GenerateSaltWithCharset(size_t length, const string &charset)
{
    if (charset.empty())
    {
        throw std::invalid_argument("Charset cannot be empty");
    }

    auto randBytes = GenerateRandomBytes(length);
    string salt;
    salt.reserve(length);

    for(const auto & byte : randBytes){
        salt.push_back(charset[byte % charset.size()]);
    }
    
    return salt;
}

bool OpenSSLRandom::IsSecure() const
{
    return RAND_status() == 1;
}

OpenSSLRandom::OpenSSLRandom()
{
    if (RAND_status() != 1)
    {
        if (RAND_poll() != 1)
        {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("OpenSSL RAND initialization failed");
        }
    }
}

void OpenSSLRandom::ThrowIfNotSecure() const
{
    if (!IsSecure())
    {
        throw std::runtime_error("OpenSSL random generator not secure");
    }
}
