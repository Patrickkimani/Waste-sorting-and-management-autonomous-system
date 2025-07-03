#include <Servo.h>

// Pin definitions
const int trigPin = 9;
const int echoPin = 10;
const int reedPin = 4;
const int irPin = 3;
const int relayPin = 8;

// Servo motor definitions
Servo servo1;  // First servo (magnetic detection)
Servo servo2;  // Second servo (ultrasonic detection)
Servo servo3;  // Third servo (IR detection)

// Servo pins
const int servo1Pin = 11;
const int servo2Pin = 5;
const int servo3Pin = 6;

// Threshold values
const int irThreshold = 500;  // Adjust based on your IR sensor
const int ultrasonicThreshold = 10;  // Distance in cm
const long debounceDelay = 50;  // Debounce time for reed switch

// Variables
int reedState;
int lastReedState = HIGH;
unsigned long lastDebounceTime = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(reedPin, INPUT_PULLUP);
  pinMode(irPin, INPUT);
  pinMode(relayPin, OUTPUT);
  
  // Attach servos
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);
  
  // Set initial positions
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  
  // Turn off conveyor initially
  digitalWrite(relayPin, LOW);
  
  Serial.println("System initialized and ready");
}

void loop() {
  // Start conveyor
  digitalWrite(relayPin, HIGH);
  
  // Phase 1: Magnetic Reed Sensor Detection
  int reading = digitalRead(reedPin);
  
  // Check for reed switch state change with debouncing
  if (reading != lastReedState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != reedState) {
      reedState = reading;
      
      if (reedState == LOW) {
        Serial.println("Magnetic object detected!");
        // Rotate first servo to 180 degrees
        servo1.write(180);
        delay(1000);  // Give time for object to be sorted
        servo1.write(0);
      }
    }
  }
  
  lastReedState = reading;
  
  // Phase 2: Ultrasonic Sensor Detection
  long duration, distance;
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert to cm
  
  if (distance < ultrasonicThreshold) {
    Serial.print("Non-metallic object detected at distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    // Rotate second servo to 180 degrees
    servo2.write(180);
    delay(1000);  // Give time for object to be sorted
    servo2.write(0);
    
    // Skip IR detection for this object to prevent double sorting
    delay(1000);  // Wait for object to pass IR sensor
  }
  
  // Phase 3: IR Proximity Sensor Detection
  int irValue = analogRead(irPin);
  
  if (irValue < irThreshold) {
    Serial.print("Non-metallic object detected by IR: ");
    Serial.println(irValue);
    
    // Rotate third servo to 180 degrees
    servo3.write(180);
    delay(1000);  // Give time for object to be sorted
    servo3.write(0);
  }
  
  // Small delay to prevent sensor interference
  delay(100);
}
