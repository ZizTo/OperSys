#pragma once

struct Employee {
    int num;
    char name[10];
    double hours;
};

enum MessageType {
    READ_REQUEST = 1,
    WRITE_REQUEST = 2,
    WRITE_REQUEST_READY = 3,
    BLOCK_RESPONSE = 4,
    SUCCESS = 5,
    SUCCESS_READ = 6,
    FAIL_READ = 7
};

struct Message {
    MessageType type;
    int employeeId = -1;
    Employee employee;
    int id;
};