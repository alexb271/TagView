#pragma once

// standard library
#include <vector>
#include <set>
#include <fstream>
#include <exception>

// gtkmm
#include <glibmm/ustring.h>

class TagDb {
    public: class Item {
        public:
            friend class TagDb;
            enum class Type { image, video };

            Item(const Glib::ustring &file_path, const Type &type);
            Item(const Glib::ustring &file_path,
                 const Type &type,
                 const std::vector<Glib::ustring> &tags,
                 const bool &favorite);

            Glib::ustring get_file_path() const;
            Type get_type() const;

            void add_tag(const Glib::ustring &tag);
            void remove_tag(const Glib::ustring &tag);
            bool is_tagged(const Glib::ustring &tag) const;
            bool is_tagged(const std::set<Glib::ustring> &tags) const;
            const std::set<Glib::ustring> &get_tags() const;

            bool is_favorite() const;
            void set_favorite(const bool &favorite);

            bool operator< (const Item &other) const;
            friend std::ostream& operator<<(std::ostream &os, Item item);


        private:
            Glib::ustring file_path;
            Type type;
            std::set<Glib::ustring> tags;
            bool favorite;
    };

    public: class FileParseException : public std::exception {
        public:
            FileParseException(size_t line_number) : line_number(line_number) {}
            const char *what() const noexcept override {
                return "Error while parsing database file";
            }
            size_t line_number;
    };

    public: class ItemNotFoundException : public std::exception {
        public:
            ItemNotFoundException(Glib::ustring file_path) : file_path(file_path) {}
            const char *what() const noexcept override {
                return "Item could not be found in the database";
            }
            Glib::ustring file_path;
    };

    // main class implementation
    public:
        TagDb();

        void load_from_file(Glib::ustring db_file_path);
        void write_to_file() const;

        std::set<Glib::ustring> get_all_tags() const;
        const std::set<Glib::ustring> &get_tags_for_item(const Glib::ustring &file_path);

        std::vector<Glib::ustring> query(const std::set<Glib::ustring> &tags_include,
                                         const std::set<Glib::ustring> &tags_exclude) const;

        std::vector<Glib::ustring> top_tags(const std::vector<Glib::ustring> &tags_include,
                                            const std::vector<Glib::ustring> &tags_exclude,
                                            unsigned int count) const;
    private:
        // member variables
        Glib::ustring db_file_path;
        Glib::ustring prefix;
        std::vector<Item> items;

        // functions
        bool str_starts_with(const std::string &str, const std::string &argument);
        std::vector<Glib::ustring> parse_tags(const std::string &str);
};
