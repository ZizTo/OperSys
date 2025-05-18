struct Employee {
    int num;
    char name[10];
    double hours;
};

enum MessageType {
    READ_REQUEST = 1,
    WRITE_REQUEST = 2,
    READ_RESPONSE = 3,
    WRITE_RESPONSE = 4,
    UNLOCK_REQUEST = 5,
    SUCCESS_RESPONSE = 6,
    ERROR_RESPONSE = 7
};

struct Message {
    MessageType type;
    int employeeId;
    Employee employee;
};

enum LockState {
    UNLOCKED = 0,
    READ_LOCKED = 1,
    WRITE_LOCKED = 2
};

struct RecordLock {
    int employeeId;
    LockState state;
    int readCount;
    HANDLE mutex;
};
