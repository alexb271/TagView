// standard library
#include <filesystem>

// project
#include "mainwindow.hh"

MainWindow::MainWindow()
:
    item_window(*this),
    db_settings_window(*this),
    preferences_window(*this),
    key_controller(Gtk::EventControllerKey::create()),
    switching_allowed(true)
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
    main_menu.signal_create_database().connect(sigc::mem_fun(*this, &MainWindow::on_create_database));
    main_menu.signal_add_items().connect(sigc::mem_fun(*this, &MainWindow::on_add_items));
    main_menu.signal_db_settings().connect(sigc::mem_fun(*this, &MainWindow::on_db_settings));
    main_menu.signal_show_tag_picker_toggled().connect(
            sigc::mem_fun(*this, &MainWindow::on_tag_picker_toggled));
    main_menu.signal_about().connect(sigc::mem_fun(*this, &MainWindow::on_about));
    main_menu.signal_preferences().connect(sigc::mem_fun(*this, &MainWindow::on_preferences));

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
    gallery.set_preview_size(config.get_preview_size());
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
    item_window.signal_request_suggestions().connect(
            sigc::mem_fun(*this, &MainWindow::on_request_suggestions));

    // configure dbsettings window
    db_settings_window.set_completer_model(list_store);
    db_settings_window.signal_directoires_changed().connect(
            sigc::mem_fun(*this, &MainWindow::on_directories_changed));
    db_settings_window.signal_exclude_tags_changed().connect(
            sigc::mem_fun(*this, &MainWindow::on_exclude_tags_changed));

    // configure preferences window
    preferences_window.set_default_db_path(config.get_default_db_path());
    preferences_window.set_preview_size(config.get_preview_size());
    preferences_window.signal_select_defualt_db().connect(
            sigc::mem_fun(*this, &MainWindow::on_select_default_db));
    preferences_window.signal_set_preview_size().connect(
            sigc::mem_fun(*this, &MainWindow::on_set_preview_size));

    // configure window
    set_child(box);
    set_title("TagView");
    set_size_request(455, 400);
    set_default_size(950, 800);

    // load default config if it exists
    if (config.get_default_db_path().size() > 0) {
        load_database(config.get_default_db_path());
    }
}

void MainWindow::load_database(const std::string &db_file_path) {
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

    // load default excluded tags to tag_picer
    tag_picker.clear_excluded_tags();
    on_reload_default_exclude_required();

    db_settings_window.setup(db_file_path,
                             db.get_directories(),
                             db.get_default_excluded_tags(),
                             db.get_prefix());

    item_window.set_directories(db.get_directories());
    item_window.set_prefix(db.get_prefix());

    main_menu.set_show_database_controls(true);

    if (gallery.is_visible()) {
        refresh_gallery();
    }
}

void MainWindow::add_items(const std::vector<std::string> &file_paths) {
    item_window.add_items(file_paths);
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
        message->set_transient_for(*this);
        message->set_modal(true);
        message->show();
}

void MainWindow::refresh_gallery() {
    TagQuery query = tag_picker.get_current_query();
    files = db.query(query.tags_include, query.tags_exclude);
    gallery.set_content(files);
    tag_picker.clear_current_item_tags();
}

bool MainWindow::on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state) {
    // Ctrl + Q exit
    if (keyval == 'q' && static_cast<int>(state) == 0b00000100) {
        hide();
    }

    // viewer image switching
    if (viewer.get_visible() && switching_allowed) {
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
        switching_allowed = true;
    }
    else {
        switching_allowed = false;
    }
}

void MainWindow::on_reload_default_exclude_required() {
    for (const Glib::ustring &tag : db.get_default_excluded_tags()) {
        tag_picker.add_excluded_tag(tag);
    }
    refresh_gallery();
    if (viewer.get_visible()) {
        switching_allowed = false;
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
    switching_allowed = true;
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
            sigc::bind(sigc::mem_fun(*this, &MainWindow::on_file_chooser_response), Action::LOAD));

    file_chooser->show();
}

void MainWindow::on_create_database() {
    main_menu.hide();

    file_chooser = std::make_unique<Gtk::FileChooserDialog>("Choose database root directory",
            Gtk::FileChooser::Action::SELECT_FOLDER, true);
    file_chooser->set_transient_for(*this);
    file_chooser->set_modal(true);

    file_chooser->add_button("Cancel", Gtk::ResponseType::CANCEL);
    file_chooser->add_button("Select", Gtk::ResponseType::OK);

    file_chooser->signal_response().connect(
            sigc::bind(sigc::mem_fun(*this, &MainWindow::on_file_chooser_response), Action::CREATE));

    file_chooser->show();
}

