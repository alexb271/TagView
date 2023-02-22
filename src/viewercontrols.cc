// gtkmm
#include "gtkmm/enums.h"

// project
#include "viewercontrols.hh"

ViewerControls::ViewerControls() {
    // configure self
    set_orientation(Gtk::Orientation::HORIZONTAL);

    // configure buttons
    button_zoom_original.set_icon_name("zoom-original-symbolic");
    button_zoom_original.set_tooltip_text("Actual size");
    button_zoom_original.set_margin_end(10);
    button_zoom_out.set_icon_name("zoom-out-symbolic");
    button_zoom_out.set_tooltip_text("Zoom out");
    button_zoom_reset.set_icon_name("zoom-fit-best-symbolic");
    button_zoom_reset.set_tooltip_text("Fit to screen");
    button_zoom_in.set_icon_name("zoom-in-symbolic");
    button_zoom_in.set_tooltip_text("Zoom in");
    button_hide_viewer.set_icon_name("go-previous-symbolic");
    button_hide_viewer.set_tooltip_text("Back");
    button_hide_viewer.set_margin_start(15);

    // append buttons
    append(button_zoom_original);
    append(button_zoom_out);
    append(button_zoom_reset);
    append(button_zoom_in);
    append(button_hide_viewer);
}

Glib::SignalProxy<void()> ViewerControls::signal_zoom_original() {
    return button_zoom_original.signal_clicked();
}

Glib::SignalProxy<void()> ViewerControls::signal_zoom_out() {
    return button_zoom_out.signal_clicked();
}

Glib::SignalProxy<void()> ViewerControls::signal_zoom_reset() {
    return button_zoom_reset.signal_clicked();
}

Glib::SignalProxy<void()> ViewerControls::signal_zoom_in() {
    return button_zoom_in.signal_clicked();
}

Glib::SignalProxy<void()> ViewerControls::signal_hide_viewer() {
    return button_hide_viewer.signal_clicked();
}
