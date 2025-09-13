#include <unity.h>
#include <cmdBuffer.h>

// void setup() {}
// void loop() {}

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_decode() {

    // Example data received (10 bytes)
    //  IotCommand myCommand = {Move, Forward, 10, 20, 30};
    uint8_t exampleData[12] = {1, 0, 1, 0, 1, 0, 10, 0, 20, 0, 30, 0};

    IotCommand decodedCmd = decodeCommand(exampleData);
    // debugIotCommand(&decodedCmd);

    TEST_ASSERT_TRUE(decodedCmd.cmd == 1);
}


int main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_decode);

    UNITY_END();
}