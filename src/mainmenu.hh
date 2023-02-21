#pragma once

// gtkmm
#include <gtkmm/popover.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/separator.h>
#include <glibmm/signalproxy.h>

// A Popover menu for the main menu button
class MainMenu : public Gtk::Popover {
    public:
        MainMenu();

        // database controls should only be shown when a database is loaded
        void set_show_database_controls(bool show_controls);

        // returns whether the tag picker is currently visible
        bool get_tag_picker_active();

        // signal socket forwaring
        Glib::SignalProxy<void()> signal_load_database();
        Glib::SignalProxy<void()> signal_create_database();
        Glib::SignalProxy<void()> signal_add_items();
        Glib::SignalProxy<void()> signal_db_settings();
        Glib::SignalProxy<void()> signal_show_tag_picker_toggled();
        Glib::SignalProxy<void()> signal_preferences();
        Glib::SignalProxy<void()> signal_about();
        Glib::SignalProxy<void()> signal_test();

    private:
        // main box to pack things into
        Gtk::Box box;

        // menu widgets
        Gtk::Button button_load_database;
        Gtk::Button button_create_database;
        Gtk::CheckButton checkbutton_show_tag_picker;
        Gtk::Separator sep_1;
        Gtk::Button button_add_items;
        Gtk::Button button_db_settings;
        Gtk::Separator sep_2;
        Gtk::Button button_preferences;
        Gtk::Button button_about;
        Gtk::Button button_test;
};
