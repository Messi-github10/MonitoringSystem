#pragma once

#include <string>
#include <map>
#include <sstream>
#include <algorithm>
using namespace std;

struct HttpRequest
{
    string method;
    string path;
    string version;
    map<string, string> headers;
    string body;
};

class HttpRequestParser
{
public:
    static HttpRequest parse(const string &raw)
    {
        HttpRequest req;
        istringstream stream(raw);
        string line;

        // 解析请求行
        if (getline(stream, line))
        {
            istringstream req_line(line);
            req_line >> req.method >> req.path >> req.version;
        }

        // 解析请求头
        while (std::getline(stream, line))
        {
            if (!line.empty() && line.back() == '\r'){
                line.pop_back();
            }
            if (line.empty()){
                break; // 空行表示头部结束
            }
            auto pos = line.find(':');
            if (pos != std::string::npos)
            {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                // 去除首尾空格
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                req.headers[key] = value;
            }
        }

        // 解析请求体
        ostringstream body_stream;
        while (getline(stream, line))
        {
            body_stream << line << "\n";
        }
        req.body = body_stream.str();
        if (!req.body.empty() && req.body.back() == '\n')
        {
            req.body.pop_back();
        }
        return req;
    }
};