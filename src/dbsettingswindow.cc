// project
#include "dbsettingswindow.hh"

DbSettingsWindow::DbSettingsWindow()
:
    dirs(ItemList::Type::INSIDE)
{
    // label setup
    lbl_dirs.set_markup("<span weight=\"bold\" size=\"large\">Directories</span>");
    lbl_dirs.set_halign(Gtk::Align::START);

    lbl_default_exclude.set_markup("<span weight=\"bold\" size=\"large\">Default Exclude</span>");
    lbl_default_exclude.set_halign(Gtk::Align::START);
    lbl_default_exclude.set_margin_top(30);

    // button setup
    btn_add_dir.set_label("Add Directory");
    btn_add_dir.set_halign(Gtk::Align::START);
    btn_add_dir.signal_clicked().connect(
            sigc::mem_fun(*this, &DbSettingsWindow::on_add_directory));


    // tag picker setup
    tp_exclude.set_label_markup("<span weight=\"bold\" size=\"large\">Tags</span>");

    // warning dialog setup
    subdir_warning = std::make_unique<Gtk::MessageDialog>(*this, "Error Adding Directory",
            false, Gtk::MessageType::WARNING);
    subdir_warning->set_secondary_text("Directory must be a subdirectory of where the database file is located.");
    subdir_warning->set_hide_on_close(true);
    subdir_warning->signal_response().connect(
            sigc::hide(sigc::mem_fun(*subdir_warning, &Gtk::Widget::hide)));

    // box setup
    box.set_margin(15);
    box.set_spacing(30);
    box.set_orientation(Gtk::Orientation::VERTICAL);

    box.append(lbl_dirs);
    box.append(dirs);
    box.append(btn_add_dir);
    box.append(lbl_default_exclude);
    box.append(tp_exclude);

    // window setup (self)
    set_child(box);
    set_title("DB Settings");
    set_size_request(200, 200);
    set_default_size(400, 600);
}

void DbSettingsWindow::set_completer_model(Glib::RefPtr<Gtk::ListStore> completer_list) {
    tp_exclude.set_completer_model(completer_list);
}

void DbSettingsWindow::setup(const std::set<Glib::ustring> &directories,
                             const std::set<Glib::ustring> &default_exclude_tags,
                             const std::string &prefix)
{
    dirs.clear();
    for (const Glib::ustring &dir : directories) {
        dirs.append(dir);
    }

    tp_exclude.clear();
    for (const Glib::ustring &tag : default_exclude_tags) {
        tp_exclude.add_tag(tag);
    }

    this->prefix = prefix;
}


sigc::signal<void (const std::set<Glib::ustring> &)> DbSettingsWindow::signal_exclude_tags_changed() {
    return tp_exclude.signal_contents_changed();
}

sigc::signal<void (const std::set<Glib::ustring> &)> DbSettingsWindow::signal_directoires_changed() {
    return dirs.signal_contents_changed();
}

bool DbSettingsWindow::on_close_request() {
    hide();
    return true;
}

void DbSettingsWindow::on_add_directory() {
    file_chooser = std::make_unique<Gtk::FileChooserDialog>("Choose a directory",
            Gtk::FileChooser::Action::SELECT_FOLDER, true);
    file_chooser->set_transient_for(*this);
    file_chooser->set_modal(true);

    file_chooser->add_button("Cancel", Gtk::ResponseType::CANCEL);
    file_chooser->add_button("Select", Gtk::ResponseType::OK);

    file_chooser->signal_response().connect(
            sigc::mem_fun(*this, &DbSettingsWindow::on_file_chooser_response));

    file_chooser->show();
}

void DbSettingsWindow::on_file_chooser_response(int respone_id) {
    file_chooser->hide();
    if (respone_id == Gtk::ResponseType::OK) {
        std::string dirname = file_chooser->get_file()->get_path();

        // add directory if the string start with the prefix
        if (dirname.rfind(prefix, 0) == 0) {
            dirname = dirname.substr(prefix.size());
            dirs.append_and_notify(dirname);
        }
        else {
            subdir_warning->show();
        }
    }
}
