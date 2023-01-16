#ifndef STATE_H
#define STATE_H

struct RadioCtoR_struct {
    int velA;  // Velocidade do motor A
    int velB;  // Velocidade do motor B
};
typedef struct RadioCtoR_struct RadioCtoR;

struct RadioRtoC_struct {
    int unsigned odom1;
    int unsigned odom2;
};
typedef struct RadioRtoC_struct RadioRtoC;
extern RadioRtoC dataRobot;

struct Connection_struct {
    bool connected;
    int unsigned lostPackets;
    int unsigned latency;
    bool isGoodSignal;
};
typedef struct Connection_struct Connection;
extern Connection connection;

#endif