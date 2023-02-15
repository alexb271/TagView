// gtkmm
#include "gtkmm/enums.h"
#include "sigc++/functors/mem_fun.h"

// project
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

    // image viewer controls are initally hidden because the gallery is shown first
    viewer_controls.set_visible(false);

    // configure main menu
    button_main_menu.set_direction(Gtk::ArrowType::NONE);
    button_main_menu.set_popover(main_menu);
    main_menu.signal_load_database().connect(sigc::mem_fun(*this, &MainWindow::load_database));
    main_menu.signal_show_tag_picker_toggled().connect(
            sigc::mem_fun(*this, &MainWindow::on_tag_picker_toggled));
    main_menu.signal_test().connect(sigc::mem_fun(*this, &MainWindow::on_test));

    // configure header
    header.set_show_title_buttons(true);
    header.pack_start(viewer_controls);
    header.pack_end(button_main_menu);
    set_titlebar(header);

    // viewer is initially hidden
    viewer.set_visible(false);

    // configure tag picker
    tag_picker.set_halign(Gtk::Align::START);
    tag_picker.set_valign(Gtk::Align::START);
    tag_picker.set_margin(15);
    tag_picker.signal_query_changed().connect(
            sigc::mem_fun(*this, &MainWindow::on_tag_query_changed));

    // configure preview gallery
    gallery.signal_item_chosen().connect(sigc::mem_fun(*this, &MainWindow::on_item_chosen));
    gallery.signal_item_selected().connect(sigc::mem_fun(*this, &MainWindow::on_item_selected));

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

    // configure window
    set_child(box);
    set_title("TagView");
    set_size_request(455, 400);
    set_default_size(950, 800);
}

void MainWindow::load_database() {
    db.load_from_file("../TestGallery/database.txt");
    tag_picker.set_completer_data(db.get_all_tags());
    main_menu.hide();
}

void MainWindow::on_tag_picker_toggled() {
    if (main_menu.get_tag_picker_active()) {
        tag_picker.set_visible(true);
    }
    else {
        tag_picker.set_visible(false);
    }
}

void MainWindow::on_tag_query_changed(TagQuery tag_selection) {
    files = db.query(tag_selection.tags_include, tag_selection.tags_exclude);
    gallery.set_content(files);
    if (!viewer.get_visible()) {
        tag_picker.clear_current_item_tags();
    }
}

void MainWindow::on_item_chosen(size_t id) {
    // storing id for arrow key navigation later
    files_idx = id;

    // enable image viewer
    gallery.set_visible(false);
    viewer_controls.set_visible(true);
    viewer.set_visible(true);
    viewer.set_image(files.at(files_idx));

    // opening an image is also selecting it for the tag picker
    on_item_selected(id);
}

void MainWindow::on_item_selected(size_t id) {
    tag_picker.set_current_item_tags(db.get_tags_for_item(files.at(id)));
}

bool MainWindow::on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state) {
    if (viewer.get_visible()) {
        if (keycode == 9) { // escape key
            on_hide_viewer();
        }
        else if (keycode == 113) { // left arrow key
            // go previous
            if (files_idx == 0) { files_idx = files.size() - 1; }
            else { files_idx -= 1; }
            viewer.set_image(files.at(files_idx));
            on_item_selected(files_idx);
        }
        else if (keycode == 114) { // right arrow key
            // go next
            if (files_idx == files.size() - 1) { files_idx = 0; }
            else { files_idx += 1; }
            viewer.set_image(files.at(files_idx));
            on_item_selected(files_idx);
        }
        return true;
    }
    return false;
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

void MainWindow::on_test() {
    main_menu.hide();
    add_item_window.set_completer_data(db.get_all_tags());
    add_item_window.show();
}

MainWindow::~MainWindow() {
}
