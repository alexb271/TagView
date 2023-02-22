// standard library
#include <filesystem>

// project
#include "itemwindow.hh"

ItemWindow::ItemWindow()
:
    tag_suggestions(ItemList::Type::OUTSIDE),
    default_directory("Database root"),
    in_edit_mode(false),
    current_idx(0),
    suggestion_count(5),
    preview_size(256)
{
    // label setup
    lbl_copy_to_dir.set_markup("<span weight=\"bold\" size=\"large\">Copy To Directory</span>");
    lbl_copy_to_dir.set_halign(Gtk::Align::START);

    // preview setup
    item_preview_error.set_markup("<span weight=\"bold\" size=\"large\">Error loading preview</span>");
    item_preview_error.set_margin_top(10);
    item_preview_error.set_margin_bottom(10);
    item_preview_error.set_visible(false);
    item_preview.set_size_request(preview_size, preview_size);
    item_preview.set_margin_top(10);
    item_preview.set_margin_bottom(10);
    item_preview.set_visible(false);

    // combo box setup
    combo_dirs.set_halign(Gtk::Align::START);

    // tag editor setup
    tag_editor.set_label_markup("<span weight=\"bold\" size=\"large\">Tags</span>");
    tag_editor.set_hexpand(false);
    tag_editor.set_halign(Gtk::Align::START);
    tag_editor.set_valign(Gtk::Align::START);
    tag_editor.set_size_request(-1, 200);
    tag_editor.signal_contents_changed().connect(
            sigc::mem_fun(*this, &ItemWindow::on_tag_editor_contents_changed));

    // suggestions setup
    lbl_suggestions.set_markup("<span weight=\"bold\" size=\"medium\">Suggestions</span>");
    lbl_suggestions.set_margin_start(60);
    lbl_suggestions.set_valign(Gtk::Align::END);
    tag_suggestions.set_margin_start(60);
    tag_suggestions.set_margin_top(15);
    tag_suggestions.set_valign(Gtk::Align::START);
    tag_suggestions.signal_add().connect(
            sigc::mem_fun(*this, &ItemWindow::on_add_suggestion));

    // suggestions are only shown if there are contents
    lbl_suggestions.set_visible(false);
    tag_suggestions.set_visible(false);

    // editor grid setup
    editor_grid.set_expand(false);
    editor_grid.set_halign(Gtk::Align::START);
    editor_grid.attach(tag_editor, 0, 0, 1, 2);
    editor_grid.attach(lbl_suggestions, 1, 0);
    editor_grid.attach(tag_suggestions, 1, 1);

    // checkbox setup
    chk_fav.set_label(" Favorite");

    // add mode buttons
    btn_skip.set_label("Skip");
    btn_skip.signal_clicked().connect(
            sigc::mem_fun(*this, &ItemWindow::on_skip));

    btn_add.set_label("Add");
    btn_add.set_halign(Gtk::Align::END);
    btn_add.set_hexpand(true);
    btn_add.signal_clicked().connect(
            sigc::mem_fun(*this, &ItemWindow::on_add));

    buttons_mode_add.set_orientation(Gtk::Orientation::HORIZONTAL);
    buttons_mode_add.set_valign(Gtk::Align::END);
    buttons_mode_add.append(btn_skip);
    buttons_mode_add.append(btn_add);

    // edit_mode_buttons
    btn_delete.set_label("Delete");
    btn_delete.signal_clicked().connect(
            sigc::mem_fun(*this, &ItemWindow::on_delete));


    btn_edit.set_label("Apply");
    btn_edit.set_halign(Gtk::Align::END);
    btn_edit.set_hexpand(true);
    btn_edit.signal_clicked().connect(
            sigc::mem_fun(*this, &ItemWindow::on_edit));

    buttons_mode_edit.set_orientation(Gtk::Orientation::HORIZONTAL);
    buttons_mode_edit.set_valign(Gtk::Align::END);
    buttons_mode_edit.append(btn_delete);
    buttons_mode_edit.append(btn_edit);

    // box setup
    box.set_margin(15);
    box.set_spacing(20);
    box.set_orientation(Gtk::Orientation::VERTICAL);

    // append items to box
    box.append(lbl_item_path);
    box.append(item_preview_error);
    box.append(item_preview);
    box.append(lbl_copy_to_dir);
    box.append(combo_dirs);
    box.append(chk_fav);
    box.append(editor_grid);
    box.append(buttons_mode_add);

    set_child(box);
    set_size_request(200, 200);
    set_default_size(400, 600);
}

void ItemWindow::set_completer_model(Glib::RefPtr<Gtk::ListStore> completer_list) {
    tag_editor.set_completer_model(completer_list);
}

void ItemWindow::set_directories(const std::set<Glib::ustring> &directories) {
    combo_dirs.remove_all();
    combo_dirs.append(default_directory);
    combo_dirs.set_active(0);

    for (const Glib::ustring &dir : directories) {
        combo_dirs.append(dir);
    }
}

