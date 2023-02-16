// standard library
#include <algorithm>

// gtkmm
#include <gtkmm/enums.h>
#include <gtkmm/entrycompletion.h>
#include <gtkmm/liststore.h>
#include <sigc++/functors/mem_fun.h>

// project
#include "tagpicker.hh"

// signal handler for C API
void on_entry_activate(GtkEntry *c_entry, gpointer data) {
    // passed data is the TagPicker instance
    // that the entry is part of
    TagPicker *tag_picker = (TagPicker*)data;

    // get a gtkmm C++ wrapper around c_entry
    Gtk::Entry *entry = Glib::wrap(c_entry);

    // get the inserted text
    Glib::ustring text = entry->get_text();

    // search tag database for text
    for (Glib::ustring tag : tag_picker->tags_all) {
        if (text == tag) {
            // in case of a match add it to included tags
            tag_picker->on_signal_add(text);

            //clear the entry's text buffer
            entry->get_buffer()->delete_text(0, -1);
        }
    }
}

TagPicker::TagPicker() {
    // setup smart pointers with data
    completer = Gtk::EntryCompletion::create();
    completer_list = Gtk::ListStore::create(list_model);

    // configure completion
    entry.set_margin(15);
    entry.set_completion(completer);
    completer->set_model(completer_list);
    completer->set_text_column(list_model.tag);
    completer->set_match_func(sigc::mem_fun(*this, &TagPicker::on_completion_match));
    completer->signal_match_selected().connect(
            sigc::mem_fun(*this, &TagPicker::on_match_selected), false);

    // the entry's activate signal is not implemented in glibmm 4.6, use C API instead
    g_signal_connect(entry.gobj(), "activate", G_CALLBACK(on_entry_activate), this);

    // configure widgets
    include_label.set_markup("<span weight=\"bold\" size=\"large\">Include</span>");
    include_label.set_margin_top(15);
    include_label.set_margin_bottom(15);
    exclude_label.set_markup("<span weight=\"bold\" size=\"large\">Exclude</span>");
    exclude_label.set_margin_top(15);
    exclude_label.set_margin_bottom(15);
    outside_label.set_markup("<span weight=\"bold\" size=\"large\">Image tags</span>");
    outside_label.set_margin_top(15);
    outside_label.set_margin_bottom(15);

    separator_1.set_margin_top(15);
    separator_1.set_margin_bottom(15);
    separator_2.set_margin_top(15);
    separator_2.set_margin_bottom(15);

    include_box.set_orientation(Gtk::Orientation::VERTICAL);
    exclude_box.set_orientation(Gtk::Orientation::VERTICAL);
    outside_box.set_orientation(Gtk::Orientation::VERTICAL);

    // configure section scrolled windows
    include_scw.set_child(include_box);
    include_scw.set_propagate_natural_height(true);
    include_scw.set_propagate_natural_width(true);
    exclude_scw.set_child(exclude_box);
    exclude_scw.set_propagate_natural_height(true);
    exclude_scw.set_propagate_natural_width(true);
    outside_scw.set_child(outside_box);
    outside_scw.set_propagate_natural_height(true);
    outside_scw.set_propagate_natural_width(true);

    // append widgets to box
    append(entry);

    append(include_label);
    append(include_scw);

    append(separator_1);

    append(exclude_label);
    append(exclude_scw);

    append(separator_2);

    append(outside_label);
    append(outside_scw);

    // configure box (self)
    set_orientation(Gtk::Orientation::VERTICAL);
    set_hexpand(false);
}

void TagPicker::set_completer_data(const std::set<Glib::ustring> &tags) {
    tags_all.clear();
    completer_list->clear();
    for (const Glib::ustring &tag : tags) {
        auto row = *(completer_list->append());
        row[list_model.tag] = tag;
        tags_all.push_back(tag);
    }
}

void TagPicker::set_current_item_tags(const std::set<Glib::ustring> &tags) {
    // delete all previous widgets
    for (auto &widget : vec_outside_widgets) {
        outside_box.remove(widget);
    }
    vec_outside_widgets.clear();

    // add all tags as new widgets
    for (const Glib::ustring &tag : tags) {
        vec_outside_widgets.push_back(ItemOutsideQuery(tag));

        // connect signal handlers with sigc::bind,
        // to supple the tag as a string along with the signal
        vec_outside_widgets[vec_outside_widgets.size() - 1].signal_add()
            .connect(sigc::bind(sigc::mem_fun(*this, &TagPicker::on_signal_add), tag));

        vec_outside_widgets[vec_outside_widgets.size() - 1].signal_exclude()
            .connect(sigc::bind(sigc::mem_fun(*this, &TagPicker::on_signal_exclude), tag));

        // add widget to box
        outside_box.append(vec_outside_widgets[vec_outside_widgets.size() - 1]);
    }
}

