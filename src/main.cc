#include <gtkmm/application.h>

#include "mainwindow.hh"

int main(int argc, char **argv) {
    auto app = Gtk::Application::create("org.tagview");
    return app->make_window_and_run<MainWindow>(argc, argv);
}
