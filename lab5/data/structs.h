#pragma once

/*struct Employee {
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
};*/

struct Employee {
    int num; // идентификационный номер сотрудника [cite: 4]
    char name[10]; // имя сотрудника [cite: 4]
    double hours; // количество отработанных часов [cite: 4]
};

enum MessageType {
    READ_REQUEST = 1,
    WRITE_REQUEST = 2,
    WRITE_REQUEST_READY = 3,
    BLOCK_RESPONSE = 4,
    SUCCESS = 5,
    SUCCESS_READ = 6,
    FAIL_READ = 7
    // Consider adding:
    // CLIENT_EXITING_NOTIFICATION
};

struct Message {
    MessageType type;
    int employeeId = -1; // ID of the employee record being accessed
    Employee employee;
    int clientId = -1; // ID of the client sending the message
};