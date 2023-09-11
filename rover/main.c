// TODO header file with the component path and ports

#include "../connection/connection.h"

#define and &&
#define or ||

#define loop while (1)

#define wait_while(X) while (X) {}
#define wait_while_not(X) wait_while(!(X))

#define PHONE_INPUT_PORT 4000
#define PHONE_OUTPUT_PORT 4001

// TODO make better data types

struct objectDetectionCameraData {
    // insert something here or replace struct
};

struct PixyCameraData {
    // insert something here or replace struct
};

struct batteryManagementSystemData {
    unsigned char batteryProcent;
    bool charging;
};

enum ESCModes {R = -1, N = 0, D = 1, B = 2};

struct ESCData {
    double speed;
    enum ESCModes mode;
};

struct selfDrivingData {
    // insert something here
};

struct roverControll {
    struct ESCData esc;
    double speed;
};

struct roverData {
    // data to display on phone
};

struct roverParameters {
    bool debug;
    bool running;
    bool test;
};

int main() {

    // input devices
    Connection ultrasonicSensor, objectDetectionCamera, PixyCamera, batteryManagementSystem;
    // output devices
    Connection ESC, servo;
    // input-output devices
    Connection phoneInput, phoneOutput, selfDrvingInput, selfDrivingOutput;

    double distance;
    struct objectDetectionCameraData objectDetectionCameraData;
    struct PixyCameraData PixyCameraData;
    struct batteryManagementSystemData batteryManagementSystemData;

    struct ESCData ESCData;
    double steer;

    struct selfDrivingData selfDrivingData;
    struct roverControll roverControll;

    struct roverParameters roverParameters;
    struct roverData roverData;

    // creating server for input devices
    ultrasonicSensor = createLocalConnection("/tmp/ultrasonicsensor", true, true, sizeof(distance));
    objectDetectionCamera = createLocalConnection("/tmp/objectdetectioncamera", true, true, sizeof(objectDetectionCameraData));
    PixyCamera = createLocalConnection("/tmp/pixycamera", true, true, sizeof(PixyCameraData));
    batteryManagementSystem = createLocalConnection("tmp/batterymanagementsystem", true, true, sizeof(batteryManagementSystemData));

    // creating server for output devices
    ESC = createLocalConnection("/tmp/esc", false, true, sizeof(ESCData));
    servo = createLocalConnection("/tmp/servo", false, true, sizeof(steer));

    // create server for the input-output devices
    selfDrvingInput = createLocalConnection("/tmp/selfdrivinginput", true, true, sizeof(selfDrivingData));
    selfDrivingOutput = createLocalConnection("/tmp/selfdrivingoutput", false, true, sizeof(roverControll));

    phoneInput = createNetworkConnection("", PHONE_INPUT_PORT, true, true, sizeof(roverParameters));
    phoneOutput = createNetworkConnection("", PHONE_OUTPUT_PORT, false, true, sizeof(roverData));


    


    wait_while_not (
            isConnected(ultrasonicSensor) and
            isConnected(objectDetectionCamera) and
            isConnected(PixyCamera) and
            isConnected(batteryManagementSystem) and
            isConnected(ESC) and
            isConnected(servo) and
            isConnected(selfDrvingInput) and
            isConnected(selfDrivingOutput) and
            isConnected(phoneInput) and
            isConnected(phoneOutput)
    )

    loop {
        
    }

    // find a way to stop the program when powering off the PI
    return 0;
}