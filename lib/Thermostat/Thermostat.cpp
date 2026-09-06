#include "Thermostat.hpp"

Thermostat::Thermostat(const ITemperatureSensor &sensor, double setpoint, double hysteresis)
    : _sensor(sensor),
      _setpoint(setpoint),
      _hysteresis(hysteresis < 0.0 ? 0.0 : hysteresis),
      _currentTemperature(0.0),
      _heating(false) {
}

void Thermostat::update() {
    _currentTemperature = _sensor.read();

    if (_currentTemperature < _setpoint - _hysteresis) {
        _heating = true;
    } else if (_currentTemperature > _setpoint + _hysteresis) {
        _heating = false;
    }
    // inside the hysteresis band the state stays unchanged
}

void Thermostat::setSetpoint(double setpoint) {
    _setpoint = setpoint;
}

double Thermostat::getSetpoint() const {
    return _setpoint;
}

void Thermostat::setHysteresis(double hysteresis) {
    _hysteresis = hysteresis < 0.0 ? 0.0 : hysteresis;
}

double Thermostat::getHysteresis() const {
    return _hysteresis;
}

double Thermostat::getCurrentTemperature() const {
    return _currentTemperature;
}

bool Thermostat::isHeating() const {
    return _heating;
}
