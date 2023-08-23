#include <stdio.h>
#include <stdlib.h>

#include "../connection/connection.h"

int main() {
    Connection test = createNetworkConnection("", 4000, false, true, sizeof(int));
    int data = 65;

    while (!isConnected(test));
    printf("connected\n\n");
    setConnectionData(test, &data);

    destroyConnection(&test);
    return 0;
}