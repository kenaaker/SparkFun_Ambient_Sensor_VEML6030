/********************************************************************************
** Form generated from reading UI file 'display_i2c_light_sensor.ui'
**
** Created by: Qt User Interface Compiler version 5.12.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAY_I2C_LIGHT_SENSOR_H
#define UI_DISPLAY_I2C_LIGHT_SENSOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_display_i2c_light_sensor
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *display_i2c_light_sensor)
    {
        if (display_i2c_light_sensor->objectName().isEmpty())
            display_i2c_light_sensor->setObjectName(QString::fromUtf8("display_i2c_light_sensor"));
        display_i2c_light_sensor->resize(800, 600);
        centralwidget = new QWidget(display_i2c_light_sensor);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        display_i2c_light_sensor->setCentralWidget(centralwidget);
        menubar = new QMenuBar(display_i2c_light_sensor);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        display_i2c_light_sensor->setMenuBar(menubar);
        statusbar = new QStatusBar(display_i2c_light_sensor);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        display_i2c_light_sensor->setStatusBar(statusbar);

        retranslateUi(display_i2c_light_sensor);

        QMetaObject::connectSlotsByName(display_i2c_light_sensor);
    } // setupUi

    void retranslateUi(QMainWindow *display_i2c_light_sensor)
    {
        display_i2c_light_sensor->setWindowTitle(QApplication::translate("display_i2c_light_sensor", "display_i2c_light_sensor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class display_i2c_light_sensor: public Ui_display_i2c_light_sensor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAY_I2C_LIGHT_SENSOR_H
