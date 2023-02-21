#pragma once

// standard library
#include <string>

// project
#include "previewgallery.hh"

class Config {
    public:
        Config();

        const std::string &get_default_db_path();
        PreviewGallery::PreviewSize get_size();
        void write_to_file();

    private:
        std::string conf_path;
        std::string default_db_path;
        PreviewGallery::PreviewSize size;

        void parse_config();
};
