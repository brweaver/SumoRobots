#include <Wire.h>
#include <Zumo32U4.h>

/*
 * Class for managing robot 
 * 
 */


    
 class Robot {
     public: enum RobotState {
      rs_forward,
      rs_backward,
      rs_ramming,
      rs_nudgeleft,
      rs_nudgeright,
      rs_turnleft,
      rs_turnright,
      rs_deadstop,
    };

  public:
    Robot();

    static const unsigned long TURN_DELAY_90 = 250; //160?
    static const unsigned long TURN_DELAY_180 = 400;
    static constexpr float NUDGE_REDUCTION = 0.33;

    RobotState currentState;
    
    // Basic Operations
    void forward();
    void turnLeft();
    void turnRight();
    void turnLeft(unsigned long delay);
    void turnRight(unsigned long delay);
    void nudgeLeft();
    void nudgeRight();
    void deadStop();
    void taanabManeuver();
    void backupManeuver();
    void refresh();
    RobotState state();
    
  private:

    void resetDefaultDelay();
    void resetDelay(int);
    bool randomBool();
    
    static const uint16_t maxSpeed = 1024;
    static const unsigned long defaultDelay = 20; // ms
    
    int16_t speed = 400;

    Zumo32U4Motors motors;
    
    uint16_t period;
    unsigned long stateInitTime;
    unsigned long stateDelayTime;

    

    
//    typedef struct Movement {
//      RobotState state;
//      unsigned long delay;
//    } Movement;
};

