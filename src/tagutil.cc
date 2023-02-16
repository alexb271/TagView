// standard library
#include <algorithm>

// project
#include "gtkmm/object.h"
#include "tagutil.hh"

// TagQuery implementation
TagQuery::TagQuery(std::vector<Glib::ustring> tags_to_include,
                   std::vector<Glib::ustring> tags_to_exclude)
:
    tags_include(tags_to_include),
    tags_exclude(tags_to_exclude)
{}


// TagInQuery implementation
ItemInQuery::ItemInQuery(const Glib::ustring &tag_name) {
    name.set_text(tag_name);

    remove.set_has_frame(false);
    remove.set_icon_name("list-remove-symbolic");
    remove.set_hexpand(true);
    remove.set_halign(Gtk::Align::END);

    set_orientation(Gtk::Orientation::HORIZONTAL);
    append(name);
    append(remove);
}

void ItemInQuery::set_tag(const Glib::ustring &tag) {
    name.set_text(tag);
}

Glib::ustring ItemInQuery::get_text() const {
    return name.get_text();
}

Glib::SignalProxy<void()> ItemInQuery::signal_remove() {
    return remove.signal_clicked();
}

// TagOutsideQuery implementation
ItemOutsideQuery::ItemOutsideQuery(bool exclude_button) {
    add.set_has_frame(false);
    add.set_icon_name("list-add-symbolic");
    add.set_hexpand(true);
    add.set_halign(Gtk::Align::END);

    if (exclude_button) {
        exclude.set_has_frame(false);
        exclude.set_icon_name("action-unavailable-symbolic");
        exclude.set_halign(Gtk::Align::END);
    }

    set_orientation(Gtk::Orientation::HORIZONTAL);
    append(name);
    append(add);
    if (exclude_button) {
        append(exclude);
    }
}

ItemOutsideQuery::ItemOutsideQuery(const Glib::ustring &tag_name, bool exclude_button) {
    name.set_text(tag_name);

    add.set_has_frame(false);
    add.set_icon_name("list-add-symbolic");
    add.set_hexpand(true);
    add.set_halign(Gtk::Align::END);

    if (exclude_button) {
        exclude.set_has_frame(false);
        exclude.set_icon_name("action-unavailable-symbolic");
        exclude.set_halign(Gtk::Align::END);
    }

    set_orientation(Gtk::Orientation::HORIZONTAL);
    append(name);
    append(add);
    if (exclude_button) {
        append(exclude);
    }
}

void ItemOutsideQuery::set_tag(const Glib::ustring &tag) {
    name.set_text(tag);
}

Glib::ustring ItemOutsideQuery::get_text() const {
    return name.get_text();
}

Glib::SignalProxy<void()> ItemOutsideQuery::signal_add() {
    return add.signal_clicked();
}
Glib::SignalProxy<void()> ItemOutsideQuery::signal_exclude() {
    return exclude.signal_clicked();
}

// ItemInQueryList implementation

ItemInQueryList::ItemInQueryList() {
    // box setup
    widgets.set_orientation(Gtk::Orientation::VERTICAL);

    // scrolled window setup
    set_propagate_natural_height(true);
    set_propagate_natural_width(true);
    set_child(widgets);
}

void ItemInQueryList::append(const Glib::ustring &text) {
    // check if the database already contains this item
    if (items.count(text) != 0) { return; }

    // register item in string based database
    items.insert(text);

    // create the widget representing the tag and connect its signal
    ItemInQuery *tag_widget = Gtk::make_managed<ItemInQuery>(text);
    tag_widget->signal_remove().connect(
            sigc::bind(sigc::mem_fun(*this, &ItemInQueryList::on_signal_remove), tag_widget));
    widgets.append(*tag_widget);

    // signal changes
    private_contents_changed.emit(items);
}

void ItemInQueryList::clear() {
    Gtk::Widget *item = widgets.get_first_child();
    while(item) {
        widgets.remove(*item);
        item = widgets.get_first_child();
    }
}

const std::set<Glib::ustring> &ItemInQueryList::get_content() const {
    return items;
}

sigc::signal<void (const std::set<Glib::ustring> &)> ItemInQueryList::signal_contents_changed() {
    return private_contents_changed;
}

