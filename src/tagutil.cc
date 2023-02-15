// standard library
#include <algorithm>

// project
#include "tagutil.hh"

// TagQuery implementation
TagQuery::TagQuery(std::vector<Glib::ustring> tags_to_include,
                   std::vector<Glib::ustring> tags_to_exclude)
:
    tags_include(tags_to_include),
    tags_exclude(tags_to_exclude)
{}


// TagInQuery implementation
TagInQuery::TagInQuery() {
    remove.set_has_frame(false);
    remove.set_icon_name("list-remove-symbolic");
    remove.set_hexpand(true);
    remove.set_halign(Gtk::Align::END);

    set_orientation(Gtk::Orientation::HORIZONTAL);
    append(name);
    append(remove);
}

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

void TagInQuery::set_tag(const Glib::ustring &tag) {
    name.set_text(tag);
}

Glib::ustring TagInQuery::get_tag() {
    return name.get_text();
}

Glib::SignalProxy<void()> TagInQuery::signal_remove() {
    return remove.signal_clicked();
}

// TagOutsideQuery implementation
TagOutsideQuery::TagOutsideQuery(bool exclude_button) {
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

TagOutsideQuery::TagOutsideQuery(const Glib::ustring &tag_name, bool exclude_button) {
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

void TagOutsideQuery::set_tag(const Glib::ustring &tag) {
    name.set_text(tag);
}

Glib::ustring TagOutsideQuery::get_tag() {
    return name.get_text();
}

Glib::SignalProxy<void()> TagOutsideQuery::signal_add() {
    return add.signal_clicked();
}
Glib::SignalProxy<void()> TagOutsideQuery::signal_exclude() {
    return exclude.signal_clicked();
}

// TagEditor implementation
TagEditor::TagEditor(Glib::ustring title)
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
    completer->set_match_func(sigc::mem_fun(*this, &TagEditor::on_completion_match));
    completer->signal_match_selected().connect(
            sigc::mem_fun(*this, &TagEditor::on_match_selected), false);

    // the entry's activate signal is not implemented in glibmm 4.6, use C API instead
    g_signal_connect(entry.gobj(), "activate", G_CALLBACK(tag_editor_on_entry_activate), this);

    // label setup
    lbl_title.set_markup("<span weight=\"bold\" size=\"large\">" + title + "</span>");
    lbl_title.set_margin_top(15);
    lbl_title.set_margin_bottom(15);

    // box setup (self)
    append(entry);
    append(lbl_title);
    append(tag_widgets);

    set_orientation(Gtk::Orientation::VERTICAL);
    set_hexpand(false);
}

void TagEditor::set_completer_data(const std::set<Glib::ustring> &completer_tags) {
    completer_list->clear();
    tags_all.clear();
    for (const Glib::ustring &tag : completer_tags) {
        auto row = *(completer_list->append());
        row[list_model.tag] = tag;
        tags_all.push_back(tag);
    }
}

void TagEditor::set_allow_create_new_tag(bool allow_create_new_tag) {
    this->allow_create_new_tag = allow_create_new_tag;
}

bool TagEditor::get_allow_create_new_tag() {
    return allow_create_new_tag;
}

void tag_editor_on_entry_activate(GtkEntry *c_entry, gpointer data) {
    // passed data is the TagEditor instance
    // that the entry is part of
    TagEditor *tag_editor = (TagEditor*)data;

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

sigc::signal<void (const std::vector<Glib::ustring> &)> TagEditor::signal_contents_changed() {
    return private_contents_changed;
}

void TagEditor::add_tag(const Glib::ustring &tag) {

    // check if the database already contains this tag
    // check both include and exclude databases
    if (std::find(tags_picked.begin(), tags_picked.end(), tag) != tags_picked.end()) {
        return;
    }

    // register tag in string based database
    tags_picked.push_back(tag);

    // create the widget representing the tag and connect its signal
    tag_widgets.create().set_tag(tag);
    tag_widgets[tag_widgets.size() - 1].signal_remove()
        .connect(sigc::bind(sigc::mem_fun(*this, &TagEditor::on_signal_remove), tag));

    private_contents_changed.emit(tags_picked);
}

void TagEditor::on_signal_remove(const Glib::ustring &tag) {
    // find index of tag in database
    // and erase tag from database
    size_t idx;
    for(idx = 0; idx < tags_picked.size(); idx++) {
        if (tags_picked[idx] == tag) {
            tags_picked.erase(tags_picked.begin() + idx);
            break;
        }
    }

    // also remove the associated widget
    tag_widgets.remove_at(idx);

    private_contents_changed.emit(tags_picked);
}

bool TagEditor::on_match_selected(const Gtk::TreeModel::iterator &iter) {
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

bool TagEditor::on_completion_match(const Glib::ustring &key,
                                    const Gtk::TreeModel::const_iterator &iter) {
    const auto row = *iter;
    Glib::ustring filter_str = row[list_model.tag];

    if (filter_str.lowercase().find(key.lowercase()) != Glib::ustring::npos) {
        return true;
    }

    return false;
}
