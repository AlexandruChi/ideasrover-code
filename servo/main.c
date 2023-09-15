#include "../macros.h"
#include "../connection/connection.h"
#include "../sockets.h"
#include "../pins.h"
#include "../datatypes.h"

// TODO check if delay betwen cahnges in PWM is required

int main() {
    wait_while_not (bcm2835_init())

    Steer steer = 0;
    Connection connection = createLocalConnection(SERVO_PATH, true, false, sizeof(Steer));

    bcm2835_gpio_fsel(SERVO_PWM_PIN, SERVO_PWM_PIN_MODE);
    bcm2835_pwm_set_range(SERVO_PWM_CHANNEL, SERVO_MAX_PWM);
    bcm2835_pwm_set_data(SERVO_PWM_CHANNEL, SERVO_NEUTRAL_PWM);
    bcm2835_pwm_set_mode(SERVO_PWM_CHANNEL, true, true);

    PWM maxPWMDif = SERVO_MAX_PWM - SERVO_NEUTRAL_PWM;
    PWM minPWMDif = SERVO_NEUTRAL_PWM - SERVO_MIN_PWM;

    loop {
        getConnectionData(connection, &steer);
        PWM pwm;

        if (!steer) {
            pwm = SERVO_NEUTRAL_PWM;
        } else if (steer > 0) {
            if (steer > 100) {
                pwm = SERVO_MAX_PWM;
            } else {
                pwm = SERVO_NEUTRAL_PWM + (steer * maxPWMDif);
            }
        } else if (steer < 0) {
            if (steer < -100) {
                pwm = SERVO_MIN_PWM;
            } else {
                pwm = SERVO_NEUTRAL_PWM - (steer * minPWMDif);
            }
        }

        if (pwm > SERVO_MAX_PWM) {
            pwm = SERVO_MAX_PWM;
        }

        if (pwm < SERVO_MIN_PWM) {
            pwm = SERVO_MIN_PWM;
        }

        bcm2835_pwm_set_data(SERVO_PWM_CHANNEL, pwm);
    }

    bcm2835_close();
    return 0;
}