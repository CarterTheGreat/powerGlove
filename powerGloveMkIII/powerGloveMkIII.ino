#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <nRF24L01.h>
#include <RF24.h>

/*
 * Power Glove Mk. III
 * Carter Watts
 * 
 * Uses: 
 *  433 MHz radio
 *  ADXL345 accel
 *
 * 433 MHz radio implemented
 * BT implemented
 * Accel implemented
 * 
 * Button support commented out
 */
 
//Running 
  String runningS;
  boolean runningB;

//Comm
  RF24 radio(7, 8); //CE, CSN
  const byte addresses[][6] = {00001, 00002};
  String data, dataIn;
  
//Accel
  ADXL345 adxl;
  boolean on;
  int x,y,z;
  String xS,yS,zS;


void setup() {
  
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(addresses[1]);// 00002
  radio.openReadingPipe(addresses[0]);// 00001
  radio.setPALevel(RF24_PA_MIN);
  
  adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);

  on = true;
  runningB = true;
}

void loop() {

  delay(5);
 
  radio.stopListening();
    //Running
      if(runningB){
        runningS = "1";
      }else runningS = "0";
    
    //Accel
      adxl.readXYZ(&x, &y, &z);
      double xyz[3];
      double ax,ay,az;
      adxl.getAcceleration(xyz);
      ax = xyz[0];
      ay = xyz[1];
      az = xyz[2];
  
      xS = String(x);
      yS = String(y);
      zS = String(z);

    //Flex

    //Collect
      data = "<";
      data.concat(runningS);
      data.concat("/");
      data.concat(xS);
      data.concat("/");
      data.concat(yS);
      data.concat("/");
      data.concat(zS);
      data.concat("/");
      data.concat(f1S);
      data.concat("/");
      data.concat(f1S);
      data.concat(">");

    //Send
      radio.write(&data, sizeof(data));


  delay(5);

  radio.startListening();
  while(!radio.avaliable());
  radio.read(&dataIn, sizeof(datain));
  Serial.println(dataIn);
   
}
