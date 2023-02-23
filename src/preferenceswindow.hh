#pragma once

// standard library
#include <memory>

// gtkmm
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/filechooserdialog.h>

// project
#include "previewgallery.hh"

class PreferencesWindow : public Gtk::Window {
    public:
        PreferencesWindow(Gtk::Window &parent);
        void set_default_db_path(const Glib::ustring &default_db_path);
        void set_preview_size(PreviewGallery::PreviewSize size);

        // signal forwarding
        sigc::signal<void (const std::string &)> signal_select_defualt_db();
        sigc::signal<void (PreviewGallery::PreviewSize)> signal_set_preview_size();

    private:
        // widgets
        Gtk::Box box;
        Gtk::Label lbl_default_db_title;
        Gtk::Label lbl_default_db_path;
        Gtk::Box buttons;
        Gtk::Button btn_select_default_db;
        Gtk::Button btn_clear_default_db;

        Gtk::Label lbl_preview_size;
        Gtk::Box checkbuttons;
        Gtk::CheckButton small;
        Gtk::CheckButton medium;
        Gtk::CheckButton large;

        std::unique_ptr<Gtk::FileChooserDialog> file_chooser;

        // signal handlers
        void on_select_default_db();
        void on_clear_default_db();
        void on_toggled();
        void on_file_chooser_response(int respone_id);
        bool on_close_request() override;

        // signals
        sigc::signal<void (const std::string &)> private_select_defualt_db;
        sigc::signal<void (PreviewGallery::PreviewSize)> private_set_preview_size;
};
