#include "Http_Parse.hpp"
#include <iostream>

int main() {
#if 0
    std::string raw_request =
        "POST /api/test HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: application/json\r\n"
        "User-Agent: TestClient\r\n"
        "Content-Length: 17\r\n"
        "\r\n"
        "{\"a\":1,\"b\":2}\n";
#endif

    std::string raw_request =
        "POST /api/test HTTP/1.1\r\n"
        "\r\n"
        "{\"a\":1,\"b\":2}\n";

    HttpRequest req = HttpRequestParser::parse(raw_request);

    std::cout << "Method: " << req.method << std::endl;
    std::cout << "Path: " << req.path << std::endl;
    std::cout << "Version: " << req.version << std::endl;
    for (const auto& h : req.headers) {
        std::cout << h.first << ": " << h.second << std::endl;
    }
    std::cout << "Body: " << req.body << std::endl;
}