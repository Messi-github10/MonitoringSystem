#pragma once

#include <curl/curl.h>
#include <map>
#include <string>
using std::map;
using std::string;

class HttpClient{
public:
    struct HttpResponse{
        int status_code = 0;
        string body;
        map<string, string> headers;
    };

    HttpClient(){
        _curl = curl_easy_init();
    }

    ~HttpClient(){
        if(_curl){
            curl_easy_cleanup(_curl);
        }
    }

    // GET请求
    HttpResponse get(const string &url, const map<string, string> &headers = {}){
        return request("GET", url, "", headers);
    }

    // POST请求
    HttpResponse post(const string &url, const string &data, const map<string, string> &headers = {}){
        return request("POST", url, data, headers);
    }

    // 设置超时事件（毫秒）
    void set_timeout(long timeout_ms){
        _timeout = timeout_ms;
    }

    // 全局初始化
    static void GlobalInit(){
        static bool initialized = false;
        if(!initialized){
            curl_global_init(CURL_GLOBAL_ALL);
            initialized = true;
        }
    }

private:
    static size_t WriteCallBack(void *contents, size_t size, size_t nmemb, void *userp){
        ((string *)userp)->append((char *)contents, size * nmemb);
        return size * nmemb;
    }

    static size_t HeaderCallBack(char *buffer, size_t size, size_t nitems, void *userdata){
        size_t total_size = size * nitems;
        string header_line(buffer, total_size);
        auto *headers = (map<string, string> *)userdata;
        auto pos = header_line.find(":");
        if(pos != string::npos){
            string key = header_line.substr(0, pos);
            string value = header_line.substr(pos + 1);

            // 去除首尾空格和换行
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n"));
            (*headers)[key] = value;
        }
        return total_size;
    }

    HttpResponse request(const string &method, const string &url, const string &data, const map<string, string> &headers_map){
        HttpResponse response;
        if(!_curl){
            return response;
        }

        curl_easy_reset(_curl);
        curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(_curl, CURLOPT_TIMEOUT_MS, _timeout);

        // 设置请求方法
        if(method == "POST"){
            curl_easy_setopt(_curl, CURLOPT_POST, 1L);
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data.c_str());
        }else if(method != "GET"){
            curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        }

        // 设置请求头
        struct curl_slist *chunk = nullptr;
        for(const auto& h : headers_map){
            string header = h.first + ":" + h.second;
            chunk = curl_slist_append(chunk, header.c_str());
        }
        if(chunk){
            curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, chunk);
        }

        // 设置响应body回调
        string body;
        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
        curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &body);

        // 设置响应header回调
        map<string, string> resp_headers;
        curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, HeaderCallBack);
        curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &resp_headers);

        // 执行请求
        CURLcode res = curl_easy_perform(_curl);

        // 获取状态码
        long http_code = 0;
        curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &http_code);

        if(chunk){
            curl_slist_free_all(chunk);
        }

        response.status_code = (int)http_code;
        response.body = body;
        response.headers = resp_headers;

        return response;
    }

private:
    CURL *_curl = nullptr;
    long _timeout = 3000;    // 默认3秒
};