//=======================================================================
// Copyright (c) 2015-2016 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>

#include <string>
#include <cstring>

#include <unistd.h>

#include<wiringPi.h>

namespace {

void read_data(){
    uint8_t laststate = HIGH;

    int dht11_dat[5] = { 0, 0, 0, 0, 0 };

    const std::size_t gpio_pin = 24;

    /* pull pin down for 18 milliseconds */
    pinMode(gpio_pin, OUTPUT);
    digitalWrite(gpio_pin, LOW);
    delay(18);

    /* then pull it up for 40 microseconds */
    digitalWrite(gpio_pin, HIGH);
    delayMicroseconds(40);

    /* prepare to read the pin */
    pinMode(gpio_pin, INPUT);

    /* detect change and read data */
    uint8_t j = 0;
    for(uint8_t i = 0; i < 85; i++){
        uint8_t counter = 0;
        while (digitalRead(gpio_pin) == laststate){
            counter++;
            delayMicroseconds(1);
            if (counter == 255){
                break;
            }
        }

        laststate = digitalRead(gpio_pin);

        if (counter == 255){
            break;
        }

        /* ignore first 3 transitions */
        if (i >= 4 && i % 2 == 0){
            /* shove each bit into the storage bytes */
            dht11_dat[j / 8] <<= 1;
            if (counter > 16){
                dht11_dat[j / 8] |= 1;
            }
            j++;
        }
    }

    /*
     * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
     */
    if (j >= 40 && dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)){
        auto humidity = dht11_dat[0];
        auto temperature = dht11_dat[2];

        auto data = std::to_string(temperature) + ";" + std::to_string(humidity) + ";2";

        std::string base_url = "http://192.168.20.140:8066/json.htm?";

        auto command = base_url + "type=command&param=udevice&idx=2&nvalue=0&svalue=" + data;

        system(("curl -s \"" + command + "\" > /dev/null").c_str());
    }
}

} //End of anonymous namespace

int main(){
    // Run the wiringPi setup
    wiringPiSetup();

    // Send data to the server continuously
    while(true){
        read_data();

        // Wait some time before messages
        usleep(30 * 1000 * 1000);
    }

    return 0;
}
