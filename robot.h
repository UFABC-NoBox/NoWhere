#include "_config.h"
#include "_types.h"

#ifndef ROBOT_H
#define ROBOT_H


class Robot{
public:


  // ====================================
  //           INITIALIZERS
  // ====================================

  static void init();


  // ====================================
  //   SPECIFIC CONFIGURATIONS (EEPROM)
  // ====================================

  static int getRobotID();


  // ====================================
  //         PRIMITIVE STATES
  // ====================================

  /*
    Boolean flag indicating Active stabilizing/Power on motors
  */
  static RobotState state;

  /*
    Last time when robot was Active
  */
  static unsigned long lastTimeActive;

  /*
    Current alarm of the robot (Errors)
  */
  static RobotAlarm alarm;

  /*
    Set's the state to the robot
  */
  static void setState(RobotState _state);

  /*
    Set's the current alarm of the robot, and put's robbot on Iddle
  */
  static void setAlarm(RobotAlarm _alarm);


  // ====================================
  //       BEEP and Voltage states
  // ====================================

  /*
    Battery voltage (in volts, ohhh!)
  */
  static float vbat;

  /*
    Beeper flag/setter
  */
  static int beepTimes;
  static int beepInterval;
  static BeepState beepState;

  static void setBeep(BeepState state);
  static void doBeep(int _times, int interval);


  // ====================================
  //         ATTITUDE/ORIENTATION
  // ====================================

  static Quaternion q;

  static VectorFloat gravity;

  static float ypr[3];

};

#endif