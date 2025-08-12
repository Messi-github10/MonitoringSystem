#ifndef SQLTEMPLATE_HPP
#define SQLTEMPLATE_HPP

#include "Noncopyable.hpp"
#include "nlohmann/json.hpp"
#include <string>
#include <unordered_map>
using std::string;
using std::unordered_map;

class SQLTemplate : public Noncopyable
{
public:

    static SQLTemplate& instance();

    void init(const string &_config_path);

    // 获取SQL并替换参数
    string get_sql(
        const string &sql_id,
        const unordered_map<string, string> &params) const;

    string get_sql(const string &sql_id) const;

private:
        SQLTemplate() = default;

private:
    unordered_map<string, string> _sql_map;
};

#endif