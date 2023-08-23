#include <stdio.h>
#include <stdlib.h>

#include "../connection/connection.h"

int main() {
    Connection test = createNetworkConnection("192.168.100.57", 4000, true, false, sizeof(int));
    int data = 70;

    while (!isConnected(test));
    printf("connected\n");
    printf("%d", data);
    //while (!getConnectionData(test, &data));

    printf("%d", data);
    fflush(stdout);

    destroyConnection(&test);
    return 0;
}