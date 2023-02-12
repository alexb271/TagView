#pragma once

// standard library
#include <string>

// gtkmm
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/picture.h>
#include <gtkmm/eventcontrollermotion.h>
#include <gtkmm/eventcontrollerscroll.h>
#include <gtkmm/gestureclick.h>
#include <gtkmm/adjustment.h>
#include <gdk/gdkpixbuf.h>
#include <glibmm/value.h>

class ImageViewer : public Gtk::ScrolledWindow {

    public:
        // constructor
        ImageViewer();

        // image loading functions
        bool set_image(const std::string &file_path);

        // zoom functions
        void zoom_in();
        void zoom_out();
        void zoom_reset();
        void zoom_original();

    private:
        // image components
        Gtk::Picture pic;
        Glib::RefPtr<Gdk::Pixbuf> buf;
        Glib::RefPtr<Gdk::Pixbuf> buf_zoom;

        // event controllers
        Glib::RefPtr<Gtk::EventControllerMotion> motion_controller;
        Glib::RefPtr<Gtk::EventControllerScroll> scroll_controller;
        Glib::RefPtr<Gtk::GestureClick> click_gesture;

        // private member variables
        Glib::RefPtr<Gtk::Adjustment> zoom_adj;
        Glib::RefPtr<Gtk::Adjustment> hadj;
        Glib::RefPtr<Gtk::Adjustment> vadj;
        double mouse_x;
        double mouse_y;
        bool mouse_down;

        // private functions
        void zoom_apply();
        void zoom_adjust_scrollbars();
        double calculate_shrunken_zoom();

        // signal handlers
        void on_motion(double x, double y);
        void on_pressed(int n_times, double x, double y);
        void on_released(int n_times, double x, double y);
        bool on_scroll(double dx, double dy);
};
