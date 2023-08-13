#include <stdio.h>
#include <stdlib.h>

#include "../connection/connection.h"

int main() {
    Connection test = createNetworkConnection("", 4000, true, true, 10);
    char *data = (char*)malloc(10);

    while (!isConnected(test));

    printf("connected\n\n");

    while (true) {
        getc(stdin);
        getConnectionData(test, data);
        printf("%s\n", data);
    }

    destroyConnection(test);
}