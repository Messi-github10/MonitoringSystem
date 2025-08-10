#pragma once

#include "Noncopyable.hpp"
#include <libconfig.h++>
#include <mutex>
#include <iostream>
#include <string>
#include <vector>

class Configuration : public Noncopyable {
public:
    static Configuration& getInstance() {
        static Configuration instance;
        return instance;
    }

    void loadConfig(const std::string& filename) {
        std::lock_guard<std::mutex> lock(cfg_mutex);
        try {
            cfg.readFile(filename.c_str());
            std::cerr << "Config loaded: " << filename << std::endl;
        } catch (const libconfig::FileIOException& e) {
            std::cerr << "File I/O error: " << e.what() << std::endl;
            throw;
        } catch (const libconfig::ParseException& e) {
            std::cerr << "Parse error (" << e.getFile() << ":" << e.getLine() << "): " << e.getError() << std::endl;
            throw;
        }
    }

    int getInt(const std::string& path, int default_value = 0) {
        std::lock_guard<std::mutex> lock(cfg_mutex);
        try {
            if (!cfg.exists(path)) {
                std::cerr << "Warning: Config path '" << path << "' not found, using default: " << default_value << std::endl;
                return default_value;
            }

            const auto& setting = cfg.lookup(path);
            if (!setting.isNumber() || setting.getType() != libconfig::Setting::TypeInt) {
                std::cerr << "Error: Config '" << path << "' is not an integer, using default: " << default_value << std::endl;
                return default_value;
            }
            return setting;
        } catch (...) {
            std::cerr << "Error: Failed to read '" << path << "', using default: " << default_value << std::endl;
            return default_value;
        }
    }

    std::string getString(const std::string& path, const std::string& default_value = "") {
        std::lock_guard<std::mutex> lock(cfg_mutex);
        try {
            if (!cfg.exists(path)) {
                std::cerr << "Warning: Config path '" << path << "' not found, using default: '" << default_value << "'" << std::endl;
                return default_value;
            }

            const auto& setting = cfg.lookup(path);
            if (setting.getType() != libconfig::Setting::TypeString) {
                std::cerr << "Error: Config '" << path << "' is not a string, using default: '" << default_value << "'" << std::endl;
                return default_value;
            }
            return setting.c_str();
        } catch (...) {
            std::cerr << "Error: Failed to read '" << path << "', using default: '" << default_value << "'" << std::endl;
            return default_value;
        }
    }

    bool validate(const std::vector<std::string>& required_keys) {
        std::lock_guard<std::mutex> lock(cfg_mutex);
        for (const auto& key : required_keys) {
            if (!cfg.exists(key)) {
                std::cerr << "Missing required key: " << key << std::endl;
                return false;
            }
        }
        return true;
    }

private:
    Configuration() = default;
    ~Configuration() = default;

    libconfig::Config cfg;
    std::mutex cfg_mutex;
};