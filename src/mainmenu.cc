// project
#include "mainmenu.hh"

MainMenu::MainMenu()
:
    button_load_database("Load Database"),
    button_create_database("Create Database"),
    checkbutton_show_tag_picker(" Show Tag Picker"),
    button_add_items("Add Items"),
    button_db_settings("DB Settings"),
    button_preferences("Preferences"),
    button_about("About")
{
    // button config
    button_load_database.set_has_frame(false);
    button_create_database.set_has_frame(false);
    button_add_items.set_has_frame(false);
    button_db_settings.set_has_frame(false);
    button_preferences.set_has_frame(false);
    button_about.set_has_frame(false);
    checkbutton_show_tag_picker.set_active(true);
    checkbutton_show_tag_picker.set_margin_top(5);
    checkbutton_show_tag_picker.set_margin_bottom(5);

    // database config options such as add item
    // are hidden until a database is actually loaded
    button_add_items.set_visible(false);
    button_db_settings.set_visible(false);
    sep_2.set_visible(false);

    // box config
    box.set_orientation(Gtk::Orientation::VERTICAL);
    box.append(button_load_database);
    box.append(button_create_database);
    box.append(sep_1);
    box.append(button_add_items);
    box.append(button_db_settings);
    box.append(sep_2);
    box.append(checkbutton_show_tag_picker);
    box.append(button_preferences);
    box.append(button_about);

    set_child(box);
}

void MainMenu::set_show_database_controls(bool show_controls) {
    if (show_controls) {
        button_add_items.set_visible(true);
        button_db_settings.set_visible(true);
        sep_2.set_visible(true);
    }
    else {
        button_add_items.set_visible(false);
        button_db_settings.set_visible(false);
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

Glib::SignalProxy<void()> MainMenu::signal_add_items() {
    return button_add_items.signal_clicked();
}

Glib::SignalProxy<void()> MainMenu::signal_db_settings() {
    return button_db_settings.signal_clicked();
}

Glib::SignalProxy<void()> MainMenu::signal_show_tag_picker_toggled() {
    return checkbutton_show_tag_picker.signal_toggled();
}

Glib::SignalProxy<void()> MainMenu::signal_preferences() {
    return button_preferences.signal_clicked();
}

Glib::SignalProxy<void()> MainMenu::signal_about() {
    return button_about.signal_clicked();
}
