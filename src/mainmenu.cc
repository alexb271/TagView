// gtkmm
#include <gtkmm/enums.h>

// project
#include "mainmenu.hh"

MainMenu::MainMenu()
:
    button_load_database("Load Database"),
    button_create_database("Create Database"),
    checkbutton_show_tag_picker(" Show Tag Picker"),
    button_add_item("Add Item"),
    button_default_exclude("Default Exclude"),
    button_preferences("Preferences"),
    button_about("About"),
    button_test("Test")
{
    // button config
    button_load_database.set_has_frame(false);
    button_create_database.set_has_frame(false);
    button_add_item.set_has_frame(false);
    button_default_exclude.set_has_frame(false);
    button_preferences.set_has_frame(false);
    button_about.set_has_frame(false);
    checkbutton_show_tag_picker.set_active(true);
    checkbutton_show_tag_picker.set_margin_top(5);
    checkbutton_show_tag_picker.set_margin_bottom(5);

    // database config options such as add item
    // are hidden until a database is actually loaded
    button_add_item.set_visible(false);
    button_default_exclude.set_visible(false);
    sep_2.set_visible(false);

    // box config
    box.set_orientation(Gtk::Orientation::VERTICAL);
    box.append(button_load_database);
    box.append(button_create_database);
    box.append(sep_1);
    box.append(button_add_item);
    box.append(button_default_exclude);
    box.append(sep_2);
    box.append(checkbutton_show_tag_picker);
    box.append(button_preferences);
    box.append(button_about);
    box.append(button_test);

    set_child(box);
}

void MainMenu::set_show_database_controls(bool show_controls) {
    if (show_controls) {
        button_add_item.set_visible(true);
        button_default_exclude.set_visible(true);
        sep_2.set_visible(true);
    }
    else {
        button_add_item.set_visible(false);
        button_default_exclude.set_visible(false);
        sep_2.set_visible(false);
    }
}

bool MainMenu::get_tag_picker_active() {
    return checkbutton_show_tag_picker.get_active();
}

Glib::SignalProxy<void()> MainMenu::signal_load_database() {
    return button_load_database.signal_clicked();
}

Glib::SignalProxy<void()> MainMenu::signal_create_database() {
    return button_create_database.signal_clicked();
}

Glib::SignalProxy<void()> MainMenu::signal_add_item() {
    return button_add_item.signal_clicked();
}

Glib::SignalProxy<void()> MainMenu::signal_default_exclude() {
    return button_default_exclude.signal_clicked();
}

Glib::SignalProxy<void()> MainMenu::signal_show_tag_picker_toggled() {
    return checkbutton_show_tag_picker.signal_toggled();
}

Glib::SignalProxy<void()> MainMenu::signal_signal_preferences() {
    return button_preferences.signal_clicked();
}

Glib::SignalProxy<void()> MainMenu::signal_signal_about() {
    return button_about.signal_clicked();
}

Glib::SignalProxy<void()> MainMenu::signal_test() {
    return button_test.signal_clicked();
}
