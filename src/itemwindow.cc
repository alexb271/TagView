// project
#include "itemwindow.hh"

ItemWindow::ItemWindow()
:
    in_edit_mode(false),
    current_idx(0)
{
    // label setup
    lbl_copy_to_dir.set_markup("<span weight=\"bold\" size=\"large\">Copy To Directory</span>");
    lbl_copy_to_dir.set_halign(Gtk::Align::START);

    // picture setup
    item_picture.set_can_shrink(true);
    item_picture.set_size_request(256, 256);
    item_picture.set_margin_top(10);
    item_picture.set_margin_bottom(10);

    // combo box setup
    combo_dirs.set_halign(Gtk::Align::START);

    // tag editor setup
    tag_editor.set_label_markup("<span weight=\"bold\" size=\"large\">Tags</span>");
    tag_editor.set_hexpand(false);
    tag_editor.set_halign(Gtk::Align::START);
    tag_editor.set_valign(Gtk::Align::START);
    tag_editor.set_size_request(-1, 200);

    // buttons setup
    chk_fav.set_label(" Favorite");

    btn_skip.set_label("Skip");
    btn_skip.signal_clicked().connect(
            sigc::mem_fun(*this, &ItemWindow::on_skip));

    btn_add.set_label("Add");
    btn_add.set_halign(Gtk::Align::END);
    btn_add.set_hexpand(true);
    btn_add.signal_clicked().connect(
            sigc::mem_fun(*this, &ItemWindow::on_add));

    buttons_skip_add.set_orientation(Gtk::Orientation::HORIZONTAL);
    buttons_skip_add.append(btn_skip);
    buttons_skip_add.append(btn_add);

    // box setup
    box.set_margin(15);
    box.set_spacing(20);
    box.set_orientation(Gtk::Orientation::VERTICAL);

    // append items to box
    box.append(lbl_item_path);
    box.append(item_picture);
    box.append(lbl_copy_to_dir);
    box.append(combo_dirs);
    box.append(chk_fav);
    box.append(tag_editor);
    box.append(buttons_skip_add);

    set_child(box);
    set_size_request(200, 200);
    set_default_size(400, 600);
}

void ItemWindow::set_completer_model(Glib::RefPtr<Gtk::ListStore> completer_list) {
    tag_editor.set_completer_model(completer_list);
}

void ItemWindow::set_directories(const std::set<Glib::ustring> &directories) {
    combo_dirs.remove_all();
    combo_dirs.append("Database root");
    combo_dirs.set_active(0);

    for (const Glib::ustring &dir : directories) {
        combo_dirs.append(dir);
    }
}

void ItemWindow::set_prefix(const std::string &prefix) {
    this->prefix = prefix;
}

void ItemWindow::add_items(const std::vector<std::string> &file_paths) {
    if (file_paths.size() == 0) { return; }
    items_to_add = file_paths;
    current_idx = 0;
    setup_for_item(current_idx);
    show();
}

sigc::signal<void ()> ItemWindow::signal_add_item() {
    return private_add_item;
}

void ItemWindow::setup_for_item(size_t idx) {
    set_title(std::to_string(idx + 1) + "/" + std::to_string(items_to_add.size()));
    lbl_item_path.set_text(items_to_add.at(idx));
    item_picture.set_filename(items_to_add.at(idx));
    combo_dirs.set_active(0);

    // do not copy if file is already in a valid subdirectory
    if (items_to_add.at(idx).rfind(prefix, 0) == 0) {
        combo_dirs.set_sensitive(false);
        item_starts_with_prefix = true;
    }
    else {
        combo_dirs.set_sensitive(true);
        item_starts_with_prefix = false;
    }

    chk_fav.set_active(false);
    tag_editor.clear();
}

bool ItemWindow::on_close_request() {
    hide();
    return true;
}

void ItemWindow::on_add() {
    if (current_idx == items_to_add.size() - 1) {
        hide();
    }
    else {
        private_add_item.emit();
        current_idx += 1;
        setup_for_item(current_idx);
    }
}

void ItemWindow::on_skip() {
    if (current_idx == items_to_add.size() - 1) {
        hide();
    }
    else {
        current_idx += 1;
        setup_for_item(current_idx);
    }
}
