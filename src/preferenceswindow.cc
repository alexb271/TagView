// project
#include "preferenceswindow.hh"
#include "gtkmm/dialog.h"

PreferencesWindow::PreferencesWindow() {
    // label setup
    lbl_default_db_title.set_markup("<span weight=\"bold\" size=\"large\">Default Database</span>");
    lbl_preview_size.set_markup("<span weight=\"bold\" size=\"large\">Preview Size</span>");

    lbl_default_db_title.set_halign(Gtk::Align::START);
    lbl_default_db_path.set_halign(Gtk::Align::START);
    lbl_preview_size.set_halign(Gtk::Align::START);

    // button setup
    btn_select_default_db.set_label("Select");
    btn_select_default_db.set_halign(Gtk::Align::START);
    btn_select_default_db.set_expand(false);
    btn_select_default_db.signal_clicked().connect(
            sigc::mem_fun(*this, &PreferencesWindow::on_select_default_db));

    // checkbutton setup
    small.set_label(" Small");
    medium.set_label(" Medium");
    large.set_label(" Large");

    medium.set_group(small);
    large.set_group(small);

    small.signal_toggled().connect(sigc::mem_fun(*this, &PreferencesWindow::on_toggled));
    medium.signal_toggled().connect(sigc::mem_fun(*this, &PreferencesWindow::on_toggled));
    large.signal_toggled().connect(sigc::mem_fun(*this, &PreferencesWindow::on_toggled));

    // box setup
    box.set_orientation(Gtk::Orientation::VERTICAL);
    box.set_margin(30);
    box.set_spacing(30);
    box.set_halign(Gtk::Align::START);
    checkbuttons.set_orientation(Gtk::Orientation::HORIZONTAL);
    checkbuttons.set_spacing(30);

    // pack widgets
    checkbuttons.append(small);
    checkbuttons.append(medium);
    checkbuttons.append(large);

    box.append(lbl_default_db_title);
    box.append(lbl_default_db_path);
    box.append(btn_select_default_db);
    box.append(lbl_preview_size);
    box.append(checkbuttons);

    // window setup
    set_title("Preferences");
    set_resizable(false);
    set_child(box);
}

void PreferencesWindow::set_default_db_path(const Glib::ustring &default_db_path) {
    lbl_default_db_path.set_text(default_db_path);
}

void PreferencesWindow::set_preview_size(PreviewGallery::PreviewSize size) {
    switch(size) {
        case PreviewGallery::PreviewSize::Small: small.set_active(true); break;
        case PreviewGallery::PreviewSize::Medium: medium.set_active(true); break;
        case PreviewGallery::PreviewSize::Large: large.set_active(true); break;
    }
}

sigc::signal<void (const std::string &)> PreferencesWindow::signal_select_defualt_db() {
    return private_select_defualt_db;
}

sigc::signal<void (PreviewGallery::PreviewSize)> PreferencesWindow::signal_set_preview_size() {
    return private_set_preview_size;
}

void PreferencesWindow::on_select_default_db() {
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
            sigc::mem_fun(*this, &PreferencesWindow::on_file_chooser_response));

    file_chooser->show();
}

void PreferencesWindow::on_toggled() {
    if (small.get_active()) {
        private_set_preview_size.emit(PreviewGallery::PreviewSize::Small);
    }
    else if (medium.get_active()) {
        private_set_preview_size.emit(PreviewGallery::PreviewSize::Medium);
    }
    else if (large.get_active()) {
        private_set_preview_size.emit(PreviewGallery::PreviewSize::Large);
    }
}

void PreferencesWindow::on_file_chooser_response(int respone_id) {
    file_chooser->hide();
    if (respone_id == Gtk::ResponseType::OK) {
        lbl_default_db_path.set_text(file_chooser->get_file()->get_path());
        private_select_defualt_db.emit(file_chooser->get_file()->get_path());
    }
}

bool PreferencesWindow::on_close_request() {
    hide();
    return true;
}
