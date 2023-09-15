#include <unistd.h>

#include "../macros.h"
#include "../connection/connection.h"
#include "../sockets.h"
#include "../datatypes.h"

void runComponentExecutable(char* path);

int main() {

    // input devices
    Connection ultrasonicSensor, objectDetectionCamera, PixyCamera, batteryManagementSystem;
    // output devices
    Connection ESC, servo;
    // input-output devices
    Connection phoneInput, phoneOutput, selfDrvingInput, selfDrivingOutput;

    Distance distance;
    ObjectData objectData;
    LaneData laneData;
    BatteryData batteryData;

    ESCMode ESCData;
    Steer steer;

    SelfDrivingData selfDrivingData;
    RoverControll roverControll;

    RoverParameters roverParameters;
    RoverData roverData;

    // TODO clean tmp files

    // creating server for input devices
    ultrasonicSensor = createLocalConnection(ULTRASONIC_SENSOR_SOCKET_PATH, true, true, sizeof(Distance));
    objectDetectionCamera = createLocalConnection(OBJECT_DETECTION_CAMERA_PATH, true, true, sizeof(ObjectData));
    PixyCamera = createLocalConnection(PIXY_CAMERA_PATH, true, true, sizeof(LaneData));
    batteryManagementSystem = createLocalConnection(BATTERY_MANAGEMENT_SYSTEM_PATH, true, true, sizeof(BatteryData));

    // creating server for output devices
    ESC = createLocalConnection(ESC_PATH, false, true, sizeof(ESCMode));
    servo = createLocalConnection(SERVO_PATH, false, true, sizeof(Steer));

    // create server for the input-output devices
    selfDrvingInput = createLocalConnection(SELF_DRIVING_INPUT_PATH, true, true, sizeof(SelfDrivingData));
    selfDrivingOutput = createLocalConnection(SELF_DRIVING_OUTPUT_PATH, false, true, sizeof(RoverControll));

    // run the processes for components
    //runComponentExecutable(pathToExecutable);
    //system(consoleCommand);

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
    phoneInput = createNetworkConnection(PHONE_IP, PHONE_INPUT_PORT, true, true, sizeof(RoverParameters));
    phoneOutput = createNetworkConnection(PHONE_IP, PHONE_OUTPUT_PORT, false, true, sizeof(RoverData));

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