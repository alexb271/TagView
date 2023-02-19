# pragma once
// standard library
#include <memory>
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
#include <gtkmm/messagedialog.h>

// project
#include "tagutils.hh"
#include "tagdb.hh"

class ItemWindow : public Gtk::Window {
    public:
        ItemWindow();
        void set_completer_model(Glib::RefPtr<Gtk::ListStore> completer_list);
        void set_directories(const std::set<Glib::ustring> &directories);
        void set_prefix(const std::string &prefix);

        void add_items(const std::vector<std::string> &file_paths);
        void edit_item(const TagDb::Item &item);

        // signal forwarding
        sigc::signal<void (TagDb::Item)> signal_add_item();
        sigc::signal<void (TagDb::Item)> signal_edit_item();

    private:
        // widgets
        Gtk::Box box;
        Gtk::Label lbl_item_path;
        Gtk::Picture item_picture;
        Gtk::Label lbl_copy_to_dir;
        Gtk::ComboBoxText combo_dirs;
        Gtk::CheckButton chk_fav;
        TagPickerBase tag_editor;
        Gtk::Box buttons_mode_add;
        Gtk::Button btn_skip;
        Gtk::Button btn_add;
        Gtk::Box buttons_mode_edit;
        Gtk::Button btn_delete;
        Gtk::Button btn_edit;

        std::unique_ptr<Gtk::MessageDialog> message;

        // members
        std::string prefix;
        std::string default_directory;
        bool in_edit_mode;
        TagDb::Item::Type current_item_type;
        std::vector<std::string> items_to_add;
        size_t current_idx;
        bool item_starts_with_prefix;

        // functions
        void setup_for_add_item(size_t idx);
        void setup_for_edit_item(const TagDb::Item &item);
        bool copy(const std::string &file_path);
        TagDb::Item create_db_item(size_t idx);
        void show_warning(Glib::ustring primary, Glib::ustring secondary);

        // signal handlers
        void on_add();
        void on_skip();
        void on_edit();
        void on_delete();
        bool on_close_request() override;

        // signals
        sigc::signal<void (TagDb::Item)> private_add_item;
        sigc::signal<void (TagDb::Item)> private_edit_item;
};
