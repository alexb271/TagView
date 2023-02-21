#pragma once

// gtkmm
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>

// project
#include "previewgallery.hh"

class PreferencesWindow : public Gtk::Window {
    public:
        PreferencesWindow();
        void set_default_db_path(const Glib::ustring &default_db_path);
        void set_preview_size(PreviewGallery::PreviewSize size);

    private:
        // widgets
        Gtk::Box box;
        Gtk::Label lbl_default_db_title;
        Gtk::Label lbl_default_db_path;
        Gtk::Button btn_select_default_db;

        Gtk::Label lbl_preview_size;
        Gtk::Box checkbuttons;
        Gtk::CheckButton small;
        Gtk::CheckButton medium;
        Gtk::CheckButton large;

        // signal handlers
        void on_select_default_db();
        bool on_close_request() override;
};
