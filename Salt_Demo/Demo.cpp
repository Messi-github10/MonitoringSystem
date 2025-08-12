#include "OpenSSLRandom.hpp"
#include <iostream>
#include <string>
using namespace std;

int main()
{
    string salt1 = OpenSSLRandom::getInstance().GenerateSalt(8);
    cout << "salt: " << salt1 << endl;
    string setting = "$1$" + salt1;
    cout << "setting: " << setting << endl;
    size_t first_dollar = setting.find('$');
    if (first_dollar == string::npos)
    {
        cout << "failed" << endl;
    }

    size_t second_dollar = setting.find('$', first_dollar + 1);
    if (second_dollar == string::npos)
    {
        cout << "failed" << endl;
    }
    string salt2 = setting.substr(second_dollar + 1);
    cout << salt2 << endl;

    return 0;
}