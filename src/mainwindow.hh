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

// project
#include "imageviewer.hh"
#include "viewercontrols.hh"
#include "previewgallery.hh"
#include "tagpicker.hh"
#include "mainmenu.hh"
#include "tagdb.hh"
#include "itemwindow.hh"
#include "dbsettingswindow.hh"

class MainWindow : public Gtk::ApplicationWindow {
    public:
        // ctor
        MainWindow();
        ~MainWindow() override;

    private:
        // members for providing entry completion
        Glib::RefPtr<Gtk::ListStore> list_store;
        TagPickerBase::ListModel list_model;

        // custom widgets
        ImageViewer viewer;
        PreviewGallery gallery;
        TagPicker tag_picker;
        MainMenu main_menu;

        // Tag Database
        TagDb db;

        // header widgets
        Gtk::HeaderBar header;
        Gtk::MenuButton button_main_menu;
        ViewerControls viewer_controls;

        // other windows
        ItemWindow item_window;
        DbSettingsWindow db_settings_window;

        // dialogs
        std::unique_ptr<Gtk::MessageDialog> message;
        std::unique_ptr<Gtk::FileChooserDialog> file_chooser;

        // main regular widgets
        Gtk::Box box;
        Gtk::Separator sep;

        // controllers
        Glib::RefPtr<Gtk::EventControllerKey> key_controller;

        // members for switching images in image viewer
        std::vector<Glib::ustring> files;
        size_t files_idx;

        // fucntions
        void load_database(std::string db_file_path);
        void set_completer_data(const std::set<Glib::ustring> &completer_tags);
        void show_warning(Glib::ustring primary, Glib::ustring secondary);

        // signal handlers
        bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);

        // tag picker
        void on_tag_query_changed(TagQuery tag_selection);
        void on_reload_default_exclude_required();

        // gallery
        void on_gallery_item_chosen(size_t id);
        void on_gallery_item_selected(size_t id);
        void on_gallery_failed_to_open(size_t id);
        void on_gallery_edit(const Glib::ustring &file_path);

        // image viewer
        void on_hide_viewer();

        // main menu
        void on_load_database();
        void on_add_items();
        void on_db_settings();
        void on_tag_picker_toggled();

        // db settings window
        void on_exclude_tags_changed(const std::set<Glib::ustring> &exclude_tags);
        void on_directories_changed(const std::set<Glib::ustring> &directories);

        // item window
        void on_add_item(TagDb::Item item);
        void on_edit_item(TagDb::Item item);
        void on_delete_item(const Glib::ustring &file_path, bool delete_file);
        void on_request_suggestions(const std::set<Glib::ustring> &tags);

        // dialog responses
        void on_file_chooser_response(int respone_id);

        // other
        void on_test();
};
