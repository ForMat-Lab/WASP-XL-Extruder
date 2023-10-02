// Constants and Global Variables

// The gear ratio is calculated as the quotient of 57000000 and 11.
const unsigned long gearRatio = 57000000 / 11;

// Variables to store the computed values for steps per revolution and degrees per step
float stepsPerRevolution;
float degreesPerStep;
// Variables to store the pulse period in microseconds and the RPM of the motor
float pulsePeriodMicros;
int rpm;
// Variable to store the direction of the motor's rotation
bool direction;

// Temporary variable used for manipulation of bits in PORTB register
int temp;

// Setup Function
void setup() {
  // Configures the data direction register for PORTD and PORTB.
  // D0 and D1 are INPUT (Serial Communication), and D2, D3, B3, B4, and B5 are OUTPUT.
  DDRD = B00000011;
  DDRB = B00111000;

  // Calculating steps per revolution and degrees per step using the gear ratio.
  stepsPerRevolution = 800.0 * (gearRatio / 1000000.0);
  degreesPerStep = 360.0 / stepsPerRevolution;
}

// Loop Function
void loop() {
  // Reading RPM value by aggregating bits from PIND and PINB.
  rpm = (PIND & B11111100) >> 2;
  rpm |= (PINB & B00000001) << 6;

  // Calculating pulse period using RPM and degrees per step.
  pulsePeriodMicros = calculatePulsePeriod(rpm, degreesPerStep);

  // Determining the direction of the motor's rotation from PINB.
  direction = (PINB & B00000010) >> 1;

  // Setting up the initial value for the PORTB register based on the direction.
  temp = (direction << 4);

  if (rpm > 0) {
    // If RPM is positive, the motor should rotate, and PORTB is manipulated accordingly in a loop.
    for (int i = 0; i < stepsPerRevolution; i++) {
      temp |= B00001000;  // Set the 3rd bit to 1
      PORTB = temp;
      delayMicroseconds(pulsePeriodMicros);  // Wait for half the period of a pulse
      temp &= B11110111;  // Reset the 3rd bit to 0
      PORTB = temp;
      delayMicroseconds(pulsePeriodMicros);  // Wait for the other half of the pulse period
    }
    delay(1000);  // Wait for 1 second before the next iteration
  } else {
    // If RPM is not positive, set the 5th bit of PORTB to 1.
    PORTB = PINB | B00100000;
  }
}

// Function to calculate the pulse period in microseconds based on RPM and resolution.
float calculatePulsePeriod(long rpm, float res) {
  // Frequency is calculated using the formula: rpm / ((res / 360) * 60)
  float frequency = rpm / ((res / 360) * 60);
  // The function returns half of the period in microseconds, calculated as (1 / frequency) * 500000.
  return (1 / frequency) * 500000;
}
