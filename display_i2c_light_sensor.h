#ifndef DISPLAY_I2C_LIGHT_SENSOR_H
#define DISPLAY_I2C_LIGHT_SENSOR_H

#include <QMainWindow>
#include <QTimer>
#include <QtGlobal>
#include <QDateTime>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
using namespace QtCharts;
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class display_i2c_light_sensor;
}
QT_END_NAMESPACE

class display_i2c_light_sensor : public QMainWindow {
    Q_OBJECT

  public:
    display_i2c_light_sensor(QWidget *parent = nullptr);
    ~display_i2c_light_sensor();
  public slots:
    void update_ambient_light(void);
  private:
    Ui::display_i2c_light_sensor *ui;
    SparkFun_Ambient_Light light_sensor;
    QTimer update_light_timer;
    QLineSeries *series;
    QChart *light_chart;
    QChartView *light_chart_view;
    QDateTimeAxis *axisX;
    QValueAxis *axisY;
    QDateTime min_time;
    QDateTime max_time;
    qreal min_reading;
    qreal max_reading;
};
#endif // DISPLAY_I2C_LIGHT_SENSOR_H
