// standard library
#include <memory>
#include <stdexcept>

// gtkmm
#include <gtkmm/enums.h>
#include <gtkmm/treepath.h>
#include <gdkmm/rectangle.h>

// project
#include "previewgallery.hh"

// PreviewGallery implementation
PreviewGallery::PreviewGallery(PreviewSize size)
:
    size(size)
{
    // configure label for when the gallery is empty
    no_items_label.set_markup("<span weight=\"bold\" size=\"xx-large\">No Items</span>");
    no_items_label.set_halign(Gtk::Align::CENTER);
    no_items_label.set_valign(Gtk::Align::CENTER);
    no_items_label.set_expand(true);

    // setup ListStore with this IconView
    store = Gtk::ListStore::create(icon_model),
    icon_view.set_model(store);
    icon_view.set_text_column(icon_model.name);
    icon_view.set_pixbuf_column(icon_model.pixbuf);

    // configure appearance
    icon_view.set_item_width((int)size);
    icon_view.set_selection_mode(Gtk::SelectionMode::SINGLE);

    // the initial child is the label
    // as there are no items loaded by default
    set_child(no_items_label);
    label_is_child = true;

    // signal handling
    icon_view.signal_item_activated().connect(
            sigc::mem_fun(*this, &PreviewGallery::on_item_activate));
    icon_view.signal_selection_changed().connect(
            sigc::mem_fun(*this, &PreviewGallery::on_selection_changed));

    // set up right click controller
    click = Gtk::GestureClick::create();
    click->set_button(GDK_BUTTON_SECONDARY);
    click->signal_pressed().connect(sigc::mem_fun(*this, &PreviewGallery::on_right_click));
    add_controller(click);

    // configure scrolled window (self)
    set_propagate_natural_width(true);
    set_propagate_natural_height(true);
    set_expand(true);
}

void PreviewGallery::set_content(const std::vector<Glib::ustring> &file_paths) {
    // clear ListStore
    store->clear();

    // if content is empty, show the no items label
    if (file_paths.size() == 0) {
        if (!label_is_child) {
            set_child(no_items_label);
            label_is_child = true;
        }
        return;
    }

    // add items
    for (size_t idx = 0; idx < file_paths.size(); idx++) {
        bool result = add_item(idx, file_paths.at(idx));
        if (!result) {
            private_signal_failed_to_open.emit(idx);
            break;
        }
    }

    // now that contents are available
    // the iconview can be set as child
    if (label_is_child) {
        set_child(icon_view);
        label_is_child = false;
    }
}

void PreviewGallery::set_size(PreviewSize size) {
    this->size = size;
}

PreviewGallery::PreviewSize PreviewGallery::get_size() const {
    return this->size;
}

Glib::ustring PreviewGallery::get_file_path(const Gtk::TreePath &tpath) const {
    return (*(store->get_iter(tpath)))[icon_model.file_path];
}

Glib::SignalProxy<void (const Gtk::TreeModel::Path &)> PreviewGallery::signal_item_activated() {
    return icon_view.signal_item_activated();
}

sigc::signal<void (size_t)> PreviewGallery::signal_item_chosen() {
    return private_signal_item_chosen;
}

sigc::signal<void (size_t)> PreviewGallery::signal_item_selected() {
    return private_signal_item_selected;
}

sigc::signal<void (size_t)> PreviewGallery::signal_failed_to_open() {
    return private_signal_failed_to_open;
}

sigc::signal<void (const Glib::ustring &)> PreviewGallery::signal_edit() {
    return private_edit;
}

bool PreviewGallery::add_item(size_t id, const Glib::ustring &file_path) {
    // get pixbuf from file path
    Glib::RefPtr<Gdk::Pixbuf> pbuf;

    try {
        pbuf = Gdk::Pixbuf::create_from_file(file_path);
    }
    catch (...) {
        return false;
    }

    // calculate scale proportion based on the longer dimension
    int image_size = pbuf->get_width() > pbuf->get_height() ? pbuf->get_width() : pbuf->get_height();
    double prop = (double)((int)size) / (double)image_size;

    // scale image by proportion
    pbuf = pbuf->scale_simple(
            std::round(pbuf->get_width() * prop),
            std::round(pbuf->get_height() * prop),
            Gdk::InterpType::BILINEAR);

    // add image to store
    auto row = *(store->append());
    row[icon_model.id] = id;
    row[icon_model.file_path] = file_path;
    row[icon_model.name] = file_path.substr(file_path.find_last_of("/") + 1);
    row[icon_model.pixbuf] = pbuf;

    return true;
}

void PreviewGallery::on_item_activate(const Gtk::TreePath &tpath) {
    Gtk::TreeRow row = (*(store->get_iter(tpath)));
    private_signal_item_chosen.emit(row[icon_model.id]);
}

void PreviewGallery::on_selection_changed() {
    // since this widget is configured to use single item selection
    // only one selected item is possible at one time
    std::vector<Gtk::TreePath> tpath_vec = icon_view.get_selected_items();
    if (tpath_vec.size() > 0) {
        Gtk::TreePath tpath = tpath_vec.at(0);
        Gtk::TreeRow row = (*(store->get_iter(tpath)));
        private_signal_item_selected.emit(row[icon_model.id]);
    }
}

// on right click, select the item and show a popup menu
// allowing for the editing of tags
void PreviewGallery::on_right_click(int n_times, double x, double y) {
    auto tree_path = icon_view.get_path_at_pos(std::round(x), std::round(y));

    if (tree_path) {
        // show GtkPopup for editing item
        Gdk::Rectangle rect;
        if (icon_view.get_cell_rect(tree_path, rect)) {
            right_click_menu = std::make_unique<PreviewGallery::RightClickMenu>(*this);
            right_click_menu->set_pointing_to(rect);

            // set data for popup
            Gtk::TreeRow row = (*(store->get_iter(tree_path)));
            right_click_menu->set_file_path(row.get_value(icon_model.file_path));

            right_click_menu->show();
        }
        icon_view.select_path(tree_path);
    }
}

void PreviewGallery::on_edit_clicked() {
    right_click_menu->hide();
    private_edit.emit(right_click_menu->get_file_path());
}

// RightClickMenu implementation
PreviewGallery::RightClickMenu::RightClickMenu(PreviewGallery &parent) {
    // wdiget setup
    btn_edit_item.set_label("Edit");
    btn_edit_item.set_has_frame(false);
    btn_edit_item.signal_clicked().connect(
            sigc::mem_fun(parent, &PreviewGallery::on_edit_clicked));

    // box setup
    box.set_orientation(Gtk::Orientation::VERTICAL);
    box.set_margin(10);
    box.set_spacing(15);
    box.append(btn_edit_item);

    set_child(box);
    set_parent(parent);
}

PreviewGallery::RightClickMenu::~RightClickMenu() {
    unparent();
}

const Glib::ustring &PreviewGallery::RightClickMenu::get_file_path() const {
    return file_path;
}

void PreviewGallery::RightClickMenu::set_file_path(const Glib::ustring &file_path) {
    this->file_path = file_path;
}

Glib::SignalProxy<void ()> PreviewGallery::RightClickMenu::signal_edit() {
    return btn_edit_item.signal_clicked();
}

