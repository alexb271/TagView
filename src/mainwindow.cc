// gtkmm
#include <gtkmm/messagedialog.h>

// project
#include "gtkmm/dialog.h"
#include "gtkmm/enums.h"
#include "mainwindow.hh"

MainWindow::MainWindow()
:
    key_controller(Gtk::EventControllerKey::create())
{
    // configure image viewer controls
    viewer_controls.signal_zoom_out().connect(sigc::mem_fun(viewer, &ImageViewer::zoom_out));
    viewer_controls.signal_zoom_reset().connect(sigc::mem_fun(viewer, &ImageViewer::zoom_reset));
    viewer_controls.signal_zoom_in().connect(sigc::mem_fun(viewer, &ImageViewer::zoom_in));
    viewer_controls.signal_zoom_original().connect(sigc::mem_fun(viewer, &ImageViewer::zoom_original));
    viewer_controls.signal_hide_viewer().connect(sigc::mem_fun(*this, &MainWindow::on_hide_viewer));

    // image viewer and its controls are initally hidden because the gallery is shown first
    viewer_controls.set_visible(false);
    viewer.set_visible(false);

    // configure main menu
    button_main_menu.set_direction(Gtk::ArrowType::NONE);
    button_main_menu.set_popover(main_menu);
    main_menu.signal_load_database().connect(sigc::mem_fun(*this, &MainWindow::on_load_database));
    main_menu.signal_add_items().connect(sigc::mem_fun(*this, &MainWindow::on_add_items));
    main_menu.signal_db_settings().connect(sigc::mem_fun(*this, &MainWindow::on_db_settings));
    main_menu.signal_show_tag_picker_toggled().connect(
            sigc::mem_fun(*this, &MainWindow::on_tag_picker_toggled));
    main_menu.signal_test().connect(sigc::mem_fun(*this, &MainWindow::on_test));

    // configure header
    header.set_show_title_buttons(true);
    header.pack_start(viewer_controls);
    header.pack_end(button_main_menu);
    set_titlebar(header);

    // configure completion
    list_store = Gtk::ListStore::create(list_model);

    // configure tag picker
    tag_picker.set_completer_model(list_store);
    tag_picker.set_halign(Gtk::Align::START);
    tag_picker.set_valign(Gtk::Align::START);
    tag_picker.set_margin(15);
    tag_picker.signal_query_changed().connect(
            sigc::mem_fun(*this, &MainWindow::on_tag_query_changed));
    tag_picker.signal_reload_default_exclude_required().connect(
            sigc::mem_fun(*this, &MainWindow::on_reload_default_exclude_required));

    // configure preview gallery
    gallery.signal_item_chosen().connect(
            sigc::mem_fun(*this, &MainWindow::on_gallery_item_chosen));
    gallery.signal_item_selected().connect(
            sigc::mem_fun(*this, &MainWindow::on_gallery_item_selected));
    gallery.signal_failed_to_open().connect(
            sigc::mem_fun(*this, &MainWindow::on_gallery_failed_to_open));
    gallery.signal_edit().connect(
            sigc::mem_fun(*this, &MainWindow::on_gallery_edit));

    // configure main box
    box.set_orientation(Gtk::Orientation::HORIZONTAL);
    box.append(tag_picker);
    box.append(sep);
    box.append(gallery);
    box.append(viewer);

    // configure controller
    add_controller(key_controller);
    key_controller->signal_key_pressed().connect(
            sigc::mem_fun(*this, &MainWindow::on_key_pressed), false);

    // configure item window
    item_window.set_completer_model(list_store);
    item_window.signal_add_item().connect(
            sigc::mem_fun(*this, &MainWindow::on_add_item));
    item_window.signal_edit_item().connect(
            sigc::mem_fun(*this, &MainWindow::on_edit_item));
    item_window.signal_delete_item().connect(
            sigc::mem_fun(*this, &MainWindow::on_delete_item));

    // configure dbsettings window
    db_settings_window.set_completer_model(list_store);
    db_settings_window.signal_directoires_changed().connect(
            sigc::mem_fun(*this, &MainWindow::on_directories_changed));
    db_settings_window.signal_exclude_tags_changed().connect(
            sigc::mem_fun(*this, &MainWindow::on_exclude_tags_changed));

    // configure window
    set_child(box);
    set_title("TagView");
    set_size_request(455, 400);
    set_default_size(950, 800);
}

