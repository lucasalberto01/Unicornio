#ifndef STATE_H
#define STATE_H

struct Communication_struct {
    int velA;
    int velB;
};
typedef struct Communication_struct Communication;

struct AckData_struct {
    int unsigned odom1;
    int unsigned odom2;
};
typedef struct AckData_struct AckData;

extern Communication comm;
extern AckData ackData;

#endif  // STATE_H