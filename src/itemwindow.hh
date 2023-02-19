# pragma once
// standard library
#include <vector>
#include <set>

// gtkmm
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/picture.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>

// project
#include "tagutils.hh"

class ItemWindow : public Gtk::Window {
    public:
        ItemWindow();
        void set_completer_model(Glib::RefPtr<Gtk::ListStore> completer_list);
        void set_directories(const std::set<Glib::ustring> &directories);
        void set_prefix(const std::string &prefix);

        void add_items(const std::vector<std::string> &file_paths);

        // signal forwarding
        sigc::signal<void ()> signal_add_item();

    private:
        // widgets
        Gtk::Box box;
        Gtk::Label lbl_item_path;
        Gtk::Picture item_picture;
        Gtk::Label lbl_copy_to_dir;
        Gtk::ComboBoxText combo_dirs;
        Gtk::CheckButton chk_fav;
        TagPickerBase tag_editor;
        Gtk::Box buttons_skip_add;
        Gtk::Button btn_skip;
        Gtk::Button btn_add;

        // members
        std::string prefix;
        bool in_edit_mode;
        std::vector<std::string> items_to_add;
        size_t current_idx;
        bool item_starts_with_prefix;

        // functions
        void setup_for_item(size_t idx);

        // signal handlers
        bool on_close_request() override;
        void on_add();
        void on_skip();

        // signals
        sigc::signal<void ()> private_add_item;
};
