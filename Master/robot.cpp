#include <Arduino.h>
#include <EEPROM.h>
#include <Thread.h>

#include "_config.h"
#include "_types.h"

#include "robot.h"
#include "motors.h"
#include "system.h"

// ====================================
//         ATTITUDE/ORIENTATION
// ====================================

int16_t Robot::dx = 0;
int16_t Robot::dy = 0;
float Robot::theta = 0;
float Robot::linear = 0;
float Robot::angular = 0;

bool Robot::onFloor = false;
bool Robot::inclinated = false;
bool Robot::debug = false;

unsigned long Robot::lastTimeActive = 0;

void threadBeeper_run();
Thread threadBeeper(threadBeeper_run, 0);

// ====================================
//            INITIALIZATION
// ====================================
void Robot::init(){

  LOG("Robot::init\n");

  // Initialize Output Pins
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);

  pinMode(PIN_BUZZER, OUTPUT);

  // Initialize Input pins
  pinMode(PIN_BTN, INPUT);
  pinMode(PIN_VBAT, INPUT);

  // Setup State/Alarms
  Robot::setState(IDDLE);
  Robot::setAlarm(NONE);

  // Setup Beeper Thread
  controller.add(&threadBeeper);
  Robot::setBeep(BEEP_NONE);
};


// ====================================
//   SPECIFIC CONFIGURATIONS (EEPROM)
// ====================================

int _id = 0;
int Robot::getRobotID(){
  if(_id == 0){
    // Load from Eeprom if not loaded yet
    _id = EEPROM.read(EEPROM_ROBOT_ID);
  }
  return _id;
}

void Robot::setRobotID(int id){
  // Clear cache (Force re-reading)
  _id = 0;
  EEPROM.write(EEPROM_ROBOT_ID, id);
}

int _channel = 0;
int Robot::getChannel(){
  if(_channel == 0){
    // Load from Eeprom if not loaded yet
    _channel = EEPROM.read(EEPROM_ROBOT_ID);
  }
  return _channel;
}
void Robot::setChannel(int channel){
  // Clear cache (Force re-reading)
  _channel = 0;
  EEPROM.write(EEPROM_CHANNEL, channel);
}


// ====================================
//         PRIMITIVE STATES
// ====================================

RobotState Robot::state = IDDLE;
RobotAlarm Robot::alarm = NONE;

// Sets the robot's state
void Robot::setState(RobotState _state){
  state = _state;

  if(_state == ACTIVE)
    Robot::lastTimeActive = millis();
  else if(state == IDDLE)
    Motors::stop();
};

// Controls the robot alarm States
void Robot::setAlarm(RobotAlarm _alarm){
  Robot::alarm = _alarm;
  if(_alarm != NONE){
    LOG("Alarm set: "); LOG(_alarm); ENDL;
    threadBeeper.enabled = true;
    Robot::doBeep(5, 30, 5);
    Robot::setState(IDDLE);
  }else{
    LOG("Alarm clear\n");
  }
}

// ====================================
//       BEEP and Voltage states
// ====================================

float Robot::vbat = 0;

int Robot::beepTimes = 2;
int Robot::beepInterval = 50;
BeepState Robot::beepState = BEEP_NONE;

void Robot::setBeep(BeepState state){
  // Skip if already in that state
  if(Robot::beepState == state)
    return;

  Robot::beepState = state;
  threadBeeper.enabled = true;

  // LOG("Beep State: "); LOG(state); ENDL;
}

void Robot::doBeep(int _times, int interval, int _reason){
  Robot::beepTimes = _times;
  Robot::beepInterval = interval;

  // Make sure Thread will run
  threadBeeper.enabled = true;

  // LOG("Beep times: "); LOG(_times); LOG(" Reason: "); LOG(_reason); ENDL;
}

void threadBeeper_run(){
  static bool isBeeping = false;
  static bool lastIsBeeping = false;

  if(Robot::beepTimes > 0){
    // Decrease beeps at each beep
    if(!isBeeping)
      Robot::beepTimes--;
    isBeeping = !isBeeping;
    threadBeeper.setInterval(isBeeping ? 30 : Robot::beepInterval);
  }else if(Robot::beepState == BEEP_NONE){
    isBeeping = false;
    threadBeeper.enabled = false;
    threadBeeper.setInterval(0);
  }else if(Robot::beepState == WARN){
    isBeeping = !isBeeping;

    threadBeeper.setInterval(isBeeping ? 30 : 1000);
  }else if(Robot::beepState == ALARM){
    isBeeping = !isBeeping;

    threadBeeper.setInterval(isBeeping ? 30 : 200);
  }

  if(lastIsBeeping != isBeeping){
    digitalWrite(PIN_BUZZER, isBeeping);
    lastIsBeeping = isBeeping;
  }
}


