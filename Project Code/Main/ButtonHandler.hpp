#ifndef ButtonHandler_h
#define ButtonHandler_h

class ButtonHandler { // Class Declaration
  public:
    ButtonHandler(int bp); // Constructor

    bool detectRisingEdge(bool temp); // Detects if button has been pressed

  private: // Pin + prev state storage
    int buttonPin;
    bool lastState;
    unsigned long debounceTimer;
    int debounceDelay;
    bool storage;
  };

#endif