#ifndef DATATYPES_H
#define DATATYPES_H

// used to controll servo and ESC
typedef unsigned short PWM;

// distance in m
typedef double Distance;

// servo steer in % (-100% to 100%)
typedef double Steer;

// motor speed in % (0% to 100%)
typedef double Speed;

// lane data 
typedef struct laneData {
    // insert something here or replace struct
} LaneData;

// objects detecded by the object detection camera
typedef struct objectData {
    // insert something here or replace struct
} ObjectData;

// battery charge % and charging status
typedef struct batteryData {
    unsigned char batteryProcent;
    bool charging;
} BatteryData;

// modes that the ESC can be in:
// R: reverse
// N: neutral/stoped/parked
// D: drive
// B: braking
typedef enum ESCDriveMode {
    R = -1, N = 0, D = 1, B = 2
} ESCDriveMode;

// soeed and mode of the ESC
typedef struct ESCMode {
    ESCDriveMode mode;
    Speed speed;
} ESCMode;

// data recived by self driving
typedef struct selfDrivingData {
    bool running;
    // insert something here
} SelfDrivingData;

// ESC and servo parameters
typedef struct roverControll {
    ESCMode esc;
    Steer steer;
} RoverControll;

// data to display on phone
typedef struct roverData {
    // insert something here
} RoverData;

// data recived from the phone
typedef struct roverParameters {
    bool debug;
    bool running;
    bool test;
} RoverParameters;

#endif // DATATYPES_H