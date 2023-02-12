// project
#include "tag.hh"

// TagQuery implementation
TagQuery::TagQuery(std::vector<Glib::ustring> tags_to_include,
                   std::vector<Glib::ustring> tags_to_exclude)
:
    tags_include(tags_to_include),
    tags_exclude(tags_to_exclude)
{}


// TagInQuery implementation
TagInQuery::TagInQuery(const Glib::ustring &tag_name) {
    name.set_text(tag_name);

    remove.set_has_frame(false);
    remove.set_icon_name("list-remove-symbolic");
    remove.set_hexpand(true);
    remove.set_halign(Gtk::Align::END);

    set_orientation(Gtk::Orientation::HORIZONTAL);
    append(name);
    append(remove);
}

Glib::ustring TagInQuery::tag() {
    return name.get_text();
}

Glib::SignalProxy<void()> TagInQuery::signal_remove() {
    return remove.signal_clicked();
}

// TagOutsideQuery implementation
TagOutsideQuery::TagOutsideQuery(const Glib::ustring &tag_name) {
    name.set_text(tag_name);

    add.set_has_frame(false);
    add.set_icon_name("list-add-symbolic");
    add.set_hexpand(true);
    add.set_halign(Gtk::Align::END);

    exclude.set_has_frame(false);
    exclude.set_icon_name("action-unavailable-symbolic");
    exclude.set_halign(Gtk::Align::END);

    set_orientation(Gtk::Orientation::HORIZONTAL);
    append(name);
    append(add);
    append(exclude);
}

Glib::ustring TagOutsideQuery::tag() {
    return name.get_text();
}

Glib::SignalProxy<void()> TagOutsideQuery::signal_add() {
    return add.signal_clicked();
}
Glib::SignalProxy<void()> TagOutsideQuery::signal_exclude() {
    return exclude.signal_clicked();
}

