#include "../macros.h"
#include "../connection/connection.h"
#include "../sockets.h"
#include "../pins.h"
#include "../datatypes.h"

int main() {
    wait_while_not (bcm2835_init())

    Distance distance = 0;
    Connection connection = createLocalConnection(ULTRASONIC_SENSOR_SOCKET_PATH, false, false, sizeof(Distance));

    bcm2835_gpio_fsel(ULTRASONIC_SENSOR_TRIGGER_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(ULTRASONIC_SENSOR_TRIGGER_PIN, LOW);
    bcm2835_gpio_fsel(ULTRASONIC_SENSOR_ECHO_PIN, BCM2835_GPIO_FSEL_INPT);

    loop {
        bcm2835_gpio_write(ULTRASONIC_SENSOR_TRIGGER_PIN, HIGH);
        bcm2835_delayMicroseconds(TRIGGER_PIN_DELAY_US);
        bcm2835_gpio_write(ULTRASONIC_SENSOR_TRIGGER_PIN, LOW);
        wait_while_not (bcm2835_gpio_lev(ULTRASONIC_SENSOR_ECHO_PIN))
        uint32_t startTimer = bcm2835_st_read();
        wait_while (bcm2835_gpio_lev(ULTRASONIC_SENSOR_ECHO_PIN));
        uint32_t endTimer = bcm2835_st_read();
        distance = (endTimer - startTimer) / 2 * SPEED_OF_SOUND_M_US;
        setConnectionData(connection, &distance);
    }

    bcm2835_close();
    return 0;
}