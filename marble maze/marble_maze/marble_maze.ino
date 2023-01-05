#include <Servo.h>
#include <AFMotor.h>

String data[10];
int StringCount = 0;
float x = 0.5;
float y = 0.5;
//x rotation
Servo myservo;  // create servo object to control a servo
//y rotation
AF_Stepper stepper(360,1);   //360 step for one rotation, dc motor1&2
int prevoius_yRotation = 0;

unsigned long previousMotorTime = 0;
void setup() {
  Serial.begin(115200);

  myservo.attach(10);  // attaches the servo on pin 10 to the servo object 
  stepper.setSpeed(100);
  stepper.onestep(FORWARD, SINGLE);
  stepper.release();
  delay(1000);
}

void loop() {
  
  if(Serial.available() > 0){
    StringCount = 0;  //reset 
    String SerialData = Serial.readStringUntil('\n');   //get data
    while(SerialData.length() > 0){   // if still have data
      int index = SerialData.indexOf(' ');    //get firstly found seperating space
      if(index == -1){   //no more space found
        data[StringCount++] = SerialData;   //add the last segment
        break;
      } else {
        data[StringCount++] = SerialData.substring(0, index);   //add the segment to array
        SerialData = SerialData.substring(index+1);   //remove the added data        
      }
    }
    x = constrain(data[0].toFloat(),(float)0,(float)1);  //update
    y = constrain(data[1].toFloat(),(float)0,(float)1);  //update
    //Serial.print("X: ");  
    //Serial.print(x,4);
    //Serial.print(", Y: ");
    //Serial.print(y,4);
  }
  unsigned long nowTime = millis();  //get current time
  unsigned long Interval = nowTime - previousMotorTime;  //calc interval  
  if(Interval > 500){  // motor move every 0.5 second
    float x_rotation = x * 20;   //[0,20] default is 10
    x_rotation = (int)x_rotation;
    myservo.write(x_rotation);
    delay(50);
    int y_rotation = (int)(y * 100-50);   //[-10,10] default is 0
    int Require_rotation = y_rotation - prevoius_yRotation;
        if(Require_rotation > 0){
      stepper.step(Require_rotation, FORWARD, SINGLE);      
    } else if(Require_rotation < 0){
      Require_rotation = 0 - Require_rotation;  //make sure rotation is positive
      stepper.step(Require_rotation, BACKWARD, SINGLE);        
    }
    stepper.release();
    prevoius_yRotation = y_rotation; 
    previousMotorTime = nowTime; 
  }
}

