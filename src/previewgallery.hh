#pragma once

// standard library
#include <memory>

// gtkmm
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/iconview.h>
#include <gtkmm/label.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treepath.h>
#include <gtkmm/gestureclick.h>
#include <gdkmm/pixbuf.h>
#include <gtkmm/popover.h>
#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>

// project
#include "tagdb.hh"

class PreviewGallery : public Gtk::ScrolledWindow {
    // the data associated with each icon
    public: class IconModel : public Gtk::TreeModelColumnRecord {
        public:
            IconModel() {
                add(id);
                add(favorite);
                add(file_path);
                add(name);
                add(pixbuf);
            }

            Gtk::TreeModelColumn<size_t> id;
            Gtk::TreeModelColumn<bool> favorite;
            Gtk::TreeModelColumn<Glib::ustring> file_path;
            Gtk::TreeModelColumn<Glib::ustring> name;
            Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> pixbuf;
    };

    public: enum class PreviewSize { Small=64, Medium=128, Large=256 };

    public:
        // ctor
        PreviewGallery(PreviewSize size = PreviewSize::Medium);

        // functions
        void set_content(const std::vector<Glib::ustring> &items);
        void set_preview_size(PreviewSize size);
        PreviewSize get_preview_size() const;
        Glib::ustring get_file_path(const Gtk::TreePath &tpath) const;

        // signal forwarding
        Glib::SignalProxy<void (const Gtk::TreeModel::Path &)> signal_item_activated();
        sigc::signal<void (size_t)> signal_item_chosen();
        sigc::signal<void (size_t)> signal_item_selected();
        sigc::signal<void (size_t)> signal_failed_to_open();
        sigc::signal<void (const Glib::ustring &)> signal_edit();

    private: class RightClickMenu : public Gtk::Popover {
                 public:
                     RightClickMenu(PreviewGallery &parent);
                     ~RightClickMenu() override;

                     const Glib::ustring &get_file_path() const;
                     void set_file_path(const Glib::ustring &file_path);

                     // signal forwarding
                     Glib::SignalProxy<void ()> signal_edit_favorite();
                     Glib::SignalProxy<void ()> signal_edit();

                 private:
                     // widgets
                     Gtk::Box box;
                     Gtk::Button btn_edit_item;

                     // members
                     Glib::ustring file_path;
             };

    private:
        // widgets
        Gtk::IconView icon_view;
        Glib::RefPtr<Gtk::ListStore> store;
        Glib::RefPtr<Gtk::GestureClick> click;
        Gtk::Label no_items_label;

        std::unique_ptr<RightClickMenu> right_click_menu;

        // members
        bool label_is_child;
        PreviewSize size;
        IconModel icon_model;

        // functions
        bool add_item(size_t id, const Glib::ustring &file_path);

        // signal handlers
        void on_item_activate(const Gtk::TreePath &tpath);
        void on_selection_changed();
        void on_right_click(int n_times, double x, double y);
        void on_fav_toggled();
        void on_edit_clicked();

        // signals
        sigc::signal<void (size_t)> private_signal_item_chosen;
        sigc::signal<void (size_t)> private_signal_item_selected;
        sigc::signal<void (size_t)> private_signal_failed_to_open;
        sigc::signal<void (const Glib::ustring &)> private_edit;
};
