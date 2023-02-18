#pragma once

// gtkmm
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>

// project
#include "tagutils.hh"

class DbSettingsWindow : public Gtk::Window {
    public:
        DbSettingsWindow();

    private:
        // widgets
        Gtk::Box box;

        // signal handlers
        bool on_close_request() override;
};
