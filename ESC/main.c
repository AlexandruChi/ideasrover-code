#include "../macros.h"
#include "../connection/connection.h"
#include "../sockets.h"
#include "../pins.h"
#include "../datatypes.h"

// TODO check if delay betwen cahnges in PWM is required
// TODO check if ESC suports breaking and how it is implemented

int main() {
    wait_while_not (bcm2835_init())

    ESCMode escMode = {N, 0};
    Connection connection = createLocalConnection(ESC_PATH, true, false, sizeof(ESCMode));
    bool breaking = false;

    bcm2835_gpio_fsel(ESC_PWM_PIN, ESC_PWM_PIN_MODE);
    bcm2835_pwm_set_range(ESC_PWM_CHANNEL, ESC_MAX_PWM);
    bcm2835_pwm_set_data(ESC_PWM_CHANNEL, ESC_NEUTRAL_PWM);
    bcm2835_pwm_set_mode(ESC_PWM_CHANNEL, true, true);

    PWM maxPWMDif = ESC_MAX_PWM - ESC_NEUTRAL_PWM;
    PWM minPWMDif = ESC_NEUTRAL_PWM - ESC_MIN_PWM;
    PWM pwm = ESC_NEUTRAL_PWM;

    loop {
        getConnectionData(connection, &escMode);

        switch (escMode.mode) {
            case N:
                pwm = ESC_NEUTRAL_PWM;
                breaking = false;
                break;

            case D:
                if (pwm < ESC_NEUTRAL_PWM and !breaking) {
                    bcm2835_pwm_set_data(ESC_PWM_CHANNEL, ESC_NEUTRAL_PWM);
                }

                if (escMode.speed < 0) {
                    escMode.speed = 0;
                } else if (escMode.speed > 100) {
                    escMode.speed = 100;
                }
                pwm = ESC_NEUTRAL_PWM + (escMode.speed * maxPWMDif);
                breaking = false;
                break;

            case R:
                if (pwm > ESC_NEUTRAL_PWM and !breaking) {
                    bcm2835_pwm_set_data(ESC_PWM_CHANNEL, ESC_NEUTRAL_PWM);
                }

                if (escMode.speed < 0) {
                    escMode.speed = 0;
                } else if (escMode.speed > 100) {
                    escMode.speed = 100;
                }
                pwm = ESC_NEUTRAL_PWM - (escMode.speed * maxPWMDif);
                breaking = false;
                break;

            case B:
                if (!breaking and pwm != ESC_NEUTRAL_PWM) {
                    if (pwm > ESC_NEUTRAL_PWM) {
                        pwm = ESC_MIN_PWM;
                    } else if (pwm < ESC_NEUTRAL_PWM) {
                        pwm = ESC_MAX_PWM;
                    }
                    breaking = true;
                }
                break;

            default:
                pwm = ESC_NEUTRAL_PWM;
                breaking = false;
                break;
        }

        if (pwm > ESC_MAX_PWM) {
            pwm = ESC_MAX_PWM;
        }

        if (pwm < ESC_MIN_PWM) {
            pwm = ESC_MIN_PWM;
        }

        bcm2835_pwm_set_data(ESC_PWM_CHANNEL, pwm);
    }

    bcm2835_close();
    return 0;
}