#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#define ULTRASONIC_SENSOR_SOCKET_PATH "/tmp/ultrasonicsensor"
#define OBJECT_DETECTION_CAMERA_PATH "/tmp/objectdetectioncamera"
#define PIXY_CAMERA_PATH "/tmp/pixycamera"
#define BATTERY_MANAGEMENT_SYSTEM_PATH "tmp/batterymanagementsystem"
#define ESC_PATH "/tmp/esc"
#define SERVO_PATH "/tmp/servo"
#define SELF_DRIVING_INPUT_PATH "/tmp/selfdrivinginput"
#define SELF_DRIVING_OUTPUT_PATH "/tmp/selfdrivingoutput"

#define PHONE_IP ""
#define PHONE_INPUT_PORT 4000
#define PHONE_OUTPUT_PORT 4001

// GPIO/BCM pin numbering

#define ULTRASONIC_SENSOR_TRIGGER_PIN 17
#define ULTRASONIC_SENSOR_ECHO_PIN 27

#endif // CONNECTIONS_H