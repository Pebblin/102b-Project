#ifndef MotorControl_h
#define MotorControl_h

class MotorControl { // Class Declaration
  public:
    MotorControl(int a1, int a2, int b1, int b2); // Constructor

    // TODO: Implement a way to move the carriage by x and y amount
    void right(int encoderTicks = 999); // Motion commands
    void left(int encoderTicks = 999);
    void up(int encoderTicks = 999);
    void down(int encoderTicks = 999);
    void upright();
    void upleft();
    void downright();
    void downleft();
    void off();

  private: 
    // Pin storage
    int ain1;
    int ain2;
    int bin1;
    int bin2;
};

#endif
