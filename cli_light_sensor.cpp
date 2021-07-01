#include <iostream>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

using namespace std;

int main() {
    SparkFun_Ambient_Light light_sensor;
    cout << "light_sensor_value is " << light_sensor.read_light() << endl;
    return 0;
}
