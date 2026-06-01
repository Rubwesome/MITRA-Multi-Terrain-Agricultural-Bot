/*
 * 4-Driver Master Rover Code (Arduino Mega 2560)
 * * Controls a 4-wheeled rover using BTN7960 high-current drivers.
 * Includes serial control overrides, turning calibration, and 
 * software-level motor polarity correction.
 */

// --- Front Left (FL) Pins ---
// RPWM and LPWM are swapped here to fix reversed motor wiring
const int FL_RPWM = 3;  
const int FL_LPWM = 2;  
const int FL_REN  = 22; 
const int FL_LEN  = 23;

// --- Back Left (BL) Pins ---
// RPWM and LPWM are swapped here to fix reversed motor wiring
const int BL_RPWM = 5;  
const int BL_LPWM = 4;  
const int BL_REN  = 24; 
const int BL_LEN  = 25;

// --- Front Right (FR) Pins ---
const int FR_RPWM = 6; 
const int FR_LPWM = 7;
const int FR_REN  = 26; 
const int FR_LEN  = 27;

// --- Back Right (BR) Pins ---
const int BR_RPWM = 8; 
const int BR_LPWM = 9;
const int BR_REN  = 28; 
const int BR_LEN  = 29;

// --- Speed & Calibration Settings (0-255) ---
const int DRIVE_SPEED     = 110; 
const int LEFT_TURN_SPEED  = 130;  
const int RIGHT_TURN_SPEED = 120; 

void setup() {
  Serial.begin(9600);
  
  Serial.println("------------------------------------");
  Serial.println("    Mega Rover Online & Ready       ");
  Serial.println("------------------------------------");
  Serial.println("Controls: W=Forward, S=Reverse, A=Left, D=Right, Space/X=Stop\n");

  // Initialize Motor Driver Output Pins
  int outputPins[] = {
    FL_RPWM, FL_LPWM, FL_REN, FL_LEN,
    BL_RPWM, BL_LPWM, BL_REN, BL_LEN,
    FR_RPWM, FR_LPWM, FR_REN, FR_LEN,
    BR_RPWM, BR_LPWM, BR_REN, BR_LEN
  };

  for (int i = 0; i < 16; i++) {
    pinMode(outputPins[i], OUTPUT);
  }

  // Ensure rover is completely stopped and coasting on startup
  stopPWM();
  disableMotors(); 
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();

    // Skip line endings
    if (command == '\n' || command == '\r') return; 

    switch (command) {
      case 'W':
      case 'w':
        Serial.println("Moving Forward");
        enableMotors();
        driveAll(DRIVE_SPEED, DRIVE_SPEED, DRIVE_SPEED, DRIVE_SPEED);
        break;

      case 'S':
      case 's':
        Serial.println("Moving Reverse");
        enableMotors();
        driveAll(-DRIVE_SPEED, -DRIVE_SPEED, -DRIVE_SPEED, -DRIVE_SPEED);
        break;

      case 'A':
      case 'a':
        Serial.println("Turning Left");
        enableMotors();
        driveAll(-LEFT_TURN_SPEED, -LEFT_TURN_SPEED, RIGHT_TURN_SPEED, RIGHT_TURN_SPEED);
        break;

      case 'D':
      case 'd':
        Serial.println("Turning Right");
        enableMotors();
        driveAll(LEFT_TURN_SPEED, LEFT_TURN_SPEED, -RIGHT_TURN_SPEED, -RIGHT_TURN_SPEED);
        break;

      case 'X':
      case 'x':
      case ' ': 
        Serial.println("Stopping (Coasting)");
        stopPWM();
        disableMotors();
        break;

      default:
        // Ignore unrecognized characters without throwing spam errors
        break;
    }
  }
}

// Update speeds across all 4 motor drivers
void driveAll(int fl, int bl, int fr, int br) {
  setMotor(FL_RPWM, FL_LPWM, fl);
  setMotor(BL_RPWM, BL_LPWM, bl);
  setMotor(FR_RPWM, FR_LPWM, fr);
  setMotor(BR_RPWM, BR_LPWM, br);
}

// Low-level driver interface for individual BTN7960 channels
void setMotor(int rPwmPin, int lPwmPin, int speed) {
  if (speed > 0) {
    analogWrite(lPwmPin, 0);          
    analogWrite(rPwmPin, speed);      
  } 
  else if (speed < 0) {
    analogWrite(rPwmPin, 0);          
    analogWrite(lPwmPin, abs(speed));  
  } 
  else {
    analogWrite(rPwmPin, 0);          
    analogWrite(lPwmPin, 0);
  }
}

// Zero-out all PWM lines
void stopPWM() {
  driveAll(0, 0, 0, 0);
}

// Awake drivers and lock rotor torque
void enableMotors() {
  digitalWrite(FL_REN, HIGH); digitalWrite(FL_LEN, HIGH);
  digitalWrite(BL_REN, HIGH); digitalWrite(BL_LEN, HIGH);
  digitalWrite(FR_REN, HIGH); digitalWrite(FR_LEN, HIGH);
  digitalWrite(BR_REN, HIGH); digitalWrite(BR_LEN, HIGH);
}

// Cut driver enable lines to allow free wheel coasting
void disableMotors() {
  digitalWrite(FL_REN, LOW); digitalWrite(FL_LEN, LOW);
  digitalWrite(BL_REN, LOW); digitalWrite(BL_LEN, LOW);
  digitalWrite(FR_REN, LOW); digitalWrite(FR_LEN, LOW);
  digitalWrite(BR_REN, LOW); digitalWrite(BR_LEN, LOW);
}
