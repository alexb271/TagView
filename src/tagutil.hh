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
class TagInQuery : public Gtk::Box {
    public:
        TagInQuery();
        TagInQuery(const Glib::ustring &tag_name);
        void set_tag(const Glib::ustring &tag);
        Glib::ustring get_tag();
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
        TagOutsideQuery(bool exclude_button = false);
        TagOutsideQuery(const Glib::ustring &tag_name, bool exclude_button = true);
        void set_tag(const Glib::ustring &tag);
        Glib::ustring get_tag();
        Glib::SignalProxy<void()> signal_add();
        Glib::SignalProxy<void()> signal_exclude();

    private:
        Gtk::Label name;
        Gtk::Button add;
        Gtk::Button exclude;
};

// class to hold a dynamic list of Gtk::Widgets in a vertical
// Gtk::Box. This is primarily meant for TagInQuery and
// TagOutsideQuery.

template<typename T>
class ItemList : public Gtk::ScrolledWindow {
    // only accept subclasses of Gtk::Widget, since in all cases
    // type T will be attempted to be inserted into a Gtk::Box
    static_assert(std::is_base_of<Gtk::Widget, T>::value, "T must inherit from Gtk::Widget");

    public:
        ItemList() {
            // box setup
            box.set_orientation(Gtk::Orientation::VERTICAL);

            // scrolled window setup
            set_propagate_natural_height(true);
            set_propagate_natural_width(true);
            set_child(box);
        }

        T& create() {
            vec_widgets.push_back(T());
            box.append(vec_widgets[vec_widgets.size() - 1]);
            return vec_widgets[vec_widgets.size() - 1];
        }

        void remove_at(size_t idx) {
            box.remove(vec_widgets.at(idx));
            vec_widgets.erase(vec_widgets.begin() + idx);
        }

        void clear() {
            for (T &item : vec_widgets) {
                box.remove(item);
            }
            vec_widgets.clear();
        }

        size_t size() const {
            return vec_widgets.size();
        }

        T& operator[] (size_t idx) {
            return vec_widgets.at(idx);
        }

    private:
        Gtk::Box box;
        std::vector<T> vec_widgets;
};

void tag_editor_on_entry_activate(GtkEntry *c_entry, gpointer data);

class TagEditor : public Gtk::Box {
    public:
        TagEditor(Glib::ustring title);

        void set_completer_data(const std::set<Glib::ustring> &completer_tags);
        void set_allow_create_new_tag(bool allow_create_new_tag);
        bool get_allow_create_new_tag();

        // C signal handler friend function
        friend void tag_editor_on_entry_activate(GtkEntry *c_entry, gpointer data);

        // signal forwarding
        sigc::signal<void (const std::vector<Glib::ustring> &)> signal_contents_changed();

    protected:
        // tags
        std::vector<Glib::ustring> tags_all;
        std::vector<Glib::ustring> tags_picked;
        ItemList<TagInQuery> tag_widgets;

        // display widgets
        Gtk::Label lbl_title;

        // signals
        sigc::signal<void (const std::vector<Glib::ustring> &)> private_contents_changed;

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
        void on_signal_remove(const Glib::ustring &tag);
        bool on_match_selected(const Gtk::TreeModel::iterator &iter);
        bool on_completion_match(const Glib::ustring &key,
                                 const Gtk::TreeModel::const_iterator &iter);
};
