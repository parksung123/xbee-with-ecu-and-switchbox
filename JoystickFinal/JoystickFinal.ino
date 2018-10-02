// Joystick x- and y- axes are connected to A1 and A0 analog pins of Arduino.
// Servos are connectd to PWM Pins 9 and 10.

#include <Servo.h>

Servo right_servo, left_servo, leftr_servo, rightr_servo, horizon_servo, vertical_servo;

int joyX = A1; //  X - axis of Joystick
int joyY = A0; // y - axis of Joystick
int joyZ = A2; // z - axis of Joystick

float x, y, z, ele, roll, sR, sL, middle;
float sMax, sMin, deltaEle, diversRoll;

const int RunningAverageCount = 10;

float runningaveragebuffer1[RunningAverageCount];
float runningaveragebuffer2[RunningAverageCount];
float runningaveragebuffer3[RunningAverageCount];

int NextRunningAverage1;
int NextRunningAverage2;
int NextRunningAverage3;

int checkSwitch;
int pinSwitch = 4;  

void setup() {
  Serial.begin(9600);
  right_servo.attach(11); 
  left_servo.attach(10); 
  leftr_servo.attach(9);
  rightr_servo.attach(8);

  horizon_servo.attach(2);
  //vertical_servo.attach(3); 
  
  horizon_servo.write(95); // 40 <= x <= 150 center position: 95
  vertical_servo.write(150); // 50 <= x <= 140 default position: 150, center position: 95 
  pinMode(pinSwitch, INPUT);
  
  sMin = 900;
  sMax = 2100;
}

void loop() {
  checkSwitch = digitalRead(pinSwitch);
  
  if(checkSwitch == 0) {
    // Basic pre-determined movement
    for(int ver = 150; ver >= 135; ver--) {
      vertical_servo.write(ver); 
      delay(20);
    }
    delay(1000);
    
    for(int hor = 95; hor <= 150; hor++) {
      //Serial.println("Horizon: " + String(hor));
      horizon_servo.write(hor);
      delay(20);
    }
    delay(2000);
    
    for(int hor = 150; hor >= 40; hor--) {
      //Serial.println("Vertical: " + String(ver));
      horizon_servo.write(hor);
      if (hor <= 125)
        vertical_servo.write(hor+10); // 
      delay(20);
    }
    delay(3000);
    
    for(int ver = 40; ver <= 95; ver++) {
      //Serial.println("Horizon: " + String(hor));
      //horizon_servo.write(hor);
      vertical_servo.write(ver);
      delay(20);
    }
    delay(1500);
    
    for(int hor = 40; hor <= 95; hor++) {
      horizon_servo.write(hor);
      vertical_servo.write(hor+55);
      delay(20);
    }
    delay(1000);
  } 
 
  x = joyX;    // y value btwn 0 and 1023
  y = joyY;    // x value btwn 0 and 1023
  z = joyZ;
  x = map(analog1.getValue(), 0, 1023, sMin, sMax);
  y = map(analog2.getValue(), 0, 1023, sMin, sMax);
  z = map(analog3.getValue(), 0, 1023, sMin, sMax);
  
  // Mixer for X01
  middle = sMin + (sMax - sMin) / 2;

  //ele=y;
  roll = x;
  deltaEle = middle - y;
  sR = roll + deltaEle;
  diversRoll = sMax - (roll - sMin);
  sL = diversRoll + deltaEle;

  // Invert signal for Servos
  sL = sMax - (sL - sMin);
  
  /* Taking average of the next 10 data from the joystick */
  runningaveragebuffer1[NextRunningAverage1++] = sL;
  runningaveragebuffer2[NextRunningAverage2++] = sR;
  runningaveragebuffer3[NextRunningAverage3++] = z;
  
  if (NextRunningAverage1 >= RunningAverageCount)
    NextRunningAverage1 = 0;
 
  if (NextRunningAverage2 >= RunningAverageCount)
    NextRunningAverage2 = 0;
  
  if (NextRunningAverage3 >= RunningAverageCount)
    NextRunningAverage3 = 0;
  
  float RunningAverageLeft = 0;
  float RunningAverageRight = 0;
  float RunningAverageZ = 0;
  
  for (int i = 0; i < RunningAverageCount; ++i)
    RunningAverageLeft += runningaveragebuffer1[i];

  for (int i = 0; i < RunningAverageCount; ++i)
    RunningAverageRight += runningaveragebuffer2[i];
 
  for (int i = 0; i < RunningAverageCount; ++i)
    RunningAverageZ += runningaveragebuffer3[i];
 
  
  RunningAverageLeft /= RunningAverageCount;
  RunningAverageRight /= RunningAverageCount;
  RunningAverageZ /= RunningAverageCount;
  
  right_servo.write(RunningAverageRight);
  left_servo.write(RunningAverageLeft);
  
  leftr_servo.write(RunningAverageZ);
  rightr_servo.write(RunningAverageZ);      
}


