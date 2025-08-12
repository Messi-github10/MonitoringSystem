#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <map>
#include <string>

using namespace std;


class Configuration
{
    class AutoRelease
    {
    public:
        AutoRelease(){}
        ~AutoRelease() {
            if(_pInstance) {
                delete _pInstance;
                _pInstance = nullptr;
            }
        }
    };
public:
    static Configuration * getInstance();
	map<string, string> & getConfigMap();
    void display() const;
private:
    Configuration();
    ~Configuration();

	void readConfiguration();
private:
	map<string, string> _configMap;
    static Configuration * _pInstance;
    static AutoRelease _ar;
};



#endif
