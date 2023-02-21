// standard library
#include <map>
#include <tuple>
#include <algorithm>
#include <filesystem>
#include <utility>

// project
#include "tagdb.hh"

// TagDb::Item implementation
TagDb::Item::Item(const Glib::ustring &file_path, const Type &type)
:
    file_path(file_path),
    type(type),
    favorite(false)
{}

TagDb::Item::Item(const Glib::ustring &file_path,
     const Type &type,
     const std::set<Glib::ustring> &tags,
     const bool &favorite)
:
    file_path(file_path),
    type(type),
    favorite(favorite)
{
    for (Glib::ustring tag : tags) {
        this->tags.insert(tag);
    }
}

void TagDb::Item::add_tag(const Glib::ustring &tag) {
    tags.insert(tag);
}

void TagDb::Item::remove_tag(const Glib::ustring &tag) {
    tags.erase(tag);
}

void TagDb::Item::set_tags(const std::set<Glib::ustring> &tags) {
    this->tags = tags;
}

bool TagDb::Item::is_tagged(const Glib::ustring &tag) const {
    // count can only return 0 or 1
    // implicit conversion to bool possible
    return tags.count(tag);
}

bool TagDb::Item::is_tagged(const std::set<Glib::ustring> &tags) const {
        for (Glib::ustring tag : tags) {
            if (is_tagged(tag)) {
                return true;
            }
        }
        return false;
}

const std::set<Glib::ustring> &TagDb::Item::get_tags() const {
    return tags;
}

void TagDb::Item::set_file_path(const Glib::ustring &file_path) {
    this->file_path = file_path;
}

const Glib::ustring &TagDb::Item::get_file_path() const {
    return file_path;
}

void TagDb::Item::set_type(TagDb::Item::Type type) {
    this->type = type;
}

TagDb::Item::Type TagDb::Item::get_type() const {
    return type;
}

bool TagDb::Item::get_favorite() const {
    return favorite;
}

void TagDb::Item::set_favorite(const bool &favorite) {
    this->favorite = favorite;
}

bool TagDb::Item::operator<(const TagDb::Item &other) const {
    if (this->favorite && (!other.favorite)) {
        return true;
    }
    else if ((!this->favorite) && other.favorite) {
        return false;
    }
    else {
        return this->file_path < other.file_path;
    }
}

std::ostream& operator<<(std::ostream &os, TagDb::Item item) {
    os << "[item]" << std::endl;
    os << "[path]" << item.file_path.raw() << std::endl;

    if (item.type == TagDb::Item::Type::image)
        os << "[type]image" << std::endl;
    else
        os << "[type]video" << std::endl;

    os << "[tags]";
    for (Glib::ustring tag : item.tags) {
        os << tag.raw() << ',';
    }
    os << std::endl;

    if (item.favorite)
        os << "[fave]yes" << std::endl;
    else
        os << "[fave]no" << std::endl;

    os << std::endl;

    return os;
}

// TagDb implementation
TagDb::TagDb()
{}

