#pragma once

#include "ITemperatureSensor.hpp"

// Two-point controller with hysteresis:
//   heating turns ON  below (setpoint - hysteresis)
//   heating turns OFF above (setpoint + hysteresis)
// In between the current state is kept, which prevents fast switching.
class Thermostat {

    public:
        Thermostat(const ITemperatureSensor &sensor, double setpoint, double hysteresis = 0.5);

        // Reads the sensor once and updates the heating state.
        void update();

        void setSetpoint(double setpoint);
        double getSetpoint() const;

        void setHysteresis(double hysteresis);
        double getHysteresis() const;

        // Temperature of the last update() call.
        double getCurrentTemperature() const;

        bool isHeating() const;

    private:
        const ITemperatureSensor &_sensor;
        double _setpoint;
        double _hysteresis;
        double _currentTemperature;
        bool _heating;
};
