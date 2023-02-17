// project
#include "itemwindow.hh"

ItemWindow::ItemWindow()
:
    label("Add Item Window")
{

    // tag editor setup
    tag_editor.set_hexpand(false);
    tag_editor.set_halign(Gtk::Align::START);
    tag_editor.set_valign(Gtk::Align::START);
    tag_editor.set_label_markup("<span weight=\"bold\" size=\"large\">Tags</span>");

    // box setup
    box.set_margin(15);
    box.set_spacing(15);
    box.set_orientation(Gtk::Orientation::VERTICAL);

    // append items to box
    box.append(label);
    box.append(tag_editor);

    // scrolled window setup
    scw.set_propagate_natural_height(true);
    scw.set_propagate_natural_width(true);
    scw.set_child(box);

    set_child(scw);
    set_modal(true);
    set_size_request(200, 200);
    set_default_size(400, 600);
}

void ItemWindow::set_completer_model(Glib::RefPtr<Gtk::ListStore> completer_list) {
    tag_editor.set_completer_model(completer_list);
}

bool ItemWindow::on_close_request() {
    hide();
    return true;
}

#include <iostream>
void ItemWindow::on_tags_changed(const std::vector<Glib::ustring> &tags) {
    for (const auto &tag : tags) {
        std::cout << tag << "\n";
    }
}
