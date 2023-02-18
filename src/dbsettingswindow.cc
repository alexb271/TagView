// project
#include "dbsettingswindow.hh"


DbSettingsWindow::DbSettingsWindow() {
    set_title("DB Settings");
    set_child(box);
}

bool DbSettingsWindow::on_close_request() {
    hide();
    return true;
}
