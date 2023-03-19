#pragma once

// standard library
#include <set>

// gtkmm
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/separator.h>

// project
#include "tagutils.hh"
#include "tagdb.hh"

class TagPicker : public TagPickerBase {
    public:
        TagPicker();

        TagQuery get_current_query() const;
        void add_excluded_tag(const Glib::ustring &tag);
        void set_current_item_tags(const std::set<Glib::ustring> &tags);
        void clear_excluded_tags();
        void clear_current_item_tags();

        // signal forwarding
        sigc::signal<void (TagDb::QueryType)> signal_filter_toggled();
        sigc::signal<void (TagQuery)> signal_query_changed();
        Glib::SignalProxy<void ()> signal_reload_default_exclude_required();

    private:
        // widgets
        Gtk::Box filter_box;
        Gtk::Label lbl_filter;
        Gtk::CheckButton chk_filter_or;
        Gtk::CheckButton chk_filter_and;
        Gtk::Label lbl_tags_exclude;
        Gtk::Button btn_reload_default_exclude;
        ItemList tags_exclude;
        Gtk::Label lbl_tags_current_item;
        ItemList tags_current_item;

        Gtk::Separator sep1;
        Gtk::Separator sep2;

        // signal handlers
        void on_filter_toggled();
        void on_signal_add(const Glib::ustring &tag);
        void on_signal_exclude(const Glib::ustring &tag);
        void on_content_changed(const std::set<Glib::ustring> &tags);

        // signals
        sigc::signal<void (TagDb::QueryType)> private_filter_toggled;
        sigc::signal<void (TagQuery)> private_query_changed;
};
