// gtkmm
#include <gtkmm/window.h>
#include <gtkmm/label.h>

class AddItemWindow : public Gtk::Window {
    public:
        AddItemWindow();

    private:
        Gtk::Label label;

        // signal handlers
        bool on_close_request() override;
};