void TagDb::load_from_file(std::string db_file_path) {
    // variables for reading from file
    std::string line;
    std::ifstream input(db_file_path);

    if (!input.good()) {
        throw FileErrorException(db_file_path);
    }

    // store file path
    this->db_file_path = db_file_path;

    // set the prefix to the directory that the file is located in
    prefix = db_file_path.substr(0, db_file_path.find_last_of("/") + 1);

    // clear any current items
    items.clear();

    // buffer variables
    Glib::ustring file_path = "";
    TagDb::Item::Type type = TagDb::Item::Type::image;
    std::set<Glib::ustring> tags;
    bool favorite = false;

    // check first line for header
    std::getline(input, line);
    if (line != "[TagView database file]") {
        input.close();
        throw TagDb::FileParseException(1);
    }

    // parse the rest of the file
    size_t line_number = 1;
    while (getline(input, line)) {
        line_number += 1;

        // use the # character for comments
        if (line[0] == '#') continue;

        // ignore empty lines
        if (line.length() == 0) continue;

        // otherwise, if the line does not
        // start with a [ character, throw error
        if (line[0] != '[') {
            input.close();
            throw TagDb::FileParseException(line_number);
        }

        // beginning a new item
        // submit current buffer if vialbe
        // clear buffer variables
        if (line == "[item]") {
            if (file_path.length() != 0 && tags.size() != 0) {
                // add new item
                items.push_back(TagDb::Item(file_path, type, tags, favorite));

                // reset buffer variables
                file_path.clear();
                type = TagDb::Item::Type::image;
                tags.clear();
                favorite = false;
            }
        }

        // check all possible entry types
        else if (str_starts_with(line, "[path]")) {
            file_path = line.substr(6);
        }

        else if (str_starts_with(line, "[type]")) {
            if (line.substr(6) == "image") {
                type = TagDb::Item::Type::image;
            }
            else if (line.substr(6) == "video") {
                type = TagDb::Item::Type::video;
            }
            else {
                throw FileParseException(line_number);
            }
        }

        else if (str_starts_with(line, "[tags]")) {
            tags = parse_tags(line.substr(6));
        }

        else if (str_starts_with(line, "[fave]")) {
            if (line.substr(6) == "yes") {
                favorite = true;
            }
            else if (line.substr(6) == "no") {
                favorite = false;
            }
            else {
                throw FileParseException(line_number);
            }
        }

        else if (str_starts_with(line, "[dir]")) {
            directories.insert(line.substr(5));
        }

        else if (str_starts_with(line, "[exclude]")) {
            for (const Glib::ustring &tag : parse_tags(line.substr(9))) {
                default_excluded_tags.insert(tag);
            }
        }

        else {
            throw FileParseException(line_number);
        }
    }

    // at the end of file, if buffer variables are valid
    // add last entry to database
    if (file_path.length() != 0 && tags.size() != 0) {
        // add new item
        items.push_back(TagDb::Item(file_path, type, tags, favorite));
    }
}

void TagDb::write_to_file() const {
    std::ofstream output(db_file_path);

    if (!output.good()) {
        throw FileErrorException(db_file_path);
    }

    output << "[TagView database file]" << std::endl << std::endl;

    for (const Glib::ustring &dir : directories) {
        output << "[dir]" << dir << std::endl;
    }

    output << "[exclude]";
    for (const Glib::ustring &tag : default_excluded_tags) {
        output << tag.raw() << ' ';
    }

    output << std::endl << std::endl;

    for (const TagDb::Item &item : items) {
        output << item;
    }

    output.close();
}

void TagDb::add_item(TagDb::Item &item) {
    // remove entry if it already in the database
    for (size_t idx = 0; idx < items.size(); idx++) {
        if (items[idx].get_file_path() == item.get_file_path()) {
            items.erase(items.begin() + idx);
            break;
        }
    }

    items.push_back(item);
    write_to_file();
}

void TagDb::edit_item(const Item &item) {
    for (size_t idx = 0; idx < items.size(); idx++) {
        if (items[idx].get_file_path() == item.get_file_path()) {
            items[idx].set_favorite(item.get_favorite());
            items[idx].set_tags(item.get_tags());
            items[idx].set_type(item.get_type());
            write_to_file();
            return;
        }
    }

    throw ItemNotFoundException(prefix + item.get_file_path());
}

void TagDb::delete_item(const Glib::ustring &file_path, bool delete_file) {
    // remove the prefix from the argument
    Glib::ustring rel_path = file_path.substr(prefix.size());

    bool found = false;

    for (size_t idx = 0; idx < items.size(); idx++) {
        if (items[idx].get_file_path() == rel_path) {
            items.erase(items.begin() + idx);
            found = true;
            write_to_file();
            break;
        }
    }

    if (!found) { throw ItemNotFoundException(file_path); }

    if (delete_file) {
        try {
            std::filesystem::remove(file_path.raw());
        }
        catch (...) {}
    }

}

void TagDb::set_directories(const std::set<Glib::ustring> &dirs) {
    directories = dirs;
    write_to_file();
}

void TagDb::set_default_excluded_tags(const std::set<Glib::ustring> &exclude_tags) {
    default_excluded_tags = exclude_tags;
    write_to_file();
}

std::set<Glib::ustring> TagDb::get_all_tags() const {
    std::set<Glib::ustring> result;

    for (TagDb::Item item : items) {
        for (Glib::ustring tag : item.get_tags()) {
            result.insert(tag);
        }
    }

    return result;
}

const std::set<Glib::ustring> &TagDb::get_default_excluded_tags() const {
    return default_excluded_tags;
}

