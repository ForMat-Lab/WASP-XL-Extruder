// ForMat Lab
// Extruder End-of-Arm Tool (EoAT) Control
// Version: 1.02
// Date: December 2023
// Author: Ozguc Bertug Capunaman

// Motor configuration
const double gear_ratio = 57.0 / 11.0;                    // Defines the gear ratio of the motor (motor to extruder gear ratio)
const double steps_per_revolution = 400.0 * gear_ratio;  // Calculates the total steps_per_revolution per revolution considering the gear ratio

unsigned int DO_RPM;           // Raw revolutions per minute value as read from input pins
const float RPM_SCALE = 10.0;  // Scaling factor for interpreting RPM inputs
const unsigned int RPM_SHIFT = 6;

// Motor control variables
unsigned int T_pulse = 50;  //Pulse width
unsigned long T_step;       // Time delay for each step in microseconds
bool dir;                   // Flag indicating the direction of rotation

unsigned int test;

void setup() {
  // Configure pin modes for ports D and B
  DDRD = B00000011;  // Set pins D0-D7 as inputs (for RPM and direction control)
  DDRB = B00111000;  // Set pins on port B as outputs (for motor control signals)

  // ##### DEBUG ######
  Serial.begin(9600);  // Initialize serial communication
  Serial.print("Gear Ratio:");
  Serial.println(gear_ratio);  // Print gear ratio
  Serial.print("Steps per Revolution:");
  Serial.println(steps_per_revolution);  // Print calculated steps_per_revolution per revolution
  // Serial.end();                    // End serial communication
}

void loop() {
  // Read RPM value from digital input pins and apply scaling factor
  DO_RPM = ((PIND & B11111000) >> 3) | ((PINB & B00000011) << 5);

  // Determine motor rotation direction based on input pin
  dir = bool((PIND & B00000100) >> 2);

  // Prepare control signal for stepper driver
  unsigned int tmp = (dir << 4);  // Set or clear the direction bit
  // Rotate motor if RPM is greater than 0
  if (DO_RPM > 0) {
    T_step = pulse_period(DO_RPM);  // Calculate time delay for steps_per_revolution based on RPM

    // ##### DEBUG ######
    if (test != DO_RPM) {
      Serial.println(DO_RPM);
      Serial.println(T_step);
      test = DO_RPM;
    }

    tmp |= B00001000;                     // Set step pin high to initiate a step
    PORTB = tmp;                          // Send step signal to motor driver
    delayMicroseconds(T_pulse);           // Wait for half the step period
    tmp &= B11110111;                     // Set step pin low to complete the step
    PORTB = tmp;                          // Send signal to motor driver
    delayMicroseconds(T_step - T_pulse);  // Wait for the other half of the step period
  } else {
    // Stop rotation if RPM is 0
    PORTB = PINB | B00100000;
  }
}

// Function to calculate the time delay for steps_per_revolution based on RPM with scaling factor
unsigned long pulse_period(int DO_RPM) {
  double scaled_rpm = (DO_RPM / RPM_SCALE) + RPM_SHIFT;  // Apply scaling factor to raw RPM input

  // Calculate frequency based on scaled RPM and steps per revolution
  double frequency = (scaled_rpm * steps_per_revolution) / 60.0;

  // Calculate the period for a complete revolution in microseconds
  double period = 1000000 / frequency;

  // Return the time delay for a single step as an integer
  return (unsigned long)period;
}