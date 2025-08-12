#include "SQLTemplate.hpp"
#include <iostream>
using namespace std;

int main() {
    try {
        SQLTemplate::instance().init("sql.json");  // 只需调用一次
    } catch (const std::exception& e) {
        std::cerr << "初始化失败: " << e.what() << std::endl;
        return 1;
    }

    unordered_map<string, string> params = {{"name", "Linus"}};
    std::string sql_1 = SQLTemplate::instance().get_sql("query", params);
    cout << sql_1 << endl;

    std::string sql_2 = SQLTemplate::instance().get_sql("select");
    cout << sql_2 << endl;
}