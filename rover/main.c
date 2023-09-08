// TODO header file with the component path and ports
// TODO maybe add Connection with bouth input and output if it is not to hard

#include "../connection/connection.h"

struct objectDetectionCameraData {
    // insert something here
};

struct PixyCameraData {
    // insert something here
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

struct 

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

    // creating server for input devices
    ultrasonicSensor = createLocalConnection("/tmp/ultrasonicsensor", true, true, sizeof(distance));
    objectDetectionCamera = createLocalConnection("/tmp/objectdetectioncamera", true, true, sizeof(objectDetectionCameraData));
    PixyCamera = createLocalConnection("/tmp/pixycamera", true, true, sizeof(PixyCameraData));
    batteryManagementSystem = createLocalConnection("tmp/batterymanagementsystem", true, true, sizeof(batteryManagementSystemData));

    // creating server for output devices
    ESC = createLocalConnection("/tmp/esc", false, true, sizeof(ESCData));
    servo = createLocalConnection("/tmp/servo", false, true, sizeof(steer));

    // create server for the input-output devices
    selfDrvingInput = createLocalConnection("/tmp/selfdrivinginput", true, true, )

    // find a way to stop the program when powering off the PI
    return 0;
}