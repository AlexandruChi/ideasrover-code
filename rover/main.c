// TODO header file with the component path and ports

#include <unistd.h>

#include "../rover.h"

// TODO make better data types

struct objectData {
    // insert something here or replace struct
};

struct laneData {
    // insert something here or replace struct
};

struct batteryData {
    unsigned char batteryProcent;
    bool charging;
};

enum ESCModes {R = -1, N = 0, D = 1, B = 2};

struct ESCData {
    double speed;
    enum ESCModes mode;
};

struct selfDrivingData {
    bool running; // replace with something better
    // insert something here
};

struct roverControll {
    struct ESCData esc;
    double steer;
};

struct roverData {
    // data to display on phone
};

struct roverParameters {
    bool debug;
    bool running;
    bool test;
};

void runComponentExecutable(char* path);

int main() {

    // input devices
    Connection ultrasonicSensor, objectDetectionCamera, PixyCamera, batteryManagementSystem;
    // output devices
    Connection ESC, servo;
    // input-output devices
    Connection phoneInput, phoneOutput, selfDrvingInput, selfDrivingOutput;

    double distance;
    struct objectData objectData;
    struct laneData laneData;
    struct batteryData batteryData;

    struct ESCData ESCData;
    double steer;

    struct selfDrivingData selfDrivingData;
    struct roverControll roverControll;

    struct roverParameters roverParameters;
    struct roverData roverData;

    // TODO clean tmp files

    // creating server for input devices
    ultrasonicSensor = createLocalConnection(ULTRASONIC_SENSOR_SOCKET_PATH, true, true, sizeof(distance));
    objectDetectionCamera = createLocalConnection(OBJECT_DETECTION_CAMERA_PATH, true, true, sizeof(objectData));
    PixyCamera = createLocalConnection(PIXY_CAMERA_PATH, true, true, sizeof(laneData));
    batteryManagementSystem = createLocalConnection(BATTERY_MANAGEMENT_SYSTEM_PATH, true, true, sizeof(batteryData));

    // creating server for output devices
    ESC = createLocalConnection(ESC_PATH, false, true, sizeof(ESCData));
    servo = createLocalConnection(SERVO_PATH, false, true, sizeof(steer));

    // create server for the input-output devices
    selfDrvingInput = createLocalConnection(SELF_DRIVING_INPUT_PATH, true, true, sizeof(selfDrivingData));
    selfDrivingOutput = createLocalConnection(SELF_DRIVING_OUTPUT_PATH, false, true, sizeof(roverControll));

    // run the processes for components
    runComponentExecutable(""); // TODO replace with stuff

    // wait for local sockets to create
    wait_while_not (
            isConnected(ultrasonicSensor) and
            isConnected(objectDetectionCamera) and
            isConnected(PixyCamera) and
            isConnected(batteryManagementSystem) and
            isConnected(ESC) and
            isConnected(servo) and
            isConnected(selfDrvingInput) and
            isConnected(selfDrivingOutput)
    )

    // create server for phone
    // maybe add script to create network
    phoneInput = createNetworkConnection(PHONE_IP, PHONE_INPUT_PORT, true, true, sizeof(roverParameters));
    phoneOutput = createNetworkConnection(PHONE_IP, PHONE_OUTPUT_PORT, false, true, sizeof(roverData));

    // wait for phone to connect to rover
    wait_while_not (isConnected(phoneInput) and isConnected(phoneOutput))

    // TODO make better
    // TODO make threads
    // TODO fix errors
    // TODO maybe check data before using it
    loop {
        // get data from sensors and cameras
        getConnectionData(ultrasonicSensor, &distance);
        getConnectionData(objectDetectionCamera, &objectData);
        getConnectionData(PixyCamera, &laneData);
        getConnectionData(batteryManagementSystem, &batteryData);

        // insert code here for data procesing
        // selfDrivingData = something usefull

        // check if it should run first

        // send and recive data to selfdriving program
        setConnectionData(selfDrvingInput, &selfDrivingData);
        getConnectionData(selfDrivingOutput, &roverControll);

        // send data to servo and ESC
        steer = roverControll.steer;
        ESCData = roverControll.esc;
        
        setConnectionData(servo, &steer);
        setConnectionData(ESC, &ESCData);

        // insert code here for data procesing
        // roverData = something usefull

        // get parameters and send data to phone
        setConnectionData(phoneOutput, &roverData);
        getConnectionData(phoneInput, &roverParameters);
    }

    // find a way to stop the program when powering off the PI
    return 0;
}

void runComponentExecutable(char* path) {
    if (!fork()) {
        for(;;) {
            execl(path, path, NULL);
        }
    }
}