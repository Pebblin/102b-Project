#ifndef EncoderHandler
#define EncoderHandler

int encoderCount1 = 0;          // Raw encoder count for motor 1 (A)
int lastAAState = 0;            // Previous state of AA

int encoderCount2 = 0;          // Raw encoder count for motor 2 (A)
int lastABState = 0;            // Previous state of AB

void handleEncoder1();

void handleEncoder2();

void resetEncoders();

#endif
