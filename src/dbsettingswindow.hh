#pragma once

// gtkmm
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>

// project
#include "tagutils.hh"

class DbSettingsWindow : public Gtk::Window {
    public:
        DbSettingsWindow();

        void set_completer_model(Glib::RefPtr<Gtk::ListStore> completer_list);
        void reset(const std::set<Glib::ustring> &directories,
                   const std::set<Glib::ustring> &default_exclude_tags);

    private:
        // widgets
        Gtk::Box box;
        Gtk::Button btn_add_dir;
        Gtk::Label lbl_dirs;
        ItemList dirs;
        Gtk::Label lbl_default_exclude;
        TagPickerBase tp_exclude;

        // signal handlers
        bool on_close_request() override;
        void on_dirs_changed(const std::set<Glib::ustring> &dirs);
        void on_tags_changed(const std::set<Glib::ustring> &dirs);
};
