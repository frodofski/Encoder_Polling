
#include "Encoder_Polling.h"

const int encoderPin_A = 8;
const int encoderPin_B = 9;

int counter = 0;

void setup()
{
  Serial.begin(9600);
  encoder_begin(encoderPin_A, encoderPin_B); // Start the decoder
}

void loop()
{
  int dir = encoder_data(); // Check for rotation
  
  if(dir == 1)       // If its forward...
  {
    counter++;       // Increment the counter
    Serial.println(counter);
  }
  
  else if(dir == -1) // If its backward...
  {
    counter--;       // Decrement the counter
    Serial.println(counter);
  }
}
