#pragma once

// gtkmm
#include <gtkmm/applicationwindow.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/button.h>
#include <gtkmm/menubutton.h>
#include <gtkmm/popover.h>
#include <gtkmm/box.h>
#include <gtkmm/separator.h>
#include <gtkmm/eventcontrollerkey.h>
#include <glibmm/ustring.h>

// project
#include "imageviewer.hh"
#include "viewercontrols.hh"
#include "previewgallery.hh"
#include "tagpicker.hh"
#include "mainmenu.hh"
#include "tagdb.hh"
#include "additemwindow.hh"

class MainWindow : public Gtk::ApplicationWindow {
    public:
        // ctor
        MainWindow();
        ~MainWindow() override;

    private:
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
        AddItemWindow add_item_window;

        // main regular widgets
        Gtk::Box box;
        Gtk::Separator sep;

        // controllers
        Glib::RefPtr<Gtk::EventControllerKey> key_controller;

        // members for switching images in image viewer
        std::vector<Glib::ustring> files;
        size_t files_idx;

        // fucntions
        void load_database();

        // signal handlers
        void on_tag_picker_toggled();
        void on_tag_query_changed(TagQuery tag_selection);
        void on_item_chosen(size_t id);
        void on_item_selected(size_t id);
        bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
        void on_hide_viewer();
        void on_test();
};
