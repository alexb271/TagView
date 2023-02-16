#pragma once
// standard library
#include <vector>
#include <set>
#include <type_traits>

// gtkmm
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/entrycompletion.h>
#include <gtkmm/liststore.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>
#include <sigc++/signal.h>

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
class ItemInQuery : public Gtk::Box {
    public:
        ItemInQuery(const Glib::ustring &tag_name);
        void set_tag(const Glib::ustring &tag);
        Glib::ustring get_text() const;
        Glib::SignalProxy<void()> signal_remove();

    private:
        Gtk::Label name;
        Gtk::Button remove;
};

// class to be placed in a vertical Gtk::Box showing tags
// of the currently viewed image which are not part of the query.
// These tags can either be added to or excluded from the query.
class ItemOutsideQuery : public Gtk::Box {
    public:
        ItemOutsideQuery(bool exclude_button = false);
        ItemOutsideQuery(const Glib::ustring &tag_name, bool exclude_button = true);
        void set_tag(const Glib::ustring &tag);
        Glib::ustring get_text() const;
        Glib::SignalProxy<void()> signal_add();
        Glib::SignalProxy<void()> signal_exclude();

    private:
        Gtk::Label name;
        Gtk::Button add;
        Gtk::Button exclude;
};

// class to hold a dynamic list of ItemInQuery widgets
class ItemInQueryList : public Gtk::ScrolledWindow {
    public:
        ItemInQueryList();

        void append(const Glib::ustring &text);
        void clear();
        const std::set<Glib::ustring> &get_content() const;
        sigc::signal<void (const std::set<Glib::ustring> &)> signal_contents_changed();

    private:
        Gtk::Box widgets;
        std::set<Glib::ustring> items;
        sigc::signal<void (const std::set<Glib::ustring> &)> private_contents_changed;

        // signal handlers
        void on_signal_remove(ItemInQuery *item);
};

void tag_editor_on_entry_activate(GtkEntry *c_entry, gpointer data);

class TagPickerBase : public Gtk::Box {
    public:
        TagPickerBase(Glib::ustring title);

        const std::set<Glib::ustring> &get_content() const;
        void set_completer_data(const std::set<Glib::ustring> &completer_tags);
        void set_allow_create_new_tag(bool allow_create_new_tag);
        bool get_allow_create_new_tag() const;

        // C signal handler friend function
        friend void tag_editor_on_entry_activate(GtkEntry *c_entry, gpointer data);

    protected:
        // tags
        std::vector<Glib::ustring> tags_all;
        ItemInQueryList tags;
        Gtk::Label lbl_title;

        // functions
        void add_tag(const Glib::ustring &tag);

    private:
        // liststore model
        class ListModel : public Gtk::TreeModelColumnRecord {
            public:
                ListModel() {
                    add(tag);
                }
                Gtk::TreeModelColumn<Glib::ustring> tag;
        };

        // entry with completion
        Gtk::Entry entry;
        Glib::RefPtr<Gtk::EntryCompletion> completer;
        Glib::RefPtr<Gtk::ListStore> completer_list;
        ListModel list_model;

        // bool whether creating new tags is allowed with this widget
        bool allow_create_new_tag;

        // signal handlers
        bool on_match_selected(const Gtk::TreeModel::iterator &iter);
        bool on_completion_match(const Glib::ustring &key,
                                 const Gtk::TreeModel::const_iterator &iter);
};
