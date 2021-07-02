#include <iostream>
#include <getopt.h>
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

using namespace std;

void usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [-q|--quiet]\n", program_name);
}

int main(int argc, char *argv[]) {
    bool quiet;
    int ch;
    SparkFun_Ambient_Light light_sensor;

    quiet = false;
    while (1) {
        int option_index;
        static struct option long_options[] = {
            {"quiet", no_argument, 0, 'q'},
            {0, 0, 0, 0}
        };

        option_index = 0;
        ch = getopt_long(argc, argv, "q", long_options, &option_index);
        if (ch == -1) {
            break;
        } else {
            switch (ch) {
            case 'q':
                quiet = true;
                break;
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
            }
        }
    }

    if (quiet) {
        cout << light_sensor.read_light() << endl;
    } else {
        cout << "light_sensor_value is " << light_sensor.read_light() << endl;
    }
    exit(EXIT_SUCCESS);
}