void MainWindow::on_add_items() {
    main_menu.hide();

    file_chooser = std::make_unique<Gtk::FileChooserDialog>("Choose files to add",
            Gtk::FileChooser::Action::OPEN, true);
    file_chooser->set_select_multiple(true);
    file_chooser->set_transient_for(*this);
    file_chooser->set_modal(true);

    file_chooser->add_button("Cancel", Gtk::ResponseType::CANCEL);
    file_chooser->add_button("Select", Gtk::ResponseType::OK);

    auto filter = Gtk::FileFilter::create();
    filter->set_name("Image files");
    filter->add_mime_type("image/png");
    filter->add_mime_type("image/jpg");
    filter->add_mime_type("image/jpeg");
    filter->add_mime_type("image/tiff");
    filter->add_mime_type("image/gif");
    file_chooser->add_filter(filter);

    auto filter_all = Gtk::FileFilter::create();
    filter_all->set_name("All Files");
    filter_all->add_pattern("*");
    file_chooser->add_filter(filter_all);

    file_chooser->signal_response().connect(
            sigc::bind(sigc::mem_fun(*this, &MainWindow::on_file_chooser_response), Action::ADD));

    file_chooser->show();
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

void MainWindow::on_preferences() {
    main_menu.hide();
    preferences_window.show();
}

void MainWindow::on_about() {
    main_menu.hide();
    about_dialog = std::make_unique<Gtk::AboutDialog>();

    // use the same icon as the default gnome image viewer
    // which also depends on the system theme
    about_dialog->set_logo_icon_name("org.gnome.eog");
    about_dialog->set_program_name("TagView");
    about_dialog->set_comments("A tagging image viewer");
    about_dialog->set_version("0.1");
    about_dialog->set_website("https://github.com/alexb271/tagview/");
    about_dialog->set_website_label("https://github.com/alexb271/tagview/");
    about_dialog->set_license_type(Gtk::License::GPL_3_0);
    about_dialog->set_transient_for(*this);
    about_dialog->set_modal(true);
    about_dialog->show();
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

    refresh_gallery();
}

void MainWindow::on_edit_item(TagDb::Item item) {
    db.edit_item(item);
    set_completer_data(db.get_all_tags());

    refresh_gallery();
}

void MainWindow::on_request_suggestions(const std::set<Glib::ustring> &tags) {
    item_window.set_suggestions(db.suggestions(tags));
}

void MainWindow::on_delete_item(const Glib::ustring &file_path, bool delete_file) {
    db.delete_item(file_path, delete_file);
    set_completer_data(db.get_all_tags());

    gallery.clear_cache();
    refresh_gallery();
}

void MainWindow::on_select_default_db(const std::string &default_db_path) {
    config.set_default_db_path(default_db_path);
}

void MainWindow::on_set_preview_size(PreviewGallery::PreviewSize size) {
    config.set_preview_size(size);

    gallery.set_preview_size(size);
    refresh_gallery();
}

void MainWindow::on_file_chooser_response(int respone_id, MainWindow::Action action) {
    file_chooser->hide();
    switch (action) {
        case MainWindow::Action::LOAD: {
            if (respone_id == Gtk::ResponseType::OK) {
                load_database(file_chooser->get_file()->get_path());
            }
            break;
        }
        case MainWindow::Action::CREATE: {
            if (respone_id == Gtk::ResponseType::OK) {
                std::string db_path = file_chooser->get_file()->get_path() + "/TagView.txt";
                if (std::filesystem::exists(db_path)) {
                    show_warning("Error Creating Database",
                                 "A database file already exists at the chosen location.");
                }
                else {
                    db.create_database(db_path);
                    load_database(db_path);
                }
            }
            break;
        }
        case MainWindow::Action::ADD: {
            if (respone_id == Gtk::ResponseType::OK) {
                std::vector<std::string> file_paths;

                // the gtkmm wrapper is a garbage fire when it comes to multiple files
                // from a FileChooserDialog, use C API instead
                GListModel *files = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(file_chooser->gobj()));
                size_t files_size = g_list_model_get_n_items(files);
                for (size_t idx = 0; idx < files_size; idx++) {
                    GFile *file = (GFile *)g_list_model_get_item(files, idx);
                    file_paths.push_back(g_file_get_path(file));
                }

                add_items(file_paths);
            }
            break;
        }
    }
}
