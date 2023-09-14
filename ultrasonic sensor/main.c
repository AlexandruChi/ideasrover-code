#include <bcm2835.h>

#include "../connection/connection.h"

int main() {
    double distance;

    if (!bcm2835_init()) {
        exit(1);
    }

    Connection connection = createLocalConnection()

    bcm2835_close();
    return 0;
}