const std::set<Glib::ustring> &TagDb::get_directories() const {
    return directories;
}

const std::string &TagDb::get_prefix() const {
    return prefix;
}

const std::set<Glib::ustring> &TagDb::get_tags_for_item(const Glib::ustring &file_path) const {
    // remove the prefix from the argument
    Glib::ustring rel_path = file_path.substr(prefix.size());

    for (const TagDb::Item &item : items) {
        if (item.get_file_path() == rel_path) {
            return item.tags;
        }
    }
    throw ItemNotFoundException(file_path);
}

const TagDb::Item &TagDb::get_item(const Glib::ustring &file_path) const {
    // remove the prefix from the argument
    Glib::ustring rel_path = file_path.substr(prefix.size());

    for (const TagDb::Item &item : items) {
        if (item.get_file_path() == rel_path) {
            return item;
        }
    }

    throw ItemNotFoundException(file_path);
}

std::vector<Glib::ustring> TagDb::query(const std::set<Glib::ustring> &tags_include,
                                        const std::set<Glib::ustring> &tags_exclude) const
{
    std::vector<Glib::ustring> result;
    std::vector<const TagDb::Item *> result_items;

    for (const TagDb::Item &item : items) {
        // if item is tagged with a tag that is
        // excluded from the query, then continue
        // the loop, ignoring the item
        if (item.is_tagged(tags_exclude)) {
            continue;
        }

        // if item is tagged with a tag that is
        // included in the query, add its file path
        // to the result
        if (item.is_tagged(tags_include)) {
            result_items.push_back(&item);
        }
    }

    // sort the items before extracting the file paths
    // the items sort favorites first by their overloaded operator
    std::sort(result_items.begin(), result_items.end(),
              [](const TagDb::Item *a, const TagDb::Item *b){ return (*a) < (*b); });

    for (const TagDb::Item *item : result_items) {
        result.push_back(prefix + item->get_file_path());
    }

    return result;
}

std::vector<Glib::ustring> TagDb::suggestions(const std::set<Glib::ustring> &tags_include) {
    // perform a query and collect each tag every time it occurs in an item
    std::vector<Glib::ustring> input_tags;
    for (const TagDb::Item &item : items) {
        // for exclude use the default exclude list
        if (item.is_tagged(default_excluded_tags)) {
            continue;
        }

        if (item.is_tagged(tags_include)) {
            for (const Glib::ustring &tag : item.get_tags()) {
                input_tags.push_back(tag);
            }
        }
    }

    // count the occurances of each tag
    std::map<Glib::ustring, size_t> map_tag_count;
    for (const Glib::ustring &tag : input_tags) {
        if (map_tag_count.count(tag) == 0) {
            map_tag_count[tag] = 1;
        }
        else {
            map_tag_count.find(tag)->second += 1;
        }
    }

    // copy the contents of the map into a vector for sorting
    std::vector<std::tuple<Glib::ustring, size_t>> vec_tag_count;
    for (const auto &item : map_tag_count) {
        vec_tag_count.push_back(
                std::tuple<Glib::ustring, size_t>(std::make_pair(item.first, item.second)));
    }

    // sort based on occurances so that the most frequent is at the beginning of the list
    std::sort(vec_tag_count.begin(), vec_tag_count.end(),
            [](const std::tuple<Glib::ustring, size_t> &a, const std::tuple<Glib::ustring, size_t> &b)
            { return std::get<1>(a) > std::get<1>(b); });


    // copy the sorted data into the final result
    std::vector<Glib::ustring> result;
    for (const auto &item : vec_tag_count) {
        result.push_back(std::get<0>(item));
    }

    return result;
}

std::set<Glib::ustring> TagDb::parse_tags(const std::string &line) {
    std::set<Glib::ustring> result;

    // strip whitespaces from the right
    std::string str = line.substr(0, line.find_last_not_of("\t \n") + 1);

    // strip final comma if there
    if (str[str.size() - 1] == ',') {
        str.erase(str.size() - 1);
    }

    size_t last = 0;
    size_t current = 0;

    while ((current = str.find(',', last)) != std::string::npos) {
        result.insert(str.substr(last, current - last));
        last = current + 1;
    }
    result.insert(str.substr(last, current - last));

    return result;
}

bool TagDb::str_starts_with(const std::string &str, const std::string &argument) {
    return str.rfind(argument, 0) == 0;
}
