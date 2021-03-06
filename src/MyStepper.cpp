#include "MyStepper.h"

MyStepper::MyStepper(int step, int direction, int maxSpeed, int acceleration, int stoppingMaxSpeed, int motorDirection)
    : Motor(motorDirection), Subscriber(), maxSpeed(maxSpeed), acceleration(acceleration), stoppingMaxSpeed(stoppingMaxSpeed),
    stepper(1, step, direction), goingToLimitA(true) {}

void MyStepper::addLimitSensors(DigitalInput* limitSoftStop, DigitalInput* limitAHardStop, DigitalInput* limitBHardStop) {
  this->limitSoftStop = limitSoftStop;
  this->limitAHardStop = limitAHardStop;
  this->limitBHardStop = limitBHardStop;
}

void MyStepper::setup() {
  this->stepper.setMaxSpeed(this->maxSpeed);
  this->stepper.setAcceleration(this->acceleration);
}

void MyStepper::doWriteWithAnalog(AnalogInput& input) {
  int rawVal = input.read(-512, 511);
  int val = abs(rawVal) > 100 ? rawVal : 0;
  if (val != 0) {
    int finalVal = this->direction * val;
    this->goingToLimitA = rawVal < 0;
    this->stepper.move(finalVal);
  } else {
    this->stepper.stop();
  }
}

void MyStepper::doWriteWithPosition(int position) {
  this->stepper.moveTo(position);
}

void MyStepper::doWriteWithRelativePosition(int position) {
  this->stepper.move(this->direction * position);
}

bool MyStepper::limitIsActive(DigitalInput* limit) {
  return (limit != 0 && limit->isPressed());
}

bool MyStepper::mustActivateHardLimit(DigitalInput* limit, bool mustGoToA) {
  return (this->limitIsActive(limit) && this->goingToLimitA == mustGoToA);
}

void MyStepper::updateStepper() {
  if (this->limitIsActive(this->limitSoftStop)) {
    this->stepper.setMaxSpeed(this->stoppingMaxSpeed);
  } else {
    this->stepper.setMaxSpeed(this->maxSpeed);
  }

  if (this->mustActivateHardLimit(this->limitAHardStop, true)
  || this->mustActivateHardLimit(this->limitBHardStop, false)) {
    this->stepper.stop();
    this->stepper.moveTo(this->stepper.currentPosition());
  } else {
    this->stepper.run();
  }
}

void MyStepper::update() {
  this->updateStepper();
  this->resetNotifications();
}

int MyStepper::getPosition() {
  return this->stepper.currentPosition();
}

bool MyStepper::hasHitLimit() {
  return (this->limitIsActive(this->limitAHardStop) || this->limitIsActive(this->limitBHardStop));
}
