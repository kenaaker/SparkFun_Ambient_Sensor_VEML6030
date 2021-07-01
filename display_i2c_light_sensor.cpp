#include "display_i2c_light_sensor.h"
#include "./ui_display_i2c_light_sensor.h"
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QtCore/QDateTime>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <limits>

display_i2c_light_sensor::display_i2c_light_sensor(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::display_i2c_light_sensor),
      light_sensor(),
      update_light_timer() {
    QMainWindow *my_main_window;

    my_main_window = this;
    ui->setupUi(this);
    series = new QLineSeries();
    light_chart = new QChart();
    light_chart_view = new QChartView(light_chart);
    light_chart->addSeries(series);
    light_chart->legend()->hide();
    light_chart->setTitle("Ambient Light detector values.");
    light_chart_view->setRenderHint(QPainter::Antialiasing);
    connect(&update_light_timer, SIGNAL(timeout()), this, SLOT(update_ambient_light()));
    update_light_timer.start(1000*10);
    my_main_window->setCentralWidget(light_chart_view);

    axisX = new QDateTimeAxis;
    axisX->setFormat("h:m:s.z");
    axisX->setTitleText("Time");
    light_chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Ambient Light Reading");
    light_chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    min_reading = std::numeric_limits<qreal>::max();
    max_reading = std::numeric_limits<qreal>::min();
}

void display_i2c_light_sensor::update_ambient_light(void) {
    uint32_t light_reading;
    double output_light_reading;

    light_reading = light_sensor.read_light();
    output_light_reading = light_reading;
    if (light_reading < min_reading) {
        min_reading = light_reading;
    }
    if (light_reading > max_reading) {
        max_reading = light_reading * 1.5;
    }
    if (series->count() == 0) {
        axisX->setMin(QDateTime::currentDateTime());
        axisY->setMin(0);
    } else {
        axisY->setMin(min_reading);
        axisY->setMax(max_reading);
    }
    series->append(QDateTime::currentDateTime().toMSecsSinceEpoch(), output_light_reading);
    axisX->setMax(QDateTime::currentDateTime());
    axisY->setMax(light_reading);
}

display_i2c_light_sensor::~display_i2c_light_sensor() {
    delete ui;
}
