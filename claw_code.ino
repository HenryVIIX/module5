// APSC 101 Group C7 Arduino Code
// Fingers crossed this will work
#include <NewPing.h>
#include <Servo.h>

// Servo globals
Servo servo;
const int MAX_ANGLE = 180;
const int MIN_ANGLE = 0;
#define SERVO_PIN 9
bool stayClosed = false;
const int SPEED = 1;

// Sonar globals
#define VCC_PIN 13
#define TRIGGER_PIN 12
#define ECHO_PIN 11 
#define GROUND_PIN 10
const int MAX_DISTANCE = 200;
const int TRIG_DIST = 10; // distance required for claw to actuate

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // initialize NewPing

void setup() {
  Serial.begin(9600); // set data transmission rate to/from computer to 9600
  pinMode(ECHO_PIN, INPUT) ;
  pinMode(TRIGGER_PIN, OUTPUT) ;
  pinMode(GROUND_PIN, OUTPUT); // tell pin 10 it is going to be an output
  pinMode(VCC_PIN, OUTPUT); // tell pin 13 it is going to be an output
  digitalWrite(GROUND_PIN, LOW); // tell pin 10 to output LOW (OV, or ground)
  digitalWrite(VCC_PIN, HIGH) ; // tell pin 13 to output HIGH (+5V)

  servo.attach(SERVO_PIN, 500, 2500);
  servo.write(MAX_ANGLE); // start the program with the servo open
  delay(500); // delay for half a second
}

void showDistance(int distance)
{
  if (distance >= MAX_DISTANCE || distance <= 0) {
    Serial.println("Out of range");
  } else {
    Serial.print(distance);
    Serial.println(" cm");
  }
}

void closeClaw(Servo servo, int closeSpeed, int closeAngle, int initAngle)
{
  for (int i = initAngle; i >= closeAngle; i -= closeSpeed) {
    servo.write(i);
    delay(15);
  }
}

void openClaw(Servo servo, int openSpeed, int openAngle, int initAngle)
{
  for (int i = initAngle; i <= openAngle; i += openSpeed) {
    servo.write(i);
    delay(15);
  }
}

void varietyRound(Servo servo, int distance) // slower close/open cycles
{
  if (distance < TRIG_DIST && stayClosed == false) { // grabbing object
    delay(1000);
    closeClaw(servo, SPEED, MIN_ANGLE, MAX_ANGLE);
    delay(500);
    stayClosed = true;
    delay(1000); // update delays as needed
  } else if (distance < TRIG_DIST && stayClosed == true) { // dropping object
    delay(1000);
    openClaw(servo, SPEED, MAX_ANGLE, MIN_ANGLE);
    delay(500);
    stayClosed = false;
    delay(1000);
  } else if (stayClosed == false) { // claw is returning from dropping object
    servo.write(MAX_ANGLE);
  } else {
    servo.write(MIN_ANGLE); // claw is carrying object
  }
}

void bulkRound(Servo servo, int distance) // faster servo closing and opening
{
  if (distance < TRIG_DIST && stayClosed == false) { // grabbing
    delay(1000);
    servo.write(MIN_ANGLE);
    delay(500);
    stayClosed = true;
    delay(1000);
  } else if (distance < TRIG_DIST && stayClosed == true) { // dropping
    delay(1000);
    servo.write(MAX_ANGLE);
    delay(500);
    stayClosed = false;
    delay(1000);
  } else if (stayClosed == false) { // coming back after drop
    servo.write(MAX_ANGLE);
  } else {
    servo.write(MIN_ANGLE); // carrying object
  }
}

void coopRound(Servo servo, int distance); // not sure how i'll write this yet. strategy tbd bc it relies on the rest of the teams



void loop() {
  int distance = sonar.ping_cm();
  varietyRound(servo, distance);
  // bulkRound(servo, distance);
  // coopRound(servo, distance);

  showDistance(distance);
  // loop refresh time
  delay(500);
}
