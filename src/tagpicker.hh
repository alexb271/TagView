#pragma once

// standard library
#include <vector>
#include <set>

// gtkmm
#include <glibmm/ustring.h>
#include <glibmm/refptr.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/entrycompletion.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/separator.h>
#include <sigc++/signal.h>

// project
#include "tagutil.hh"

class TagPicker : public Gtk::Box {
    public:
        TagPicker();

        void set_completer_data(const std::set<Glib::ustring> &tags);
        void set_current_item_tags(const std::set<Glib::ustring> &tags);
        void clear_current_item_tags();
        TagQuery get_current_query() const;

        // signal forwarding
        sigc::signal<void (TagQuery)> signal_query_changed();

        // C signal handler friend function
        friend void on_entry_activate(GtkEntry *c_entry, gpointer data);

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

        // included tag display
        Gtk::ScrolledWindow include_scw;
        Gtk::Box include_box;
        std::vector<ItemInQuery> vec_include_widgets;

        // excluded tag display
        Gtk::ScrolledWindow exclude_scw;
        Gtk::Box exclude_box;
        std::vector<ItemInQuery> vec_exclude_widgets;

        // current image tags display
        Gtk::ScrolledWindow outside_scw;
        Gtk::Box outside_box;
        std::vector<ItemOutsideQuery> vec_outside_widgets;

        // other widgets
        Gtk::Label include_label;
        Gtk::Label exclude_label;
        Gtk::Label outside_label;
        Gtk::Separator separator_1;
        Gtk::Separator separator_2;

        // actual tag data
        std::vector<Glib::ustring> tags_all;
        std::vector<Glib::ustring> tags_include;
        std::vector<Glib::ustring> tags_exclude;

        // functions
        void remove(Gtk::Box &box,
                std::vector<ItemInQuery> &widget_vec,
                std::vector<Glib::ustring> &tag_vec,
                const Glib::ustring &tag);


        // signal handlers
        void on_signal_add(const Glib::ustring &tag);
        void on_signal_exclude(const Glib::ustring &tag);
        void on_signal_remove(const Glib::ustring &tag, bool exclude);
        bool on_match_selected(const Gtk::TreeModel::iterator &iter);
        bool on_completion_match(const Glib::ustring &key,
                                 const Gtk::TreeModel::const_iterator &iter);

        // own signals
        sigc::signal<void (TagQuery)> private_signal_query_changed;
};
