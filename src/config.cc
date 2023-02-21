// standard library
#include <fstream>

// project
#include "config.hh"

Config::Config()
:
    size(PreviewGallery::PreviewSize::Medium)
{
    conf_path = std::getenv("HOME") + std::string("/.tagview");
    std::ifstream conf_file(conf_path);
    if (conf_file.good()) {
        std::string line;
        while (getline(conf_file, line)) {
            if (line.rfind("[db]", 0) == 0) {
                default_db_path = line.substr(4);
            }
            else if (line.rfind("[size]", 0) == 0) {
                std::string size_str = line.substr(6);
                if (size_str == "small") {
                    size = PreviewGallery::PreviewSize::Small;
                }
                else if (size_str == "medium") {
                    size = PreviewGallery::PreviewSize::Medium;
                }
                else if (size_str == "large") {
                    size = PreviewGallery::PreviewSize::Large;
                }
            }
        }
    }
}

const std::string &Config::get_default_db_path() {
    return default_db_path;
}

void Config::set_default_db_path(const std::string &path) {
    default_db_path = path;
    write_to_file();
}

PreviewGallery::PreviewSize Config::get_preview_size() {
    return size;
}

void Config::set_preview_size(PreviewGallery::PreviewSize size) {
    this->size = size;
    write_to_file();
}

void Config::write_to_file() {
    std::ofstream output(conf_path);
    if (output.good()) {
        if (default_db_path.size() > 0) {
            output << "[db]" << default_db_path << std::endl;
        }
        output << "[size]";
        switch(size) {
            case PreviewGallery::PreviewSize::Small: output << "small"; break;
            case PreviewGallery::PreviewSize::Medium: output << "medium"; break;
            case PreviewGallery::PreviewSize::Large: output << "large"; break;
        }
    }
}

void Config::parse_config() {

}
