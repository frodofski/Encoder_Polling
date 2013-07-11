
#include "Encoder_Polling.h"

const int encoderPin_A = 8;
const int encoderPin_B = 9;

void setup()
{
  Serial.begin(9600);
  encoder_begin(encoderPin_A, encoderPin_B); // Start the decoder
}

void loop()
{
  int dir = encoder_data(); // Check for rotation
  
  if(dir != 0)              // If it has rotated...
  {
    Serial.println(dir);   // Print the direction
  }  
}
