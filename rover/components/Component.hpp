#ifndef UltrasonicSensor_hpp
#define UltrasonicSensor_hpp

#include <string>

#include "../../connection/connection.h"

class Component {
    Connection connection;
public:
    UltrasonicSensor(std::string socketPath);
    ~UltrasonicSensor();
};

#endif /* UltrasonicSensor_hpp */
