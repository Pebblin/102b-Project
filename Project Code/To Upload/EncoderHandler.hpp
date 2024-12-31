#ifndef EncoderHandler_h
#define EncoderHandler_h

class EncoderHandler { // Class Declaration
  public:
    EncoderHandler(int AAPin, int BAPin, int ABPin, int BBPin); // Constructor

    void handleEncoder1();      // Handle encoder 1
    void handleEncoder2();      // Handle encoder 2
    void resetEncoders();       // Zero out encoder values

    int returnEncoder1();       // Return encoder 1 tick value
    int returnEncoder2();       // Return encoder 2 tick value

    int returnRotation1();      // Return encoder 1 rotation value
    int returnRotation2();      // Return encoder 2 rotation value

  private: // Pin + prev state storage
    int encoderCount1;          // Raw encoder count for motor 1 (A)
    int lastAAState;            // Previous state of AA

    int encoderCount2;          // Raw encoder count for motor 2 (A)
    int lastABState;            // Previous state of AB

    int encoderAAPin;           // Encoder Pins
    int encoderBAPin;
    int encoderABPin;
    int encoderBBPin;

    int cpr;                    // 700 encoder ticks per revolution for our motor
};

#endif
