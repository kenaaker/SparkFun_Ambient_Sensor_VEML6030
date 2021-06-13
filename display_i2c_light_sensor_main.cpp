#include <QApplication>
#include "display_i2c_light_sensor.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    display_i2c_light_sensor w;
    int app_return_code;

    w.show();
    app_return_code = a.exec();
    return app_return_code;
    return 0;
}
