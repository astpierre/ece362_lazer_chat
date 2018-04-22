// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);           // start serial for output
}

void loop()
{
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  int cnt = 0;
 
  while (1 < Wire.available()) // loop through all but the last
  {
    //Serial.println(howMany);
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);  // print the character
    cnt = cnt + 1;
    //Serial.print("i");
  }
  //Serial.println(cnt);
  cnt = 0;
  int x = Wire.read();    // receive byte as an integer
  //Serial.println(x);         // print the integer
}

void requestEvent()
{
  Wire.write("success"); // respond with message of 6 bytes
  // as expected by master
}
