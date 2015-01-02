#include <Servo.h> 
 
// Motor drives
#define steeringMotorPWMPin 5
#define steeringMotorDirPin 8

#define driveMotorPWMPin 6
#define driveMotorDirPin 7

// Standard arduino status led, used as brake light
#define statusLedPin 13

// HC-SR04 Ping distance sensor
#define trigPin 13
#define echoPin 12

#define minDistance 20  // cm, obstacle
#define maxDistance 100// cm, gap

// head sensor is mounted on 2 servos for 360 degree freedom
#define headServo1Pin 11
#define headServo2Pin 10

#define servoPause 300
#define motorPause 500

#define headOffset 45 // degrees
#define driveSpeed 64 // 0-255 scale

// Head control
// 0 degree = head looking at right side
// ahead is 90 ddegree
// will look back by turning left
int headDirections[6] = {
  90+30, // forward left
  90-30, // straight ahead
  90,    // forward right
  270-30,// backward to left
  270,   // backward straight
  270+30 // backward to right
};

int currentHeadDirection = -1; // unknown at start
Servo headServo1;  // each servo moves on 180 degree
Servo headServo2;  // angle is splitted equaly to cover 360

void setup() {
  Serial.begin (9600);
  
  // Ping sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Head
  headServo1.attach(headServo1Pin);
  headServo2.attach(headServo2Pin);
  
  // Motors
  pinMode(steeringMotorPWMPin,OUTPUT);
  pinMode(steeringMotorDirPin,OUTPUT);
  pinMode(driveMotorPWMPin,OUTPUT);
  pinMode(driveMotorDirPin,OUTPUT);
  stopMotors();
  
  Serial.println("Ready!");
  
  // Ready! Do something crazy!
  driveRandomDirection();
}

// place the sensor head at one of the 6 predefined directions
void positionHead(int headDirection)
{
  int angle = headDirections[headDirection];
  angle += headOffset;
  angle %= 360;
  
  headServo1.write(angle/2);
  headServo2.write(angle/2);
  
  if(currentHeadDirection != headDirection)
    delay(servoPause);   // wait for servo to move
  currentHeadDirection = headDirection;
}

// Use the ping sensor to get distance in cm
long getDistance()
{
  long duration, distance;
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  
  Serial.print(distance);
  Serial.println(" cm");
  
 return distance;
}

void stopMotors()
{
  digitalWrite(steeringMotorPWMPin,LOW);
  digitalWrite(steeringMotorDirPin,LOW);
  digitalWrite(driveMotorPWMPin,LOW);
  digitalWrite(driveMotorDirPin,LOW);
  
  // Turn on the brake light
  digitalWrite(statusLedPin,HIGH);
  Serial.println("Stop!");
}

void driveCar(int headDirection)
{
  // Turn off the brake light
  digitalWrite(statusLedPin,LOW);
  
  switch(headDirection){
    case 0:
      Serial.println("Drive forward to left");
      digitalWrite(steeringMotorPWMPin,LOW);
      digitalWrite(steeringMotorDirPin,HIGH);
      analogWrite(driveMotorPWMPin,driveSpeed);
      digitalWrite(driveMotorDirPin,LOW);
      break;
    case 1:
      Serial.println("Drive forward straight ahead");
      digitalWrite(steeringMotorPWMPin,LOW);
      digitalWrite(steeringMotorDirPin,LOW);
      analogWrite(driveMotorPWMPin,driveSpeed);
      digitalWrite(driveMotorDirPin,LOW);
     break;
    case 2:
      Serial.println("Drive forward to right");
      digitalWrite(steeringMotorPWMPin,LOW);
      digitalWrite(steeringMotorDirPin,HIGH);
      analogWrite(driveMotorPWMPin,driveSpeed);
      digitalWrite(driveMotorDirPin,LOW);
      break;
    case 3:
      Serial.println("Drive backward to left");
      digitalWrite(steeringMotorPWMPin,HIGH);
      digitalWrite(steeringMotorDirPin,LOW);
      digitalWrite(driveMotorPWMPin,LOW);
      analogWrite(driveMotorDirPin,driveSpeed);
      break;
    case 4:
      Serial.println("Drive backward straight");
      digitalWrite(steeringMotorPWMPin,LOW);
      digitalWrite(steeringMotorDirPin,LOW);
      digitalWrite(driveMotorPWMPin,LOW);
      analogWrite(driveMotorDirPin,driveSpeed);
      break;
    case 5:
      Serial.println("Drive backward to right");
      digitalWrite(steeringMotorPWMPin,LOW);
      digitalWrite(steeringMotorDirPin,HIGH);
      digitalWrite(driveMotorPWMPin,LOW);
      analogWrite(driveMotorDirPin,driveSpeed);
      break;
    default:
      stopMotors();
  }
}

void driveRandomDirection()
{
  int headDirection = (currentHeadDirection+1)%6;//random(6);
  Serial.print("Heading direction = ");
  Serial.println(headDirection);
  positionHead(headDirection);

}

void loop() {
  int distance = getDistance();
  
  if(distance < minDistance || distance > maxDistance)
  {
    Serial.println("------------------------");
    stopMotors();
    delay(motorPause);
    
    // Try something!
    driveRandomDirection();
  }
  else {
      driveCar(currentHeadDirection);
  }
  Serial.print('.');

  delay(100);
}

