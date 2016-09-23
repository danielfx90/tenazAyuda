#include "MyStepper.h"

MyStepper::MyStepper(int step, int direction, int maxSpeed, int speed)
    : Motor(), maxSpeed(maxSpeed), speed(speed), stepper(1, step, direction) {}

void MyStepper::setup() {
  this->stepper.setMaxSpeed(this->maxSpeed);
  this->stepper.setSpeed(this->speed);
}

void MyStepper::doWriteWithAnalog(AnalogInput& input) {
  int rawVal = input.read(-100, 100);
  int finalVal = rawVal / 10;
  this->stepper.move(finalVal);
}

void MyStepper::doWriteWithPosition(int position) {
	this->stepper.moveTo(position);
}

void MyStepper::update() {
  this->stepper.runSpeed();
}