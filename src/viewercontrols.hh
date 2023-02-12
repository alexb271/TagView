#pragma once

// gtkmm
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <glibmm/signalproxy.h>

class ViewerControls : public Gtk::Box {
    public:
        ViewerControls();

        // signal forwarding
        Glib::SignalProxy<void()> signal_zoom_original();
        Glib::SignalProxy<void()> signal_zoom_out();
        Glib::SignalProxy<void()> signal_zoom_reset();
        Glib::SignalProxy<void()> signal_zoom_in();
        Glib::SignalProxy<void()> signal_hide_viewer();

    private:
        Gtk::Button button_zoom_original;
        Gtk::Button button_zoom_out;
        Gtk::Button button_zoom_reset;
        Gtk::Button button_zoom_in;
        Gtk::Button button_hide_viewer;
};
