#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h> // for sleep()

#define SENSOR_ID "28-01193a10a833"
#define SENSOR_MEASUREMENTS_PATH "/sys/bus/w1/devices/28-01193a10a833/w1_slave"

using namespace std;

string get_temp() {
    ifstream file(SENSOR_MEASUREMENTS_PATH);
    if (!file.is_open()) {
        cerr << "Error: Could not open " << SENSOR_MEASUREMENTS_PATH << endl;
        return "N/A";
    }

    string line, temp_data;
    while (getline(file, line)) {
        if (line.find("t=") != string::npos) {
            temp_data = line.substr(line.find("t=") + 2);
        }
    }
    file.close();

    if (!temp_data.empty()) {
        float temp = stoi(temp_data) / 1000.0;
            ostringstream stream;
            stream << temp << " °C";
            return stream.str();
    }

    return "N/A";

}

int main(){
    while (true) {
        cout << "Temperature: " << get_temp() << endl;
        sleep(2);
    }
}