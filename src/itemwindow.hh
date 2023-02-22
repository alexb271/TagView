# pragma once

// standard library
#include <memory>
#include <vector>
#include <set>

// gtkmm
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/image.h>
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
        void set_suggestions(const std::vector<Glib::ustring> &tags);

        void add_items(const std::vector<std::string> &file_paths);
        void edit_item(const TagDb::Item &item);

        // signal forwarding
        sigc::signal<void (TagDb::Item)> signal_add_item();
        sigc::signal<void (TagDb::Item)> signal_edit_item();
        sigc::signal<void (const Glib::ustring &, bool)> signal_delete_item();
        sigc::signal<void (const std::set<Glib::ustring> &)> signal_request_suggestions();

    private: class DeleteDialog : public Gtk::MessageDialog {
                 public:
                     DeleteDialog(ItemWindow &parent);

                     bool get_delete_file();

                 private:
                     Gtk::CheckButton chk_delete_file;
             };
    private:
        // widgets
        Gtk::Box box;
        Gtk::Label lbl_item_path;

        // preview
        Gtk::Label item_preview_error;
        Gtk::Image item_preview;

        Gtk::Label lbl_copy_to_dir;
        Gtk::ComboBoxText combo_dirs;
        Gtk::CheckButton chk_fav;

        // tag editor with suggestions
        Gtk::Grid editor_grid;
        TagPickerBase tag_editor;
        Gtk::Label lbl_suggestions;
        ItemList tag_suggestions;

        Gtk::Box buttons_mode_add;
        Gtk::Button btn_skip;
        Gtk::Button btn_add;

        Gtk::Box buttons_mode_edit;
        Gtk::Button btn_delete;
        Gtk::Button btn_edit;

        std::unique_ptr<Gtk::MessageDialog> message;
        std::unique_ptr<DeleteDialog> delete_dialog;

        // members
        std::string prefix;
        bool item_starts_with_prefix;
        std::string default_directory;

        bool in_edit_mode;
        TagDb::Item::Type current_edited_item_type;
        Glib::ustring current_edited_item_file_path;

        std::vector<std::string> items_to_add;
        size_t current_idx;

        // max number of suggestions to show
        size_t suggestion_count;
        int preview_size;

        // functions
        void setup_for_add_item(size_t idx);
        void setup_for_edit_item(const TagDb::Item &item);
        bool set_preview(const Glib::ustring &file_path);
        bool copy(const std::string &file_path);
        TagDb::Item create_db_item(size_t idx);
        void show_warning(Glib::ustring primary, Glib::ustring secondary);

        // signal handlers
        void on_tag_editor_contents_changed(const std::set<Glib::ustring> &tags);
        void on_add_suggestion(const Glib::ustring &tag);
        void on_add();
        void on_skip();
        void on_edit();
        void on_delete();
        void on_delete_respone(int respone_id);
        bool on_close_request() override;

        // signals
        sigc::signal<void (TagDb::Item)> private_add_item;
        sigc::signal<void (TagDb::Item)> private_edit_item;
        sigc::signal<void (const Glib::ustring &, bool)> private_delete_item;
        sigc::signal<void (const std::set<Glib::ustring> &)> private_request_suggestions;
};