void MainWindow::load_database(std::string db_file_path) {
    main_menu.hide();
    try {
        db.load_from_file(db_file_path);
    }
    catch (TagDb::FileParseException &ex) {
        show_warning("Error Loading Database",
                     "There was an error parsing the file at line " + std::to_string(ex.line_number));
        return;
    }
    catch (TagDb::FileErrorException &ex) {
        show_warning("Error Loading Database",
                     "There was an error opening the file:\n" + ex.file_path);

        return;
    }

    set_completer_data(db.get_all_tags());
    on_reload_default_exclude_required(); // load default excluded tags to tag_picer
    db_settings_window.setup(db.get_directories(), db.get_default_excluded_tags(), db.get_prefix());
    item_window.set_directories(db.get_directories());
    item_window.set_prefix(db.get_prefix());
    main_menu.set_show_database_controls(true);
}

void MainWindow::set_completer_data(const std::set<Glib::ustring> &completer_tags) {
    list_store->clear();
    for (const Glib::ustring &tag : completer_tags) {
        auto row = *(list_store->append());
        row[list_model.tag] = tag;
    }
}

void MainWindow::show_warning(Glib::ustring primary, Glib::ustring secondary) {
        message = std::make_unique<Gtk::MessageDialog>(*this, primary,
                                                       false, Gtk::MessageType::WARNING);
        message->set_secondary_text(secondary);
        message->set_modal(true);
        message->set_hide_on_close(true);
        message->signal_response().connect(
                sigc::hide(sigc::mem_fun(*message, &Gtk::Widget::hide)));
        message->show();
}

bool MainWindow::on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state) {
    // Ctrl + Q exit
    if (keyval == 'q' && static_cast<int>(state) == 0b00000100) {
        hide();
    }

    // viewer image switching
    if (viewer.get_visible()) {
        if (keycode == 9) { // escape key
            on_hide_viewer();
        }
        else if (keycode == 113) { // left arrow key
            // go previous
            if (files_idx == 0) { files_idx = files.size() - 1; }
            else { files_idx -= 1; }
            viewer.set_image(files.at(files_idx));
            on_gallery_item_selected(files_idx);
        }
        else if (keycode == 114) { // right arrow key
            // go next
            if (files_idx == files.size() - 1) { files_idx = 0; }
            else { files_idx += 1; }
            viewer.set_image(files.at(files_idx));
            on_gallery_item_selected(files_idx);
        }
        return true;
    }
    return false;
}

void MainWindow::on_tag_query_changed(TagQuery tag_selection) {
    files = db.query(tag_selection.tags_include, tag_selection.tags_exclude);
    gallery.set_content(files);
    if (!viewer.get_visible()) {
        tag_picker.clear_current_item_tags();
    }
}

void MainWindow::on_reload_default_exclude_required() {
    for (const Glib::ustring &tag : db.get_default_excluded_tags()) {
        tag_picker.add_excluded_tag(tag);
    }
}

void MainWindow::on_gallery_item_chosen(size_t id) {
    // storing id for arrow key navigation later
    files_idx = id;

    // enable image viewer
    gallery.set_visible(false);
    viewer_controls.set_visible(true);
    viewer.set_visible(true);
    viewer.set_image(files.at(files_idx));

    // opening an image is also selecting it for the tag picker
    on_gallery_item_selected(id);
}

void MainWindow::on_gallery_item_selected(size_t id) {
    tag_picker.set_current_item_tags(db.get_tags_for_item(files.at(id)));
}

