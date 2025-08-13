#include "HttpClient.hpp"
#include <iostream>
using namespace std;

// g++ Main.cpp -o HttpDemo -lcurl

int main(){
    HttpClient::GlobalInit();
    HttpClient client;
    auto resp = client.get("http://www.baidu.com");
    cout << "Status: " << resp.status_code << endl;
    cout << "Body: " << resp.body << endl;
    for(const auto & h : resp.headers){
        cout << h.first << ": " << h.second << endl;
    }
    return 0;
}