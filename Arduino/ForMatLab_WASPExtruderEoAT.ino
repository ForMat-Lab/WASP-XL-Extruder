// ForMat Lab
// Extruder End-of-Arm Tool (EoAT) Control
// Version: 1.01
// Date: December 2023
// Author: Ozguc Bertug Capunaman

// Motor configuration
const float gear_ratio = 57.0 / 11.0;                   // Defines the gear ratio of the motor (motor to extruder gear ratio)
const float steps_per_revolution = 4000.0 * gear_ratio;  // Calculates the total steps_per_revolution per revolution considering the gear ratio
const float step_angle = 360.0 / steps_per_revolution;  // Determines the step_angleolution of movement in degrees per step
const float RPM_SCALE = 10.0;                           // Scaling factor for interpreting RPM inputs

// Motor control variables
float T;      // Time delay for each step in microseconds
int raw_rpm;  // Raw revolutions per minute value as read from input pins
bool dir;     // Flag indicating the direction of rotation

int test;

void setup() {
  // Configure pin modes for ports D and B
  DDRD = B00000011;  // Set pins D0-D7 as inputs (for RPM and direction control)
  DDRB = B00111000;  // Set pins on port B as outputs (for motor control signals)

  // // ##### DEBUG ######
  // Serial.begin(9600);  // Initialize serial communication
  // Serial.println(gear_ratio, 10);  // Print gear ratio
  // Serial.println(steps_per_revolution, 10);       // Print calculated steps_per_revolution per revolution
  // Serial.println(step_angle, 10);         // Print the calculated step angle
  // Serial.end();                    // End serial communication
}

void loop() {
  // Read RPM value from digital input pins and apply scaling factor
  raw_rpm = ((PIND & B11111000) >> 3) | ((PINB & B00000011) << 5);

  // Determine motor rotation direction based on input pin
  dir = bool((PIND & B00000100) >> 2);

  // Prepare control signal for stepper driver
  int tmp = (dir << 4);  // Set or clear the direction bit
  // Rotate motor if RPM is greater than 0
  if (raw_rpm > 0) {
    T = setRPM(raw_rpm, step_angle, RPM_SCALE);  // Calculate time delay for steps_per_revolution based on RPM
    
    // // ##### DEBUG ######
    // if (test != raw_rpm) {
    //   Serial.println(raw_rpm);
    //   Serial.println(T);
    //   test = raw_rpm;
    // }

    tmp |= B00001000;      // Set step pin high to initiate a step
    PORTB = tmp;           // Send step signal to motor driver
    delayMicroseconds(T);  // Wait for half the step period
    tmp &= B11110111;      // Set step pin low to complete the step
    PORTB = tmp;           // Send signal to motor driver
    delayMicroseconds(T);  // Wait for the other half of the step period
  } else {
    // Stop rotation if RPM is 0
    PORTB = PINB | B00100000;
  }
}

// Function to calculate the time delay for steps_per_revolution based on RPM with scaling factor
long int setRPM(long raw_rpm, float step_angle, double scale) {
  float scaled_rpm = raw_rpm / scale;  // Apply scaling factor to raw RPM input

  // Calculate frequency based on scaled RPM and step per revolution
  float frequency = (scaled_rpm * steps_per_revolution) / 60.0;

  // Calculate and return the half-period in microseconds
  float period = 1000000.0 / frequency;
  period *= 0.5;
  return (long int)period;
}