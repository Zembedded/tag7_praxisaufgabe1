#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char **argv)
{
    // InitGoogleMock also initializes GoogleTest
    ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS())
    ;

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
