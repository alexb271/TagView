#pragma once

// standard library
#include <string>
#include <memory>

// gtkmm
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/filechooserdialog.h>

// project
#include "tagutils.hh"

class DbSettingsWindow : public Gtk::Window {
    public:
        DbSettingsWindow();

        void set_completer_model(Glib::RefPtr<Gtk::ListStore> completer_list);
        void setup(const Glib::ustring &db_path,
                   const std::set<Glib::ustring> &directories,
                   const std::set<Glib::ustring> &default_exclude_tags,
                   const std::string &prefix);
        sigc::signal<void (const std::set<Glib::ustring> &)> signal_exclude_tags_changed();
        sigc::signal<void (const std::set<Glib::ustring> &)> signal_directoires_changed();

    private:
        // widgets
        Gtk::Box box;
        Gtk::Label lbl_db_path;
        Gtk::Button btn_add_dir;
        Gtk::Label lbl_dirs;
        ItemList dirs;
        Gtk::Label lbl_default_exclude;
        TagPickerBase tp_exclude;

        // members for adding directories
        std::unique_ptr<Gtk::MessageDialog> subdir_warning;
        std::unique_ptr<Gtk::FileChooserDialog> file_chooser;
        std::string prefix;

        // signal handlers
        bool on_close_request() override;
        void on_add_directory();
        void on_file_chooser_response(int respone_id);

        // signals
        sigc::signal<void (const std::set<Glib::ustring> &)> private_exclude_tags_changed;
        sigc::signal<void (const std::set<Glib::ustring> &)> private_directoires_changed;
};
