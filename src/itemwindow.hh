// standard library
#include <vector>

// gtkmm
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>

// project
#include "tagutil.hh"

class ItemWindow : public Gtk::Window {
    public:
        ItemWindow();
        void set_completer_data(const std::set<Glib::ustring> &completer_tags);

    private:
        Gtk::Label label;
        Gtk::ScrolledWindow scw;
        Gtk::Box box;

        TagPickerBase tag_editor;

        // signal handlers
        bool on_close_request() override;
        void on_tags_changed(const std::vector<Glib::ustring> &tags);
};
