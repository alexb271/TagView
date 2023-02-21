// project
#include "tagutils.hh"

// TagQuery implementation
TagQuery::TagQuery(std::set<Glib::ustring> tags_to_include,
                   std::set<Glib::ustring> tags_to_exclude)
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

// ItemList implementation
ItemList::ItemList(ItemList::Type type)
:
    type(type)
{
    // box setup
    box.set_orientation(Gtk::Orientation::VERTICAL);

    // scrolled window setup
    set_propagate_natural_height(true);
    set_propagate_natural_width(true);
    set_child(box);
}

void ItemList::append(const Glib::ustring &text) {
    // check for zero length string
    if (text.size() == 1) { return; }

    // check if the database already contains this item
    if (items.count(text) != 0) { return; }

    // register item in string based database
    items.insert(text);

    // create the widget representing the tag and connect its signal
    if (type == ItemList::Type::INSIDE) {
        std::unique_ptr<ItemInQuery> tag_widget = std::make_unique<ItemInQuery>(text);
        tag_widget->signal_remove().connect(
                sigc::bind(sigc::mem_fun(*this, &ItemList::on_signal_remove), text));

        box.append(*tag_widget);
        widgets.push_back(std::move(tag_widget));
    }

    else if (type == ItemList::Type::OUTSIDE) {
        std::unique_ptr<ItemOutsideQuery> tag_widget = std::make_unique<ItemOutsideQuery>(text);
        tag_widget->signal_add().connect(
                sigc::bind(sigc::mem_fun(*this, &ItemList::on_signal_add), text));

        box.append(*tag_widget);
        widgets.push_back(std::move(tag_widget));
    }

    else if (type == ItemList::Type::OUTSIDE_WITH_EXCLUDE) {
        std::unique_ptr<ItemOutsideQuery> tag_widget = std::make_unique<ItemOutsideQuery>(text, true);
        tag_widget->signal_add().connect(
                sigc::bind(sigc::mem_fun(*this, &ItemList::on_signal_add), text));
        tag_widget->signal_exclude().connect(
                sigc::bind(sigc::mem_fun(*this, &ItemList::on_signal_exclude), text));

        box.append(*tag_widget);
        widgets.push_back(std::move(tag_widget));
    }
}

void ItemList::append_and_notify(const Glib::ustring &text) {
    append(text);

    // signal changes
    private_contents_changed.emit(items);
}
void ItemList::clear() {
    items.clear();
    for (std::unique_ptr<Gtk::Widget> &item : widgets) {
        box.remove(*item);
    }
    widgets.clear();
}

const std::set<Glib::ustring> &ItemList::get_content() const {
    return items;
}

bool ItemList::contains(const Glib::ustring &item) const {
    return items.count(item);
}

size_t ItemList::size() const {
    return widgets.size();
}

sigc::signal<void (const std::set<Glib::ustring> &)> ItemList::signal_contents_changed() {
    return private_contents_changed;
}

sigc::signal<void (const Glib::ustring &)> ItemList::signal_add() {
    return private_add;
}

sigc::signal<void (const Glib::ustring &)> ItemList::signal_exclude() {
    return private_exclude;
}

void ItemList::on_signal_remove(const Glib::ustring &text) {
    // find tag widget
    size_t idx;
    for (idx = 0; idx < widgets.size(); idx++) {
        if (static_cast<ItemInQuery *>(widgets[idx].get())->get_text() == text ) {
            break;
        }
    }

    // remove the widget from the box
    box.remove(*(widgets[idx].get()));

    // also remove it from the owning vector
    widgets.erase(widgets.begin() + idx);

    // remove the item's text from the text based database
    items.erase(text);

    private_contents_changed.emit(items);
}

void ItemList::on_signal_add(const Glib::ustring &text) {
    private_add.emit(text);
}

void ItemList::on_signal_exclude(const Glib::ustring &text) {
    private_exclude.emit(text);
}

