#pragma once
// standard library
#include <vector>
#include <set>
#include <memory>

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
        TagQuery(std::set<Glib::ustring> tags_to_include,
                 std::set<Glib::ustring> tags_to_exclude);
        std::set<Glib::ustring> tags_include;
        std::set<Glib::ustring> tags_exclude;
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
        ItemOutsideQuery(const Glib::ustring &tag_name, bool exclude_button = false);
        void set_tag(const Glib::ustring &tag);
        Glib::ustring get_text() const;
        Glib::SignalProxy<void()> signal_add();
        Glib::SignalProxy<void()> signal_exclude();

    private:
        Gtk::Label name;
        Gtk::Button add;
        Gtk::Button exclude;
};

// class to hold a dynamic list of ItemInQuery or ItemOutsideQuery widgets
class ItemList : public Gtk::ScrolledWindow {
    public: enum class Type { INSIDE, OUTSIDE, OUTSIDE_WITH_EXCLUDE };
    public:
        ItemList(Type type);

        void append(const Glib::ustring &text);
        void append_and_notify(const Glib::ustring &text);
        void clear();
        const std::set<Glib::ustring> &get_content() const;

        // signal forwarding
        sigc::signal<void (const std::set<Glib::ustring> &)> signal_contents_changed();
        sigc::signal<void (const Glib::ustring &)> signal_add();
        sigc::signal<void (const Glib::ustring &)> signal_exclude();


    private:
        Type type;
        Gtk::Box box;
        std::vector<std::unique_ptr<Gtk::Widget>> widgets;
        std::set<Glib::ustring> items;

        sigc::signal<void (const std::set<Glib::ustring> &)> private_contents_changed;
        sigc::signal<void (const Glib::ustring &)> private_add;
        sigc::signal<void (const Glib::ustring &)> private_exclude;

        // signal handlers
        void on_signal_remove(const Glib::ustring &text);
        void on_signal_add(const Glib::ustring &text);
        void on_signal_exclude(const Glib::ustring &text);
        void on_test();
};

void tag_editor_on_entry_activate(GtkEntry *c_entry, gpointer data);

class TagPickerBase : public Gtk::Box {
    public:
        TagPickerBase(Glib::ustring title);

        void set_completer_data(const std::set<Glib::ustring> &completer_tags);
        void set_allow_create_new_tag(bool allow_create_new_tag);
        bool get_allow_create_new_tag() const;

        // C signal handler friend function
        friend void tag_editor_on_entry_activate(GtkEntry *c_entry, gpointer data);

    protected:
        // tags
        std::set<Glib::ustring> tags_all;
        ItemList tags;
        Gtk::Label lbl_tags;

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