void TagPicker::clear_current_item_tags() {
    // delete all previous widgets
    for (auto &widget : vec_outside_widgets) {
        outside_box.remove(widget);
    }
    vec_outside_widgets.clear();
}

TagQuery TagPicker::get_current_query() const {
    return TagQuery(tags_include, tags_exclude);
}

sigc::signal<void (TagQuery)> TagPicker::signal_query_changed() {
    return private_signal_query_changed;
}

void TagPicker::remove(Gtk::Box &box,
        std::vector<ItemInQuery> &widget_vec,
        std::vector<Glib::ustring> &tag_vec,
        const Glib::ustring &tag)
{
        // find tag in database
        size_t idx;
        for(idx = 0; idx < tag_vec.size(); idx++) {
            if (tag_vec[idx] == tag) {
                tag_vec.erase(tag_vec.begin() + idx);
                break;
            }
        }

        // remove widget from box
        box.remove(widget_vec[idx]);

        // remove widget from owning vector
        widget_vec.erase(widget_vec.begin() + idx);
}

void TagPicker::on_signal_add(const Glib::ustring &tag) {
    // check if the database already contains this tag
    // check both include and exclude databases
    if (std::find(tags_exclude.begin(), tags_exclude.end(), tag) != tags_exclude.end() ||
        std::find(tags_include.begin(), tags_include.end(), tag) != tags_include.end()) {
        return;
    }

    // register tag in string based database
    tags_include.push_back(tag);

    // create the widget representing the tag and connect its signal
    vec_include_widgets.push_back(ItemInQuery(tag));
    vec_include_widgets[vec_include_widgets.size() - 1].signal_remove()
        .connect(sigc::bind(sigc::mem_fun(*this, &TagPicker::on_signal_remove), tag, false));

    // add widget to box
    include_box.append(vec_include_widgets[vec_include_widgets.size() - 1]);

    private_signal_query_changed.emit(TagQuery(tags_include, tags_exclude));
}

void TagPicker::on_signal_exclude(const Glib::ustring &tag) {
    // check if the database already contains this tag
    // check both include and exclude databases
    if (std::find(tags_exclude.begin(), tags_exclude.end(), tag) != tags_exclude.end() ||
        std::find(tags_include.begin(), tags_include.end(), tag) != tags_include.end()) {
        return;
    }

    // register tag in string based database
    tags_exclude.push_back(tag);

    // create the widget representing the tag and connect its signal
    vec_exclude_widgets.push_back(ItemInQuery(tag));
    vec_exclude_widgets[vec_exclude_widgets.size() - 1].signal_remove()
        .connect(sigc::bind(sigc::mem_fun(*this, &TagPicker::on_signal_remove), tag, true));

    // add widget to box
    exclude_box.append(vec_exclude_widgets[vec_exclude_widgets.size() - 1]);

    private_signal_query_changed.emit(TagQuery(tags_include, tags_exclude));
}

void TagPicker::on_signal_remove(const Glib::ustring &tag, bool exclude) {
    if (exclude) {
        remove(exclude_box, vec_exclude_widgets, tags_exclude, tag);
    }
    else {
        remove(include_box, vec_include_widgets, tags_include, tag);
    }

    private_signal_query_changed.emit(TagQuery(tags_include, tags_exclude));
}

bool TagPicker::on_match_selected(const Gtk::TreeModel::iterator &iter) {
    // get the tag
    Glib::ustring tag = (*iter)[list_model.tag];

    // use the same function that is used for signals
    on_signal_add(tag);

    // clear the entry's text buffer
    entry.get_buffer()->delete_text(0, -1);

    // return focus to the entry
    entry.grab_focus();

    // the signal is handled, no need for the default handler
    return true;
}

bool TagPicker::on_completion_match(const Glib::ustring &key,
                                    const Gtk::TreeModel::const_iterator &iter) {
    const auto row = *iter;
    Glib::ustring filter_str = row[list_model.tag];

    if (filter_str.lowercase().find(key.lowercase()) != Glib::ustring::npos) {
        return true;
    }

    return false;
}
