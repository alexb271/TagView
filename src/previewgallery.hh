#pragma once

// gtkmm
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/iconview.h>
#include <gtkmm/label.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treepath.h>
#include <gtkmm/gestureclick.h>
#include <gdkmm/pixbuf.h>
#include <sigc++/signal.h>
#include <glibmm/ustring.h>


class PreviewGallery : public Gtk::ScrolledWindow {
    // the data associated with each icon
    public: class IconModel : public Gtk::TreeModelColumnRecord {
        public:
            IconModel() {
                add(id);
                add(file_path);
                add(name);
                add(pixbuf);
            }

            Gtk::TreeModelColumn<size_t> id;
            Gtk::TreeModelColumn<Glib::ustring> file_path;
            Gtk::TreeModelColumn<Glib::ustring> name;
            Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> pixbuf;
    };

    public: enum class PreviewSize { Small=64, Medium=128, Large=256 };

    public:
        // ctor
        PreviewGallery(PreviewSize size = PreviewSize::Medium);

        // functions
        void set_content(const std::vector<Glib::ustring> &file_paths);
        void set_size(PreviewSize size);
        PreviewSize get_size() const;
        Glib::ustring get_file_path(const Gtk::TreePath &tpath) const;

        // signal forwarding
        Glib::SignalProxy<void (const Gtk::TreeModel::Path &)> signal_item_activated();
        sigc::signal<void (size_t)> signal_item_chosen();
        sigc::signal<void (size_t)> signal_item_selected();

    private:
        // members
        Gtk::IconView icon_view;
        IconModel icon_model;
        Glib::RefPtr<Gtk::ListStore> store;
        PreviewSize size;
        Glib::RefPtr<Gtk::GestureClick> click;
        Gtk::Label no_items_label;
        bool label_is_child;

        // functions
        bool add_item(size_t id, const Glib::ustring &file_path);

        // signal handlers
        void on_item_activate(const Gtk::TreePath &tpath);
        void on_selection_changed();
        void on_right_click(int n_times, double x, double y);

        // signals
        sigc::signal<void (size_t)> private_signal_item_chosen;
        sigc::signal<void (size_t)> private_signal_item_selected;
};
