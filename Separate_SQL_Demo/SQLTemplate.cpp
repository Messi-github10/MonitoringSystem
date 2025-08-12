#include "SQLTemplate.hpp"
#include <fstream>
#include <regex>
using std::ifstream;
using json = nlohmann::json;

SQLTemplate &SQLTemplate::instance()
{
    static SQLTemplate instance;
    return instance;
}

void SQLTemplate::init(const string &_config_path)
{
    ifstream ifs(_config_path);
    if(!ifs){
        throw std::runtime_error("Cannot open SQL config: " + _config_path);
    }

    json config = json::parse(ifs);
    
    for(auto & [key, value] : config.items()){
        _sql_map[key] = value.get<string>();
    }
}

string SQLTemplate::get_sql(
    const string &sql_id,
    const unordered_map<string, string> &params) const
{
    auto it = _sql_map.find(sql_id);
    if(it == _sql_map.end()){
        throw std::runtime_error("SQL template not found: " + sql_id);
    }

    string sql = it->second;
    for(const auto & [key, value] : params){
        std::regex placeholder(":" + key);
        sql = std::regex_replace(sql, placeholder, "'" + value + "'");
    }
    return sql;
}

string SQLTemplate::get_sql(const string &sql_id) const{
    auto it = _sql_map.find(sql_id);
    if(it == _sql_map.end()){
        throw std::runtime_error("SQL not found");
    }
    return it->second;
}
