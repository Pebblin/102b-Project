#ifndef JoystickHandler_h
#define JoystickHandler_h

class MotorControl;

class JoystickHandler { // Class Declaration
  public:
    JoystickHandler(int xp, int yp); // Constructor

    void cardinalInput(MotorControl& MC) ; // Handles inputs for cardinal directions
    void diagonalInput(MotorControl& MC) ; // Handles inputs for diagonal directions

  private: // Pin storage
    int xpin;
    int ypin;
};

#endif
