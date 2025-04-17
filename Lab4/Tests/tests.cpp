#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "structs.h"
#include <cstring>

TEST_SUITE("SharedData Structure Tests") {
    TEST_CASE("Size and Alignment") {
        CHECK(sizeof(SharedData) == (sizeof(int) * 2 + 20));
    }

    TEST_CASE("Initialization") {
        SharedData data = { -1, -1, {'t','e','s','t'} };
        CHECK(data.messageID == -1);
        CHECK(data.writerID == -1);
        CHECK(std::strncmp(data.message, "test", 4) == 0);
    }

    TEST_CASE("Message Boundary") {
        SharedData data;
        std::memset(data.message, 0, sizeof(data.message));
        const char* longMsg = "This is a very long message exceeding 20 characters";
        std::strncpy(data.message, longMsg, sizeof(data.message)-1);
        data.message[sizeof(data.message)-1] = '\0';
        
        CHECK(std::strlen(data.message) == 19);
    }
}

TEST_SUITE("Message Processing Logic") {
    TEST_CASE("MessageID Increment") {
        SharedData data1 = {0, 1, {'a'}};
        SharedData data2 = {1, 2, {'b'}};
        CHECK(data2.messageID == data1.messageID + 1);
    }

    TEST_CASE("WriterID Assignment") {
        SharedData data = {5, 3, {'m','s','g'}};
        CHECK(data.writerID >= 0);
    }
}

TEST_SUITE("File Operations") {
    TEST_CASE("Empty File Initialization") {
        SharedData emptyData = {-1, -1, {'a'}};
        CHECK(emptyData.messageID == -1);
        CHECK(emptyData.writerID == -1);
        CHECK(emptyData.message[0] == 'a');
    }
}