void ItemWindow::set_prefix(const std::string &prefix) {
    this->prefix = prefix;
}

void ItemWindow::set_suggestions(const std::vector<Glib::ustring> &tags) {
    if (tags.size() == 0) {
        lbl_suggestions.set_visible(false);
        tag_suggestions.set_visible(false);
        return;
    }

    size_t count = suggestion_count < tags.size() ? suggestion_count : tags.size();
    size_t idx = 0;

    tag_suggestions.clear();
    for (size_t i = 0; i < count && idx < tags.size(); i++, idx++) {
        if (tag_editor.contains(tags.at(idx))) {
            i -= 1;
        }
        else {
            tag_suggestions.append(tags.at(idx));
        }
    }

    if (tag_suggestions.size() > 0) {
        lbl_suggestions.set_visible(true);
        tag_suggestions.set_visible(true);
    }
    else {
        lbl_suggestions.set_visible(false);
        tag_suggestions.set_visible(false);
    }
}

void ItemWindow::add_items(const std::vector<std::string> &file_paths) {
    if (file_paths.size() == 0) { return; }
    if (in_edit_mode) {
        box.remove(buttons_mode_edit);
        box.append(buttons_mode_add);
    }
    in_edit_mode = false;
    items_to_add = file_paths;
    current_idx = 0;
    setup_for_add_item(current_idx);
    show();
}

void ItemWindow::edit_item(const TagDb::Item &item) {
    if (!in_edit_mode) {
        box.remove(buttons_mode_add);
        box.append(buttons_mode_edit);
    }
    in_edit_mode = true;
    current_edited_item_type = item.get_type();
    current_edited_item_file_path = item.get_file_path();
    setup_for_edit_item(item);
    show();
}

sigc::signal<void (TagDb::Item)> ItemWindow::signal_add_item() {
    return private_add_item;
}

sigc::signal<void (TagDb::Item)> ItemWindow::signal_edit_item() {
    return private_edit_item;
}

sigc::signal<void (const std::set<Glib::ustring> &)> ItemWindow::signal_request_suggestions() {
    return private_request_suggestions;
}

sigc::signal<void (const Glib::ustring &, bool)> ItemWindow::signal_delete_item() {
    return private_delete_item;
}

void ItemWindow::setup_for_add_item(size_t idx) {
    set_title(std::to_string(idx + 1) + "/" + std::to_string(items_to_add.size()));
    lbl_item_path.set_text(items_to_add.at(idx));
    set_preview(items_to_add.at(idx));


    // do not copy if file is already in a valid subdirectory
    if (items_to_add.at(idx).rfind(prefix, 0) == 0) {
        item_starts_with_prefix = true;

        lbl_copy_to_dir.set_visible(false);
        combo_dirs.set_visible(false);
    }
    else {
        item_starts_with_prefix = false;

        lbl_copy_to_dir.set_visible(true);
        combo_dirs.set_visible(true);
        combo_dirs.set_active(0);
    }

    chk_fav.set_active(false);
    tag_editor.clear();
    tag_suggestions.clear();
    lbl_suggestions.set_visible(false);
    tag_suggestions.set_visible(false);
}

void ItemWindow::setup_for_edit_item(const TagDb::Item &item)
{
    set_title("Edit Item");
    lbl_item_path.set_text(item.get_file_path());
    set_preview(prefix + item.get_file_path());
    chk_fav.set_active(item.get_favorite());

    lbl_copy_to_dir.set_visible(false);
    combo_dirs.set_visible(false);

    tag_editor.clear();
    for (const Glib::ustring &tag : item.get_tags()) {
        tag_editor.add_tag(tag);
    }
    private_request_suggestions.emit(tag_editor.get_content());
}

bool ItemWindow::set_preview(const Glib::ustring &file_path) {
    Glib::RefPtr<Gdk::Pixbuf> pbuf;
    try {
        pbuf = Gdk::Pixbuf::create_from_file(file_path);
    }
    catch (...) {
        item_preview_error.set_visible(true);
        item_preview.set_visible(false);
        return false;
    }

    // calculate scale proportion based on the longer dimension
    int image_size = pbuf->get_width() > pbuf->get_height() ? pbuf->get_width() : pbuf->get_height();
    double prop = (double)(preview_size) / (double)image_size;


    // scale image by proportion
    pbuf = pbuf->scale_simple(
            std::round(pbuf->get_width() * prop),
            std::round(pbuf->get_height() * prop),
            Gdk::InterpType::BILINEAR);

    item_preview.set(pbuf);

    item_preview_error.set_visible(false);
    item_preview.set_visible(true);
    return true;
}

