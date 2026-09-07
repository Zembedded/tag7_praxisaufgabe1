#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Thermostat.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnPointee;

class MockTemperatureSensor : public ITemperatureSensor {
    public:
        MOCK_METHOD(double, read, (), (const, override));
};

class ThermostatTest : public ::testing::Test {
    protected:
        static constexpr double SETPOINT = 21.0;
        static constexpr double HYSTERESIS = 0.5;

        NiceMock<MockTemperatureSensor> sensor;
        double temperature = 21.0;
        Thermostat thermostat{sensor, SETPOINT, HYSTERESIS};

        void SetUp() override {
            // every read() returns whatever `temperature` currently holds
            ON_CALL(sensor, read()).WillByDefault(ReturnPointee(&temperature));
        }

        // Simulates one control cycle at the given temperature.
        void updateAt(double celsius) {
            temperature = celsius;
            thermostat.update();
        }
};

TEST_F(ThermostatTest, StartsIdle) {
    EXPECT_TRUE(thermostat.isHeating());
}

TEST_F(ThermostatTest, ReturnsSetpointAndHysteresisFromConstructor) {
    EXPECT_DOUBLE_EQ(SETPOINT, thermostat.getSetpoint());
    EXPECT_DOUBLE_EQ(HYSTERESIS, thermostat.getHysteresis());
}

TEST_F(ThermostatTest, UpdateReadsSensorExactlyOnce) {
    EXPECT_CALL(sensor, read()).Times(1).WillOnce(Return(18.0));
    std::cout << "Hallo" << std::endl;

    thermostat.update();
}

TEST_F(ThermostatTest, UpdateStoresCurrentTemperature) {
    updateAt(18.25);

    EXPECT_DOUBLE_EQ(18.25, thermostat.getCurrentTemperature());
}

TEST_F(ThermostatTest, TurnsHeatingOnBelowLowerThreshold) {
    updateAt(SETPOINT - HYSTERESIS - 0.1);

    EXPECT_TRUE(thermostat.isHeating());
}

TEST_F(ThermostatTest, TurnsHeatingOffAboveUpperThreshold) {
    updateAt(SETPOINT - 1.0);   // heating on
    updateAt(SETPOINT + HYSTERESIS + 0.1);

    EXPECT_FALSE(thermostat.isHeating());
}

TEST_F(ThermostatTest, KeepsHeatingOnInsideHysteresisBand) {
    updateAt(SETPOINT - 1.0);   // heating on
    updateAt(SETPOINT);         // inside the band

    EXPECT_TRUE(thermostat.isHeating());
}

TEST_F(ThermostatTest, KeepsHeatingOffInsideHysteresisBand) {
    updateAt(SETPOINT + 1.0);   // heating off
    updateAt(SETPOINT);         // inside the band

    EXPECT_FALSE(thermostat.isHeating());
}

TEST_F(ThermostatTest, LowerThresholdItselfIsStillInsideTheBand) {
    updateAt(SETPOINT - HYSTERESIS);

    EXPECT_FALSE(thermostat.isHeating());
}

TEST_F(ThermostatTest, UpperThresholdItselfIsStillInsideTheBand) {
    updateAt(SETPOINT - 1.0);        // heating on
    updateAt(SETPOINT + HYSTERESIS);

    EXPECT_TRUE(thermostat.isHeating());
}

TEST_F(ThermostatTest, RunsCompleteHeatingCycle) {
    updateAt(19.0);
    EXPECT_TRUE(thermostat.isHeating());

    updateAt(20.8);   // warming up, still inside the band
    EXPECT_TRUE(thermostat.isHeating());

    updateAt(21.6);   // above the upper threshold
    EXPECT_FALSE(thermostat.isHeating());

    updateAt(21.2);   // cooling down, still inside the band
    EXPECT_FALSE(thermostat.isHeating());

    updateAt(20.4);   // below the lower threshold again
    EXPECT_TRUE(thermostat.isHeating());
}

TEST_F(ThermostatTest, NewSetpointTakesEffectOnNextUpdate) {
    updateAt(22.0);
    EXPECT_FALSE(thermostat.isHeating());

    thermostat.setSetpoint(25.0);
    EXPECT_DOUBLE_EQ(25.0, thermostat.getSetpoint());
    EXPECT_FALSE(thermostat.isHeating()) << "state must not change before update()";

    thermostat.update();
    EXPECT_TRUE(thermostat.isHeating());
}

TEST_F(ThermostatTest, WiderHysteresisSuppressesSwitching) {
    thermostat.setHysteresis(3.0);

    updateAt(19.0);   // would switch on with 0.5, but not with 3.0

    EXPECT_FALSE(thermostat.isHeating());
}

TEST_F(ThermostatTest, ZeroHysteresisSwitchesDirectlyAtSetpoint) {
    thermostat.setHysteresis(0.0);

    updateAt(SETPOINT - 0.01);
    EXPECT_TRUE(thermostat.isHeating());

    updateAt(SETPOINT + 0.01);
    EXPECT_FALSE(thermostat.isHeating());
}

TEST_F(ThermostatTest, NegativeHysteresisIsClampedToZero) {
    Thermostat clamped(sensor, SETPOINT, -2.0);
    EXPECT_DOUBLE_EQ(0.0, clamped.getHysteresis());

    thermostat.setHysteresis(-2.0);
    EXPECT_DOUBLE_EQ(0.0, thermostat.getHysteresis());
}
