#include <Wire.h>
#include <ADXL345.h>
#include <SoftwareSerial.h>
#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
//Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>
/*
 * Power Glove Mk. III
 * Carter Watts
 * 
 * Uses: 
 *  nRF24L01 - mesh
 *  ADXL345 accel
 * 
 * 
 * Accel implemented
 * Mesh to be implemented
 */
 
//Running 
  String runningS;
  boolean runningB;

//Comm 
  RF24 radio(7,8);
  RF24Network network(radio);
  RF24Mesh mesh(radio,network);
  struct payload_t {
    String data;
  };
  
//Accel
  ADXL345 adxl;
  boolean on;
  int x,y,z;
  String xS,yS,zS;

//Flex
  int f1, f2;
  String f1S, f2S;

void setup() {
  
  Serial.begin(115200);

  Serial.println(F("Glove starting"));
  
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

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  // Connect to the mesh
  mesh.begin();  

  Serial.println( F("Glove started"));
  
}

void loop() {
  mesh.begin();
  mesh.DHCP();
 
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

      f1S = "xxx";
      f2S = "xxx";
      
    //Collect
      String data = "<";
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

      sendData(data, 1);
      sendData(data, 2);
      //data.toCharArray(dataOut, 28);

    if(network.available())
      recieve();
  
}

void sendData(String data, int target){

    for (int i = 0; i < mesh.addrListTop; i++) {
      payload_t payload = {data};

      //WHAT IS HAPPENING HERE AND BELOW
      if (mesh.addrList[i].nodeID == target) {  //Searching for node one from address list
        payload = {data};
      }
      
      RF24NetworkHeader header(mesh.addrList[i].address, OCT); //Constructing a header
      Serial.println(network.write(header, &payload, sizeof(payload)) == target ? "Send OK" : "Send Fail"); //Sending an message
      
   }
}

String recieve(){
  String dataIn;
  
  RF24NetworkHeader header;
  network.peek(header);
  switch(header.type){
      // Display the incoming millis() values from the sensor nodes
    case 'M': 
      network.read(header,&dataIn,sizeof(dataIn));
      Serial.print(dataIn);
      Serial.print(" from RF24Network address 0");
      Serial.println(header.from_node, OCT);
      break;
    default: 
      network.read(header,0,0); 
      Serial.println(header.type);
      break;
    }
  
  return dataIn;
}
