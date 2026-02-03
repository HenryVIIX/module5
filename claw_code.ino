// APSC 101 Group C7 Arduino Code
// Fingers crossed this will work
#include <NewPing.h>
#include <Servo.h>

// Servo globals
Servo servo;
const int MAX_ANGLE = 180;
const int MIN_ANGLE = 0;
const int BALLOON_MIN = 90;
#define SERVO_PIN 9
bool stayClosed = false;
bool balloonTried = false;
const int SPEED = 2;
const int OPEN_DELAY = 1000;
const int CLOSE_DELAY = 1500;
const unsigned int MAX_CLOSE_TIME = 3000;

// Sonar globals
#define VCC_PIN 13
#define TRIGGER_PIN 12
#define ECHO_PIN 11 
#define GROUND_PIN 10
const int MAX_DISTANCE = 200;
const int TRIG_DIST = 15; // distance required for claw to actuate

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // initialize NewPing

void setup() {
  Serial.begin(9600); // set data transmission rate to/from computer to 9600
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(GROUND_PIN, OUTPUT); // tell pin 10 it is going to be an output
  pinMode(VCC_PIN, OUTPUT); // tell pin 13 it is going to be an output
  digitalWrite(GROUND_PIN, LOW); // tell pin 10 to output LOW (OV, or ground)
  digitalWrite(VCC_PIN, HIGH) ; // tell pin 13 to output HIGH (+5V)

  servo.attach(SERVO_PIN, 500, 2500);
  servo.write(MAX_ANGLE); // start the program with the servo open
  delay(500); // delay for half a second
}

int getAvgDist()
{
  int totalDist = 0;
  int samples = 3;

  for (int i = 0; i < samples; i++) {
    totalDist += sonar.ping_cm();
    delay(10);
  }

  return totalDist/samples;
}


// This function is obsolete for the competition now. Useful for testing tho ig
void showDistance(int distance)
{
  if (distance >= MAX_DISTANCE || distance <= 0) {
    Serial.println("Out of range");
  } else {
    Serial.print(distance);
    Serial.println(" cm");
  }
}

void closeClaw(Servo &servo, int closeSpeed, int closeAngle, int initAngle)
{
  unsigned long int startTime = millis();

  for (int i = initAngle; i >= closeAngle; i -= closeSpeed) {
    servo.write(i);
    delay(15);
    if (millis() - startTime > MAX_CLOSE_TIME) {
      // Assume we've hit an object
      break;
    }
  }
}

void openClaw(Servo &servo, int openSpeed, int openAngle, int initAngle)
{
  for (int i = initAngle; i <= openAngle; i += openSpeed) {
    servo.write(i);
    delay(15);
  }
}

void balloonFirst(Servo &servo, int distance)
{
  if (distance < TRIG_DIST && stayClosed == false) { // grabbing object
    delay(OPEN_DELAY);
    closeClaw(servo, SPEED, BALLOON_MIN, MAX_ANGLE);
    stayClosed = true;
    Serial.println("balloon close");
    delay(CLOSE_DELAY); // update delays as needed
  } else if (distance < TRIG_DIST && stayClosed == true) { // dropping object
    delay(OPEN_DELAY);
    openClaw(servo, SPEED, MAX_ANGLE, BALLOON_MIN);
    balloonTried = true;
    stayClosed = false;
    Serial.println("balloon open");
    delay(CLOSE_DELAY);
  } else if (stayClosed == false) { // claw is returning from dropping object
    servo.write(MAX_ANGLE);
  } else {
    servo.write(BALLOON_MIN); // claw is carrying object
  }
}

void varietyRound(Servo &servo, int distance) // slower close/open cycles
{
  if (distance < TRIG_DIST && stayClosed == false) { // grabbing object
    delay(OPEN_DELAY);
    closeClaw(servo, SPEED, MIN_ANGLE, MAX_ANGLE);
    Serial.println("regular close");
    stayClosed = true;
    delay(CLOSE_DELAY); // update delays as needed
  } else if (distance < TRIG_DIST && stayClosed == true) { // dropping object
    delay(OPEN_DELAY);
    openClaw(servo, SPEED, MAX_ANGLE, MIN_ANGLE);
    Serial.println("Regular open");
    stayClosed = false;
    delay(CLOSE_DELAY);
  } else if (stayClosed == false) { // claw is returning from dropping object
    servo.write(MAX_ANGLE);
  } else {
    servo.write(MIN_ANGLE); // claw is carrying object
  }
}

void bulkRound(Servo &servo, int distance) // faster servo closing and opening
{
  if (distance < TRIG_DIST && stayClosed == false) { // grabbing
    delay(OPEN_DELAY);
    servo.write(MIN_ANGLE);
    stayClosed = true;
    delay(CLOSE_DELAY);
  } else if (distance < TRIG_DIST && stayClosed == true) { // dropping
    delay(OPEN_DELAY);
    servo.write(MAX_ANGLE);
    stayClosed = false;
    delay(CLOSE_DELAY);
  } else if (stayClosed == false) { // coming back after drop
    servo.write(MAX_ANGLE);
  } else {
    servo.write(MIN_ANGLE); // carrying object
  }
}

void coopRound(Servo servo, int distance); // not sure how i'll write this yet. strategy tbd bc it relies on the rest of the teams



void loop() {
  int distance = getAvgDist();

  if (!balloonTried) {
    balloonFirst(servo, distance);
  } else {
    varietyRound(servo, distance);
  }

  // bulkRound(servo, distance);
  // coopRound(servo, distance);

  // useful for determining actuation distance
  showDistance(distance);
  // loop refresh time
  delay(500);
}