void ItemInQueryList::on_signal_remove(ItemInQuery *item) {
    // remove the item's text from the text based database
    items.erase(item->get_text());

    // also remove the associated widget
    widgets.remove(*item);

    // free the memory of the widget, since
    // it is no longer managed by the box
    // and it's not needed anymore
    delete item;

    private_contents_changed.emit(items);
}

// TagPickerBase implementation
TagPickerBase::TagPickerBase(Glib::ustring title)
    : allow_create_new_tag(true)
{
    // setup smart pointers with data
    completer = Gtk::EntryCompletion::create();
    completer_list = Gtk::ListStore::create(list_model);

    // configure completion
    entry.set_margin(15);
    entry.set_completion(completer);
    completer->set_model(completer_list);
    completer->set_text_column(list_model.tag);
    completer->set_match_func(sigc::mem_fun(*this, &TagPickerBase::on_completion_match));
    completer->signal_match_selected().connect(
            sigc::mem_fun(*this, &TagPickerBase::on_match_selected), false);

    // the entry's activate signal is not implemented in glibmm 4.6, use C API instead
    g_signal_connect(entry.gobj(), "activate", G_CALLBACK(tag_editor_on_entry_activate), this);

    // label setup
    lbl_title.set_markup("<span weight=\"bold\" size=\"large\">" + title + "</span>");
    lbl_title.set_margin_top(15);
    lbl_title.set_margin_bottom(15);

    // box setup (self)
    append(entry);
    append(lbl_title);
    append(tags);

    set_orientation(Gtk::Orientation::VERTICAL);
    set_hexpand(false);
}

const std::set<Glib::ustring> &TagPickerBase::get_content() const {
    return tags.get_content();
}

void TagPickerBase::set_completer_data(const std::set<Glib::ustring> &completer_tags) {
    completer_list->clear();
    tags_all.clear();
    for (const Glib::ustring &tag : completer_tags) {
        auto row = *(completer_list->append());
        row[list_model.tag] = tag;
        tags_all.push_back(tag);
    }
}

void TagPickerBase::set_allow_create_new_tag(bool allow_create_new_tag) {
    this->allow_create_new_tag = allow_create_new_tag;
}

bool TagPickerBase::get_allow_create_new_tag() const {
    return allow_create_new_tag;
}

void tag_editor_on_entry_activate(GtkEntry *c_entry, gpointer data) {
    // passed data is the TagEditor instance
    // that the entry is part of
    TagPickerBase *tag_editor = (TagPickerBase*)data;

    // get a gtkmm C++ wrapper around c_entry
    Gtk::Entry *entry = Glib::wrap(c_entry);

    // get the inserted text
    Glib::ustring text = entry->get_text();

    // if entering new tags is allowed, simply add it
    if (tag_editor->allow_create_new_tag) {
        tag_editor->add_tag(text);
        entry->get_buffer()->delete_text(0, -1);
    }
    // else search tag database for a match on text first
    else {
        for (Glib::ustring tag : tag_editor->tags_all) {
            if (text == tag) {
                // in case of a match add it to included tags
                tag_editor->add_tag(text);

                //clear the entry's text buffer
                entry->get_buffer()->delete_text(0, -1);
            }
        }
    }
}

void TagPickerBase::add_tag(const Glib::ustring &tag) {
    tags.append(tag);
}

bool TagPickerBase::on_match_selected(const Gtk::TreeModel::iterator &iter) {
    // get the tag
    Glib::ustring tag = (*iter)[list_model.tag];

    // add the tag to the string based vector and the display box
    add_tag(tag);

    // clear the entry's text buffer
    entry.get_buffer()->delete_text(0, -1);

    // return focus to the entry
    entry.grab_focus();

    // the signal is handled, no need for the default handler
    return true;
}

bool TagPickerBase::on_completion_match(const Glib::ustring &key,
                                    const Gtk::TreeModel::const_iterator &iter) {
    const auto row = *iter;
    Glib::ustring filter_str = row[list_model.tag];

    if (filter_str.lowercase().find(key.lowercase()) != Glib::ustring::npos) {
        return true;
    }

    return false;
}
