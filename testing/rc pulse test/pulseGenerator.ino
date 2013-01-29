/* This program works well to produce pulses of a specific duration.  Do not change
 *
 * Instructions:
 *  1. Connect ground of arduino to ground of circuit receiving the pulse
 *  2. Connect pin 10 of arduino to the input circuit
 *  3. Open the COM port.  Ensure that a new line character, "\n", is sent with each string
 *  4. Send the number of microseconds that the pulse duration should be.
 *
 */

#include <Servo.h>

#define SERVOPIN  10

Servo servoMain; // Define our Servo
uint16_t pulseWidthUs;

void setup()
{
  Serial.begin(9600);
  Serial.println("Enter the number of microseconds that the pulse width should be:");

  
  servoMain.attach(SERVOPIN,500,2900); // servo on digital pin 10
  pulseWidthUs = 1000;
}

void loop()
{
  if(Serial.available() > 0)
  {
    pulseWidthUs = Serial.parseInt();
    
    if (Serial.read() == '\n')
    {
      Serial.print("The number you entered is: ");
      Serial.print(pulseWidthUs);
      Serial.print("\n\n");
      Serial.println("Enter the number of microseconds that the pulse width should be:");
    }
  }
  
  servoMain.writeMicroseconds(pulseWidthUs);
}
