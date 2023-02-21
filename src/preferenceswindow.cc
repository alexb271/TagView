// project
#include "preferenceswindow.hh"

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

void PreferencesWindow::on_select_default_db() {

}

bool PreferencesWindow::on_close_request() {
    hide();
    return true;
}
