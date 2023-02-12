// project
#include "additemwindow.hh"

AddItemWindow::AddItemWindow()
:
    label("Add Item Window")
{
    set_child(label);
    set_modal(true);
    set_size_request(200, 200);
}

bool AddItemWindow::on_close_request() {
    hide();
    return true;
}
