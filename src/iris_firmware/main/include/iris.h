#ifndef IRIS_H
#define IRIS_H

#define IRIS_FAIL(exit_label, errcode) \
    ret = errcode;                     \
    goto exit_label;

typedef enum iris_status {
    IRIS_OK = 0,
    IRIS_GENERAL_ERROR = -1,
    IRIS_OUT_OF_MEMORY = -2,
    IRIS_INVALID_ARGUMENT = -3,
    IRIS_OUT_OF_RANGE = -4,
    IRIS_INVALID_OPERATION = -5
} iris_status_t;

#endif //IRIS_H
