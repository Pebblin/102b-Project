#ifndef MotorControl_h
#define MotorControl_h

class MotorControl { // Class Declaration
  public:
    MotorControl(int a1, int a2, int b1, int b2); // Constructor

    void right(); // Motion commands
    void left();
    void up();
    void down();
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
