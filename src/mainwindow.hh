#pragma once

// standard library
#include <memory>

// gtkmm
#include <gtkmm/applicationwindow.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/button.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/popover.h>
#include <gtkmm/box.h>
#include <gtkmm/separator.h>
#include <gtkmm/liststore.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/aboutdialog.h>

// project
#include "imageviewer.hh"
#include "viewercontrols.hh"
#include "previewgallery.hh"
#include "tagpicker.hh"
#include "mainmenu.hh"
#include "tagdb.hh"
#include "config.hh"
#include "itemwindow.hh"
#include "dbsettingswindow.hh"
#include "preferenceswindow.hh"

class MainWindow : public Gtk::ApplicationWindow {
    public:
        // ctor
        MainWindow();

    private:
        // file chooser action
        enum class Action { LOAD, CREATE, ADD };

        // members for providing entry completion
        Glib::RefPtr<Gtk::ListStore> list_store;
        TagPickerBase::ListModel list_model;

        // custom widgets
        ImageViewer viewer;
        PreviewGallery gallery;
        TagPicker tag_picker;
        MainMenu main_menu;

        // other custom classes
        TagDb db;
        Config config;

        // header widgets
        Gtk::HeaderBar header;
        Gtk::MenuButton button_main_menu;
        ViewerControls viewer_controls;

        // other windows
        ItemWindow item_window;
        DbSettingsWindow db_settings_window;
        PreferencesWindow preferences_window;

        // dialogs
        std::unique_ptr<Gtk::MessageDialog> message;
        std::unique_ptr<Gtk::FileChooserDialog> file_chooser;
        std::unique_ptr<Gtk::AboutDialog> about_dialog;

        // main regular widgets
        Gtk::Box box;
        Gtk::Separator sep;

        // controllers
        Glib::RefPtr<Gtk::EventControllerKey> key_controller;

        // members for switching images in image viewer
        std::vector<Glib::ustring> files;
        size_t files_idx;
        bool switching_allowed;

        // fucntions
        void load_database(const std::string &db_file_path);
        void add_items(const std::vector<std::string> &file_paths);
        void set_completer_data(const std::set<Glib::ustring> &completer_tags);
        void show_warning(Glib::ustring primary, Glib::ustring secondary);
        void refresh_gallery();

        // signal handlers
        bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);

        // tag picker
        void on_filter_toggled(TagDb::QueryType query_type);
        void on_tag_query_changed(TagQuery tag_selection);
        void on_reload_default_exclude_required();

        // gallery
        void on_gallery_item_chosen(size_t id);
        void on_gallery_item_selected(size_t id);
        void on_gallery_failed_to_open(size_t id);
        void on_gallery_edit(const Glib::ustring &file_path);
        void on_gallery_generation_status_changed(bool generation_in_progress);

        // image viewer
        void on_hide_viewer();

        // main menu
        void on_load_database();
        void on_create_database();
        void on_add_items();
        void on_db_settings();
        void on_tag_picker_toggled();
        void on_preferences();
        void on_about();

        // db settings window
        void on_exclude_tags_changed(const std::set<Glib::ustring> &exclude_tags);
        void on_directories_changed(const std::set<Glib::ustring> &directories);

        // item window
        void on_add_item(TagDb::Item item);
        void on_edit_item(TagDb::Item item);
        void on_delete_item(const Glib::ustring &file_path, bool delete_file);
        void on_request_suggestions(const std::set<Glib::ustring> &tags);

        // preferences window
        void on_select_default_db(const std::string &default_db_path);
        void on_set_preview_size(PreviewGallery::PreviewSize size);

        // dialog responses
        void on_file_chooser_response(int respone_id, Action action);
};