void MainWindow::on_gallery_failed_to_open(size_t id) {
    show_warning("Failed to Load Item", files.at(id));
}

void MainWindow::on_gallery_edit(const Glib::ustring &file_path) {
    item_window.edit_item(db.get_item(file_path));
}

void MainWindow::on_hide_viewer() {
    TagQuery query = tag_picker.get_current_query();
    if (query.tags_include.size() == 0) {
        tag_picker.clear_current_item_tags();
    }

    viewer.set_visible(false);
    viewer_controls.set_visible(false);
    gallery.set_visible(true);
    gallery.grab_focus();
}

void MainWindow::on_load_database() {
    main_menu.hide();

    file_chooser = std::make_unique<Gtk::FileChooserDialog>("Choose a database file",
            Gtk::FileChooser::Action::OPEN, true);
    file_chooser->set_transient_for(*this);
    file_chooser->set_modal(true);

    file_chooser->add_button("Cancel", Gtk::ResponseType::CANCEL);
    file_chooser->add_button("Select", Gtk::ResponseType::OK);

    auto filter = Gtk::FileFilter::create();
    filter->set_name("Text files");
    filter->add_mime_type("text/plain");
    file_chooser->add_filter(filter);

    file_chooser->signal_response().connect(
            sigc::mem_fun(*this, &MainWindow::on_file_chooser_response));

    file_chooser->show();
}

void MainWindow::on_add_items() {
    main_menu.hide();
    std::vector<std::string> files;
    files.push_back("/home/user/cat.jpg");
    files.push_back("/home/user/Code/Gtk/TagView/TestGallery/screenshot.png");
    files.push_back("/home/user/falcon.jpg");
    files.push_back("/home/user/tropical.jpeg");
    files.push_back("/does_not_exist");
    item_window.add_items(files);
}

void MainWindow::on_db_settings() {
    main_menu.hide();
    db_settings_window.show();
}

void MainWindow::on_tag_picker_toggled() {
    if (main_menu.get_tag_picker_active()) {
        tag_picker.set_visible(true);
    }
    else {
        tag_picker.set_visible(false);
    }
}

void MainWindow::on_exclude_tags_changed(const std::set<Glib::ustring> &exclude_tags) {
    db.set_default_excluded_tags(exclude_tags);
}

void MainWindow::on_directories_changed(const std::set<Glib::ustring> &directories) {
    db.set_directories(directories);
    item_window.set_directories(directories);
}

void MainWindow::on_add_item(TagDb::Item item) {
    db.add_item(item);
    set_completer_data(db.get_all_tags());

    // refresh the gallery
    TagQuery query = tag_picker.get_current_query();
    gallery.set_content(db.query(query.tags_include, query.tags_exclude));
}

void MainWindow::on_edit_item(TagDb::Item item) {
    db.edit_item(item);
    set_completer_data(db.get_all_tags());

    tag_picker.set_current_item_tags(db.get_tags_for_item(db.get_prefix() + item.get_file_path()));

    // refresh the gallery
    TagQuery query = tag_picker.get_current_query();
    gallery.set_content(db.query(query.tags_include, query.tags_exclude));
}

void MainWindow::on_delete_item(const Glib::ustring &file_path, bool delete_file) {
    db.delete_item(file_path, delete_file);

    // refresh the gallery
    TagQuery query = tag_picker.get_current_query();
    gallery.set_content(db.query(query.tags_include, query.tags_exclude));
}

void MainWindow::on_file_chooser_response(int respone_id) {
    file_chooser->hide();
    if (respone_id == Gtk::ResponseType::OK) {
        load_database(file_chooser->get_file()->get_path());
    }
}

void MainWindow::on_test() {
    main_menu.hide();

    load_database("/home/user/Code/Gtk/TagView/TestGallery/database.txt");
}

MainWindow::~MainWindow() {
}