bool ItemWindow::copy(const std::string &file_path) {
    // item name is the part of the path after the last /
    std::string item_name = file_path.substr(file_path.find_last_of("/") + 1);

    // configure directory
    std::string dir = combo_dirs.get_active_text();
    if (dir == default_directory) { dir = ""; }
    else { dir = dir + "/"; }

    // establish destination
    std::string destination_path = prefix + dir + item_name;

    // check if the source file exists
    if (!std::filesystem::exists(file_path)) {
        show_warning("Error Copying File", "The source file could not be found.");
        return false;
    }

    // check if the destination file already exists
    if (std::filesystem::exists(destination_path)) {
        show_warning("Error Copying File", "The destination file already exists.");
        return false;
    }

    // perform the copy
    try {
        std::filesystem::copy_file(file_path, destination_path);
    }
    catch (...) {
        show_warning("Error Copying File", "There was an error while copying the file.");
        return false;
    }

    return true;
}

TagDb::Item ItemWindow::create_db_item(size_t idx) {
    std::string item_path;
    if (item_starts_with_prefix) {
        // if the full path starts with the prefix, then the
        // relative path is everything after the prefix
        item_path = items_to_add.at(idx).substr(prefix.size());
    }
    else {
        // item name is the part of the path after the last /
        std::string item_name = items_to_add.at(idx).substr(items_to_add.at(idx).find_last_of("/") + 1);

        // configure directory
        std::string dir = combo_dirs.get_active_text();
        if (dir == default_directory) { dir = ""; }
        else { dir = dir + "/"; }

        item_path = dir + item_name;
    }

    return TagDb::Item(item_path, TagDb::Item::Type::image,
                       tag_editor.get_content(), chk_fav.get_active());
}

void ItemWindow::show_warning(Glib::ustring primary, Glib::ustring secondary) {
        message = std::make_unique<Gtk::MessageDialog>(*this, primary,
                                                       false, Gtk::MessageType::WARNING);
        message->set_secondary_text(secondary);
        message->set_modal(true);
        message->set_hide_on_close(true);
        message->signal_response().connect(
                sigc::hide(sigc::mem_fun(*message, &Gtk::Widget::hide)));
        message->show();
}

void ItemWindow::on_tag_editor_contents_changed(const std::set<Glib::ustring> &tags) {
    private_request_suggestions.emit(tag_editor.get_content());
}

void ItemWindow::on_add_suggestion(const Glib::ustring &tag) {
    tag_editor.add_tag(tag);
    private_request_suggestions.emit(tag_editor.get_content());
}

void ItemWindow::on_add() {
    if (tag_editor.get_content().size() == 0) {
        show_warning("Error Adding Item", "Provide at least one tag for the item");
        return;
    }

    else {
        bool result = true;
        if (!item_starts_with_prefix) {
            result = copy(items_to_add.at(current_idx));
        }
        if (result) {
            private_add_item.emit(create_db_item(current_idx));

            current_idx += 1;
            if (current_idx == items_to_add.size()) {
                hide();
            }
            else {
                setup_for_add_item(current_idx);
            }
        }
    }
}

void ItemWindow::on_skip() {
    if (current_idx == items_to_add.size() - 1) {
        hide();
    }
    else {
        current_idx += 1;
        setup_for_add_item(current_idx);
    }
}

void ItemWindow::on_edit() {
    TagDb::Item result(current_edited_item_file_path,
                       current_edited_item_type,
                       tag_editor.get_content(),
                       chk_fav.get_active());
    private_edit_item.emit(result);
    hide();
}

void ItemWindow::on_delete() {
    delete_dialog = std::make_unique<ItemWindow::DeleteDialog>(*this);
    delete_dialog->show();
}

void ItemWindow::on_delete_respone(int respone_id) {
    delete_dialog->hide();
    if (respone_id == Gtk::ResponseType::OK) {
        private_delete_item.emit(prefix + current_edited_item_file_path,
                                 delete_dialog->get_delete_file());
        hide();
    }
}

bool ItemWindow::on_close_request() {
    hide();
    return true;
}

// DeleteDialog implementation
ItemWindow::DeleteDialog::DeleteDialog(ItemWindow &parent)
:
    Gtk::MessageDialog(parent, "Delete Item?", false, Gtk::MessageType::QUESTION,
                       Gtk::ButtonsType::NONE, true)
{
    add_button("Cancel", Gtk::ResponseType::CANCEL);
    add_button("Delete", Gtk::ResponseType::OK);
    set_secondary_text("Are you sure you want to delete this item?");
    set_hide_on_close(true);
    set_modal(true);

    chk_delete_file.set_label(" Also delete file");
    chk_delete_file.set_halign(Gtk::Align::CENTER);
    get_content_area()->append(chk_delete_file);

    signal_response().connect(sigc::mem_fun(parent, &ItemWindow::on_delete_respone));
}

bool ItemWindow::DeleteDialog::get_delete_file() {
    return chk_delete_file.get_active();
}
