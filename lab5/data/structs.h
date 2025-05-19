struct Employee {
    int num;
    char name[10];
    double hours;
};

enum MessageType {
    READ_REQUEST = 1,
    WRITE_REQUEST = 2,
    BLOCK_RESPONSE = 3,
    SUCCESS_RESPONSE = 4,
};

struct Message {
    MessageType type;
    int employeeId = -1;
    Employee employee;
};