// gtkmm
#include "sigc++/functors/mem_fun.h"

// project
#include "imageviewer.hh"

ImageViewer::ImageViewer()
:
    // should use precise binary fractions for base, min, max, step
    zoom_adj(Gtk::Adjustment::create(1, 0.125, 3, 0.0625)),
    hadj(get_hadjustment()),
    vadj(get_vadjustment())
{
    // picture setup
    pic.set_keep_aspect_ratio(true);
    pic.set_expand(true);
    set_child(pic);

    // scrolled window setup
    set_propagate_natural_height(true);
    set_propagate_natural_width(true);
    set_halign(Gtk::Align::CENTER);
    set_valign(Gtk::Align::CENTER);

    // scroll controller setup
    scroll_controller = Gtk::EventControllerScroll::create();
    scroll_controller->set_flags(Gtk::EventControllerScroll::Flags::BOTH_AXES);
    add_controller(scroll_controller);
    scroll_controller->signal_scroll().connect(sigc::mem_fun(*this, &ImageViewer::on_scroll), true);

    // motion controller setup
    motion_controller = Gtk::EventControllerMotion::create();
    add_controller(motion_controller);
    motion_controller->signal_motion().connect(sigc::mem_fun(*this, &ImageViewer::on_motion), true);

    // click gesture controller setup
    click_gesture = Gtk::GestureClick::create();
    click_gesture->set_button(GDK_BUTTON_PRIMARY);
    add_controller(click_gesture);
    click_gesture->signal_pressed().connect(sigc::mem_fun(*this, &ImageViewer::on_pressed), true);
    click_gesture->signal_released().connect(sigc::mem_fun(*this, &ImageViewer::on_released), true);
}

bool ImageViewer::set_image(const std::string &file_path) {
    try {
        buf = Gdk::Pixbuf::create_from_file(file_path);
        pic.set_pixbuf(buf);
        pic.set_can_shrink(true);

        return true;
    }
    catch (Glib::Error &error) {
        return false;
    }
}

void ImageViewer::zoom_in() {
    if (pic.get_can_shrink()) {
        zoom_adj->set_value(calculate_shrunken_zoom());
    }

    if ((zoom_adj->get_value() + zoom_adj->get_step_increment()) <= zoom_adj->get_upper()) {
        zoom_adj->set_value(zoom_adj->get_value() + zoom_adj->get_step_increment());
        zoom_apply();
    }

    zoom_adjust_scrollbars();

}

void ImageViewer::zoom_out() {
    if (pic.get_can_shrink()) {
        zoom_adj->set_value(calculate_shrunken_zoom());
    }

    if ((zoom_adj->get_value() + zoom_adj->get_step_increment()) >= zoom_adj->get_lower()) {
        zoom_adj->set_value(zoom_adj->get_value() - zoom_adj->get_step_increment());
        zoom_apply();
    }

    zoom_adjust_scrollbars();
}

void ImageViewer::zoom_reset() {
    zoom_adj->set_value(1);
    pic.set_pixbuf(buf);
    pic.set_can_shrink(true);
}

void ImageViewer::zoom_original() {
    zoom_adj->set_value(1);
    pic.set_pixbuf(buf);
    pic.set_can_shrink(false);
}

void ImageViewer::zoom_apply() {
    auto height = buf->get_height();
    auto width = buf->get_width();

    auto new_height = (int)(height * zoom_adj->get_value());
    auto new_width = (int)(width * zoom_adj->get_value());

    buf_zoom = buf->scale_simple(new_width, new_height, Gdk::InterpType::BILINEAR);
    pic.set_pixbuf(buf_zoom);
    pic.set_can_shrink(false);

}

// calculate the zoom level from a state where the
// image's can_shrink property is true and shrinking
// was handled by the image itself
double ImageViewer::calculate_shrunken_zoom(){
    double prop_height = (double)pic.get_height() / (double)buf->get_height();

    double prop_width = (double)pic.get_width() / (double)buf->get_width();

    // use the smaller of the two values
    double prop = (prop_height < prop_width) ? prop_height : prop_width;

    // snap result to nearest step increment
    double res = zoom_adj->get_lower();
    while(res < prop) { res += zoom_adj->get_step_increment(); }

    return res;
}

void ImageViewer::zoom_adjust_scrollbars() {
    // In a GtkAdjustment the allowed range for the value in a scrollbar
    // is Upper - Page Size. However, if both are 0, then no value can be set.
    // This prevents setting the scrollbar from an inital position where
    // the full GtkPicture that is attached to the ScrolledWindow is visible.
    //
    // Even if the GtkPicture widget has already been assigned a larger Pixbuf
    // the scrollbars will not change their GtkAdjustments until the
    // ScrolledWindow is redrawn. If it is redrawn without adjusted scrollbars
    // it will visibly jump to the new position once adjusted.
    //
    // To allow assigning a value, we first change the Upper property.
    // When zooming from this initial position, we center the scrollbars.

    double h_val, v_val;

    if (hadj->get_upper() == hadj->get_page_size()) {
        hadj->set_upper(buf_zoom->get_width());
        h_val = (buf_zoom->get_width() - hadj->get_page_size()) / 2;
    }

    // Otherwise set the sliders to proportionally identical places in the
    // new range
    else {
        // calculate old range
        double hrange_old = hadj->get_upper() - hadj->get_page_size();
        // calculate new range
        double hrange_new = buf_zoom->get_width() - hadj->get_page_size();
        // get the current slider position in proportion to the old range
        double hprop = hadj->get_value() / hrange_old;
        // apply proportion multiplier to new range
        h_val = hrange_new * hprop;
    }

    if (vadj->get_upper() == vadj->get_page_size()) {
        vadj->set_upper(buf_zoom->get_height());
        v_val = (buf_zoom->get_height() - vadj->get_page_size()) / 2;
    }
    else {
        // calculate old range
        double vrange_old = vadj->get_upper() - vadj->get_page_size();
        // calculate new range
        double vrange_new = buf_zoom->get_height() - vadj->get_page_size();
        // get the current slider position in proportion to the old range
        double vprop = vadj->get_value() / vrange_old;
        // apply proportion multiplier to new range
        v_val = vrange_new * vprop;
    }

    hadj->set_value(h_val);
    vadj->set_value(v_val);
}

bool ImageViewer::on_scroll(double dx, double dy) {
    if (dy == -1) {
        zoom_in();
    }
    else if (dy == 1) {
        zoom_out();
    }

    return true;
}

void ImageViewer::on_motion(double x, double y) {
    // std::cout << "motion x: " << x << " y: " << y << std::endl;
    if (mouse_down) {
        double x_movement = x - mouse_x;
        double y_movement = y - mouse_y;

        hadj->set_value(hadj->get_value() - x_movement);
        vadj->set_value(vadj->get_value() - y_movement);
    }
    mouse_x = x;
    mouse_y = y;
}

void ImageViewer::on_pressed(int n_pressed, double x, double y) {
    // std::cout << "click: n: " << n_pressed << " x: " << x << " y: " << y << std::endl;
    mouse_down = true;
}

void ImageViewer::on_released(int n_pressed, double x, double y) {
    // std::cout << "click released: n: " << n_pressed << " x: " << x << " y: " << y << std::endl;
    mouse_down = false;
}
