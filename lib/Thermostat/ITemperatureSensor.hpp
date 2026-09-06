#pragma once

class ITemperatureSensor {

    public:
        virtual ~ITemperatureSensor() = default;
        virtual double read() const = 0;
};
