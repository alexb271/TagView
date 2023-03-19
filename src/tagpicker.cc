// project
#include "tagpicker.hh"

TagPicker::TagPicker()
:
    TagPickerBase(true),
    tags_exclude(ItemList::Type::INSIDE),
    tags_current_item(ItemList::Type::OUTSIDE_WITH_EXCLUDE)
{
    // filter setup
    filter_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    filter_box.set_spacing(15);
    lbl_filter.set_markup("<span weight=\"bold\">Filter:</span>");
    chk_filter_or.set_label(" Or");
    chk_filter_and.set_label(" And");
    chk_filter_and.set_group(chk_filter_or);
    chk_filter_or.set_active(true);
    filter_box.append(lbl_filter);
    filter_box.append(chk_filter_or);
    filter_box.append(chk_filter_and);

    // label setup
    lbl_tags.set_markup("<span weight=\"bold\" size=\"large\">Include</span>");
    lbl_tags_exclude.set_markup("<span weight=\"bold\" size=\"large\">Exclude</span>");
    lbl_tags_current_item.set_markup("<span weight=\"bold\" size=\"large\">Image tags</span>");

    // button setup
    btn_reload_default_exclude.set_icon_name("view-refresh-symbolic");
    btn_reload_default_exclude.set_tooltip_text("Reload default");
    btn_reload_default_exclude.set_halign(Gtk::Align::CENTER);
    btn_reload_default_exclude.set_has_frame(false);

    prepend(filter_box);
    append(sep1);
    append(lbl_tags_exclude);
    append(btn_reload_default_exclude);
    append(tags_exclude);
    append(sep2);
    append(lbl_tags_current_item);
    append(tags_current_item);

    set_allow_create_new_tag(false);

    // connect signals
    chk_filter_or.signal_toggled().connect(
            sigc::mem_fun(*this, &TagPicker::on_filter_toggled));
    chk_filter_and.signal_toggled().connect(
            sigc::mem_fun(*this, &TagPicker::on_filter_toggled));
    tags.signal_contents_changed().connect(
            sigc::mem_fun(*this, &TagPicker::on_content_changed));
    tags_exclude.signal_contents_changed().connect(
            sigc::mem_fun(*this, &TagPicker::on_content_changed));
    tags_current_item.signal_add().connect(
            sigc::mem_fun(*this, &TagPicker::on_signal_add));
    tags_current_item.signal_exclude().connect(
            sigc::mem_fun(*this, &TagPicker::on_signal_exclude));
}

TagQuery TagPicker::get_current_query() const {
    return TagQuery(tags.get_content(), tags_exclude.get_content());
}

void TagPicker::add_excluded_tag(const Glib::ustring &tag) {
    tags_exclude.append(tag);
}

void TagPicker::set_current_item_tags(const std::set<Glib::ustring> &tags) {
    tags_current_item.clear();
    for (const Glib::ustring &tag : tags) {
        tags_current_item.append(tag);
    }
}

void TagPicker::clear_excluded_tags() {
    tags_exclude.clear();
}

void TagPicker::clear_current_item_tags() {
    tags_current_item.clear();
}

sigc::signal<void (TagDb::QueryType)> TagPicker::signal_filter_toggled() {
    return private_filter_toggled;
}

sigc::signal<void (TagQuery)> TagPicker::signal_query_changed() {
    return private_query_changed;
}

Glib::SignalProxy<void ()> TagPicker::signal_reload_default_exclude_required() {
    return btn_reload_default_exclude.signal_clicked();
}

void TagPicker::on_filter_toggled() {
    if (chk_filter_or.get_active()) {
        private_filter_toggled.emit(TagDb::QueryType::OR);
    }
    else if (chk_filter_and.get_active()) {
        private_filter_toggled.emit(TagDb::QueryType::AND);
    }
}

void TagPicker::on_signal_add(const Glib::ustring &tag) {
    if (tags.get_content().count(tag) == 1) { return; }
    tags.append(tag);
    private_query_changed.emit(get_current_query());
}

void TagPicker::on_signal_exclude(const Glib::ustring &tag) {
    tags_exclude.append(tag);
    private_query_changed.emit(get_current_query());
}

void TagPicker::on_content_changed(const std::set<Glib::ustring> &tags) {
    private_query_changed.emit(get_current_query());
}