// TagPickerBase implementation
TagPickerBase::TagPickerBase(bool label_below_entry)
:
    tags(ItemList::Type::INSIDE),
    allow_create_new_tag(true)
{
    // setup smart pointers with data
    completer = Gtk::EntryCompletion::create();

    // configure completion
    entry.set_completion(completer);
    completer->set_text_column(list_model.tag);
    completer->set_match_func(sigc::mem_fun(*this, &TagPickerBase::on_completion_match));
    completer->signal_match_selected().connect(
            sigc::mem_fun(*this, &TagPickerBase::on_match_selected), false);

    // the entry's activate signal is not implemented in glibmm 4.6, use C API instead
    g_signal_connect(entry.gobj(), "activate", G_CALLBACK(tag_editor_on_entry_activate), this);

    // box setup (self)
    if (label_below_entry) {
        append(entry);
        append(lbl_tags);
    }
    else {
        append(lbl_tags);
        append(entry);
    }
    append(tags);

    set_orientation(Gtk::Orientation::VERTICAL);
    set_spacing(20);
    set_hexpand(false);
}

const std::set<Glib::ustring> &TagPickerBase::get_content() const {
    return tags.get_content();
}

void TagPickerBase::set_completer_model(Glib::RefPtr<Gtk::ListStore> completer_list) {
    completer->set_model(completer_list);
}

void TagPickerBase::set_allow_create_new_tag(bool allow_create_new_tag) {
    this->allow_create_new_tag = allow_create_new_tag;
}

bool TagPickerBase::get_allow_create_new_tag() const {
    return allow_create_new_tag;
}

void TagPickerBase::set_label_markup(const Glib::ustring &markup) {
    lbl_tags.set_markup(markup);
}

void TagPickerBase::clear() {
    tags.clear();
    clear_text();
}

void TagPickerBase::clear_text() {
    entry.get_buffer()->delete_text(0, -1);
}

void TagPickerBase::add_tag(const Glib::ustring &tag) {
    tags.append(tag);
}

bool TagPickerBase::contains(const Glib::ustring &item) const {
    return tags.contains(item);
}

size_t TagPickerBase::size() const {
    return tags.size();
}

void tag_editor_on_entry_activate(GtkEntry *c_entry, gpointer data) {
    // passed data is the TagPicker instance
    // that the entry is part of
    TagPickerBase *tag_picker = (TagPickerBase*)data;

    // get a gtkmm C++ wrapper around c_entry
    Gtk::Entry *entry = Glib::wrap(c_entry);

    // get the inserted text
    Glib::ustring text = entry->get_text();

    // exit if string contains only whitespaces
    // or if it contains a comma
    if (text.find_first_not_of("\t \n") == Glib::ustring::npos ||
        text.find_first_of(",") != Glib::ustring::npos) {
        return;
    }

    // strip whitespaces from start and end
    text = text.substr(text.find_first_not_of("\t \n"));
    if (text.find_last_not_of("\t \n") < text.size() - 1) {
        text = text.substr(0, text.find_last_not_of("\t \n") + 1);
    }

    // if entering new tags is allowed, simply add it
    if (tag_picker->allow_create_new_tag) {
        tag_picker->add_tag_notify(text);
        entry->get_buffer()->delete_text(0, -1);
    }
    // else search tag database for a match on text first
    else {
        for (auto item : tag_picker->completer->get_model()->children()) {
            if (text == item.get_value(tag_picker->list_model.tag)) {
                // in case of a match add it to included tags
                tag_picker->add_tag_notify(text);

                //clear the entry's text buffer
                entry->get_buffer()->delete_text(0, -1);
            }
        }
    }
}

sigc::signal<void (const std::set<Glib::ustring> &)> TagPickerBase::signal_contents_changed() {
    return tags.signal_contents_changed();
}

void TagPickerBase::add_tag_notify(const Glib::ustring &tag) {
    tags.append_and_notify(tag);
}

bool TagPickerBase::on_match_selected(const Gtk::TreeModel::iterator &iter) {
    // get the tag
    Glib::ustring tag = (*iter)[list_model.tag];

    // add the tag to the string based vector and the display box
    add_tag_notify(tag);

    // clear the entry's text buffer
    entry.get_buffer()->delete_text(0, -1);

    // return focus to the entry
    entry.grab_focus();

    // the signal is handled, no need for the default handler
    return true;
}

bool TagPickerBase::on_completion_match(const Glib::ustring &key,
                                        const Gtk::TreeModel::const_iterator &iter) {
    Glib::ustring filter_str = iter->get_value(list_model.tag);

    if (filter_str.lowercase().find(key.lowercase()) != Glib::ustring::npos) {
        return true;
    }

    return false;
}
