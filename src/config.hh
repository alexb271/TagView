#pragma once

// standard library
#include <string>

// project
#include "previewgallery.hh"

class Config {
    public:
        Config();

        const std::string &get_default_db_path();
        void set_default_db_path(const std::string &path);
        PreviewGallery::PreviewSize get_preview_size();
        void set_preview_size(PreviewGallery::PreviewSize size);
        void write_to_file();

    private:
        std::string conf_path;
        std::string default_db_path;
        PreviewGallery::PreviewSize size;

        void parse_config();
};
