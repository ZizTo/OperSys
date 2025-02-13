#include <gtest/gtest.h>
#include "structs.h"
#include "Reporter.h"
#include "Creator.h"

TEST(LabTest, JustTest) {
    Creator *c = new Creator("test.txt");
    c->writeEmpl(1, "1", 2.3);
    c->writeEmpl(2, "2", 2);
    delete c;
    Reporter *r = new Reporter("test.txt", 2);
    r->getFromFile();
    r->writeOnFile("testtest.txt");
    
    std::vector<string> expected = {
        "Employee number 1: 1 work for 2.3 hours and get 4.6",
        "Employee number 2: 2 work for 2 hours and get 4"
    };
    EXPECT_EQ(r->getLines(), expected);
}

TEST(LabTest, JustTest2) {
    Creator* c = new Creator("test.txt");
    delete c;
    Reporter* r = new Reporter("test.txt", 2);
    r->getFromFile();
    r->writeOnFile("testtest.txt");

    std::vector<string> expected = {
    };
    EXPECT_EQ(r->getLines(), expected);
}

TEST(LabTest, JustTestZero) {
    Creator* c = new Creator("test.txt");
    c->writeEmpl(1, "1", 0);
    c->writeEmpl(2, "2", 0);
    delete c;
    Reporter* r = new Reporter("test.txt", 2);
    r->getFromFile();
    r->writeOnFile("testtest.txt");

    std::vector<string> expected = {
        "Employee number 1: 1 work for 0 hours and get 0",
        "Employee number 2: 2 work for 0 hours and get 0"
    };
    EXPECT_EQ(r->getLines(), expected);
}