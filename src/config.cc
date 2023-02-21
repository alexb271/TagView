// standard library
#include <fstream>

// project
#include "config.hh"

Config::Config() {
    std::string conf_path = std::getenv("HOME") + std::string("/.tagview");
}

const std::string &Config::get_default_db_path() {
    return default_db_path;
}

PreviewGallery::PreviewSize Config::get_size() {
    return size;
}

void Config::write_to_file() {

}
