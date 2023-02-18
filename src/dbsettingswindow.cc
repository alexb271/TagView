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

    // tag picker setup
    tp_exclude.set_label_markup("<span weight=\"bold\" size=\"large\">Tags</span>");
    tp_exclude.signal_contents_changed().connect(
            sigc::mem_fun(*this, &DbSettingsWindow::on_tags_changed));

    // dir list setup
    dirs.signal_contents_changed().connect(
            sigc::mem_fun(*this, &DbSettingsWindow::on_dirs_changed));

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
}

void DbSettingsWindow::set_completer_model(Glib::RefPtr<Gtk::ListStore> completer_list) {
    tp_exclude.set_completer_model(completer_list);
}

void DbSettingsWindow::reset(const std::set<Glib::ustring> &directories,
                             const std::set<Glib::ustring> &default_exclude_tags)
{
    for (const Glib::ustring &dir : directories) {
        dirs.append(dir);
    }

    tp_exclude.clear();
    for (const Glib::ustring &tag : default_exclude_tags) {
        tp_exclude.add_tag(tag);
    }
}

bool DbSettingsWindow::on_close_request() {
    hide();
    return true;
}

void DbSettingsWindow::on_dirs_changed(const std::set<Glib::ustring> &dirs) {
}

void DbSettingsWindow::on_tags_changed(const std::set<Glib::ustring> &dirs) {
}
