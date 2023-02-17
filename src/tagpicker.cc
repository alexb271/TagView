// project
#include "tagpicker.hh"

TagPicker::TagPicker()
:
    tags_exclude(ItemList::Type::INSIDE),
    tags_current_item(ItemList::Type::OUTSIDE_WITH_EXCLUDE)
{
    // connect signals
    tags.signal_contents_changed().connect(
            sigc::mem_fun(*this, &TagPicker::on_content_changed));
    tags_exclude.signal_contents_changed().connect(
            sigc::mem_fun(*this, &TagPicker::on_content_changed));
    tags_current_item.signal_add().connect(
            sigc::mem_fun(*this, &TagPicker::on_signal_add));
    tags_current_item.signal_exclude().connect(
            sigc::mem_fun(*this, &TagPicker::on_signal_exclude));

    // label setup
    lbl_tags.set_markup("<span weight=\"bold\" size=\"large\">Include</span>");
    lbl_tags_exclude.set_markup("<span weight=\"bold\" size=\"large\">Exclude</span>");
    lbl_tags_current_item.set_markup("<span weight=\"bold\" size=\"large\">Image tags</span>");

    append(sep1);
    append(lbl_tags_exclude);
    append(tags_exclude);
    append(sep2);
    append(lbl_tags_current_item);
    append(tags_current_item);

    set_allow_create_new_tag(false);
}
void TagPicker::set_current_item_tags(const std::set<Glib::ustring> &tags) {
    tags_current_item.clear();
    for (const Glib::ustring &tag : tags) {
        tags_current_item.append(tag);
    }
}

void TagPicker::clear_current_item_tags() {
    tags_current_item.clear();
}

TagQuery TagPicker::get_current_query() const {
    return TagQuery(tags.get_content(), tags_exclude.get_content());
}

sigc::signal<void (TagQuery)> TagPicker::signal_query_changed() {
    return private_query_changed;
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
