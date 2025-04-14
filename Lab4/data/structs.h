#pragma once

struct SharedData {
    int read_idx;
    int write_idx;
    char message[20];
};