#include <DualVNH5019MotorShield.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>


/*
 * Mini Pod Code
 * V 0.2
 * 
 * Carter Watts
 * 
 * Notes:
 *  Added breaking TEST
 *  Tune comfortable hand gestures
 *  
 */
 
  int led = 19;
  
//Comm
  RF24 radio (7, 8); // CE, CSN
  RF24Network network(radio);
  const uint16_t master = 00;     //Octal format of nodes
  const uint16_t left_node = 01;
  const uint16_t right_node = 02;

//Motor
  unsigned char INA1 = 2;
  unsigned char INB1 = 4;
  unsigned char PWM1 = 9;
  unsigned char EN1DIAG1 = 6;
  unsigned char CS1 = A0;
  unsigned char INA2 = 3;
  unsigned char INB2 = 5;
  unsigned char PWM2 = 10;
  unsigned char EN2DIAG2 = 6;
  unsigned char CS2 = A1;
  DualVNH5019MotorShield md (INA1,
                           INB1,
                           PWM1,
                           EN1DIAG1,
                           CS1,
                           INA2,
                           INB2,
                           PWM2,
                           EN2DIAG2,
                           CS2);

//Vars

  struct payload_t {
    String data;
  };
  int startInd, ind1, ind2, ind3, ind4, ind5, endInd;
  int runningB = 0;
  int x, y, z, f1, f2;
  String runningS, xS ,yS, zS, f1S, f2S;
      int iter = 0;
      String L = "";
      String L5 = "";
      String L10 = "";
      String L15 = "";
      String L20 = "";
  //SET SIDE HERE FOR EVERY POD MADE-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-NOTICE-\-\-\-\-\-\-\-\-\-|
  const String LEFT = "left";
  const String RIGHT = "right";
  String side = LEFT; const uint16_t this_node = left_node;
  //String side = RIGHT;const uint16_t this_node = right_node;
  
void setup() {
  
  Serial.begin(115200);
  SPI.begin();
  
  pinMode(led, OUTPUT);
  
  //Radio
    radio.begin();
    network.begin(/*channel*/ 90, /*node address*/ this_node);
    
    delay(1000);
    
  //Motor Driver  
    md.init();
    Serial.println("Motor Driver Starting");

  //Startup display
    Serial.print(F("Started "));
    Serial.print(side);
    Serial.println(F(" pod"));
  
    delay(300);
    digitalWrite(led, HIGH);
    delay(300);
    digitalWrite(led, LOW);
    delay(300);
    digitalWrite(led, HIGH);
    delay(300);  
    digitalWrite(led, LOW);
    delay(300);
    
}

void loop(){

  md.init();
  digitalWrite(led, LOW);
  
  network.update();
    //Reading
    if(radio.available()){
      String dataIn = "";
      while(radio.available())
        radio.read(&dataIn, sizeof(dataIn));

      
      //Testing
      Serial.print("Recieved: ");
      Serial.println(dataIn);
      digitalWrite(led, HIGH);

      String data = String(dataIn);
  
      //Indexing
        ind1 = data.indexOf('/');
        ind2 = data.indexOf('/',ind1+1);
        ind3 = data.indexOf('/',ind2+1);
        ind4 = data.indexOf('/',ind3+1);
        ind5 = data.indexOf('/',ind4+1);
        endInd = data.indexOf('>');
  
      //String Data
        runningS = data.substring(startInd+1,ind1);
        xS = data.substring(ind1+1,ind2);
        yS = data.substring(ind2+1,ind3);
        zS = data.substring(ind3+1,ind4);
        f1S = data.substring(ind4+1,endInd);
        f2S = data.substring(ind5+1,endInd);
      
      //Int data
        runningB = runningS.toInt();
        x = xS.toInt();
        y = yS.toInt();
        z = zS.toInt();
        f1 = f1S.toInt();
        f2 = f2S.toInt();

      if(runningB && (L5 != L10 && L5 != L20 &&  L10 != L20  )){        
        //Control motors
          if(x < 40 && x > -40)
            x = 0;
          if(y < 40 && y > -40)
            y = 0; 
                                 
          //For left
          if(side == LEFT){
            int funct = -(.75*y) +x;
            //brakes
            if (funct < 30 && funct > -30){
              md.setBrakes(0,0);
            //Drive
            }else{
              int motorSpeed = map(funct, -370, 370, -400, 400);
              md.setSpeeds(-motorSpeed, -motorSpeed);
              stopIfFault();
              Serial.print(F("Speed set to: "));
              Serial.println(motorSpeed);
              Serial.print(F("Funct = "));
              Serial.println(funct);
            }
          }        
        
        //For right
        if(side == RIGHT){
          int funct = (.75*y)+x;
          //Brakes
          if (funct < 30 && funct > -30){
            md.setBrakes(0,0);
            //Drive
          }else{
            int motorSpeed = map(funct, -370, 370, -400, 400);
            md.setSpeeds(motorSpeed, motorSpeed);
            stopIfFault();
            Serial.print(F("Speed set to: "));
            Serial.println(motorSpeed);
            Serial.print(F("Funct = "));
            Serial.println(funct);
          }          
        }
      }
      /*
      if(iter % 5 == 0){
        L20 = L15;
        L15 = L10;
        L10 = L5;
        L5 = data;
      }
      
      L = data;
      iter++;
      */
      data = "<0/0/0/0/x/x/>";
    } 
}

void stopIfFault(){
  
  if (md.getM1Fault()){
    Serial.println("M1 fault");
    delay(10);
  }
  if (md.getM2Fault()){
    Serial.println("M2 fault");
    delay(10);
  }
  
}
