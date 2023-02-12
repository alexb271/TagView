#pragma once

// standard library
#include <vector>

// gtkmm
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>

// the data structure provided by this widget
// contains a dynamic list for both tags to include
// and exclude from a search in the database
class TagQuery {
    public:
        TagQuery(std::vector<Glib::ustring> tags_to_include,
                 std::vector<Glib::ustring> tags_to_exclude);
        std::vector<Glib::ustring> tags_include;
        std::vector<Glib::ustring> tags_exclude;
};

// class to be placed in a vertical Gtk::Box showing tags
// that are either currently in the query or are currently
// being excluded from the query.
// These tags can be removed from the query.
class TagInQuery : public Gtk::Box {
    public:
        TagInQuery(const Glib::ustring &tag_name);
        Glib::ustring tag();
        Glib::SignalProxy<void()> signal_remove();

    private:
        Gtk::Label name;
        Gtk::Button remove;
};

// class to be placed in a vertical Gtk::Box showing tags
// of the currently viewed image which are not part of the query.
// These tags can either be added to or excluded from the query.
class TagOutsideQuery : public Gtk::Box {
    public:
        TagOutsideQuery(const Glib::ustring &tag_name);
        Glib::ustring tag();
        Glib::SignalProxy<void()> signal_add();
        Glib::SignalProxy<void()> signal_exclude();

    private:
        Gtk::Label name;
        Gtk::Button add;
        Gtk::Button exclude;
};

