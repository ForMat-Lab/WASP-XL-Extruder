// ForMat Lab
// Extruder End-of-Arm Tool (EoAT) Control
// Version: 1.01
// Date: December 2023
// Author: Ozguc Bertug Capunaman

// Motor configuration
const double gear_ratio = 57.0 / 11.0;     // Defines the gear ratio of the motor (motor to extruder gear ratio)
const double steps = 400.0 * gear_ratio;   // Calculates the total steps per revolution considering the gear ratio
const double res = (double)360.0 / steps;  // Determines the resolution of movement in degrees per step
const double RPM_SCALE = 10.0;             // Scaling factor for interpreting RPM inputs

// Motor control variables
double T;     // Time delay for each step in microseconds
int raw_rpm;  // Raw revolutions per minute value as read from input pins
bool dir;     // Flag indicating the direction of rotation

void setup() {
  // Configure pin modes for ports D and B
  DDRD = B00000011;  // Set pins D0-D7 as inputs (for RPM and direction control)
  DDRB = B00111000;  // Set pins on port B as outputs (for motor control signals)

  // ##### DEBUG ######
  Serial.begin(9600);              // Initialize serial communication
  Serial.println(gear_ratio, 10);  // Print gear ratio
  Serial.println(steps, 10);       // Print calculated steps per revolution
  Serial.println(res, 10);         // Print the calculated resolution
  Serial.end();                    // End serial communication
}

void loop() {
  // Read RPM value from digital input pins and apply scaling factor
  raw_rpm = ((PIND & B11111100) >> 2) | ((PINB & B00000001) << 6);
  T = setRPM(raw_rpm, res, RPM_SCALE);  // Calculate time delay for steps based on RPM

  // Determine motor rotation direction based on input pin
  dir = bool((PINB & B00000010) >> 1);

  // Prepare control signal for stepper driver
  int tmp = (dir << 4);  // Set or clear the direction bit

  // Rotate motor if RPM is greater than 0
  if (raw_rpm > 0) {
    for (int i = 0; i < steps; i++) {
      tmp |= B00001000;      // Set step pin high to initiate a step
      PORTB = tmp;           // Send step signal to motor driver
      delayMicroseconds(T);  // Wait for half the step period
      tmp &= B11110111;      // Set step pin low to complete the step
      PORTB = tmp;           // Send signal to motor driver
      delayMicroseconds(T);  // Wait for the other half of the step period
    }
  } else {
    // Stop rotation if RPM is 0
    PORTB = PINB | B00100000;
  }
}

// Function to calculate the time delay for steps based on RPM with scaling factor
double setRPM(long raw_rpm, float res, double scale) {
  double scaledRPM = raw_rpm / scale;  // Apply scaling factor to raw RPM input

  // Calculate frequency based on scaled RPM and resolution
  double freq = scaled_rpm / ((res / 360) * 60);

  // Calculate and return the half-period in microseconds
  double period = 1.0 / freq;
  return (period / 2.0) * 1000000;
}