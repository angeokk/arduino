#include <Servo.h>

#define SERVO_1_PIN 8
#define SERVO_2_PIN 9

#define BUTTON_PIN 4     

#define LED_PIN1 10
#define LED_PIN2 11

#define LASER_PIN 12
#define PHOTORESISTOR_PIN 1

const int chance = 80240;
const int turned_time = 2000;

const int off2 = 5;

const int volt_divider = 500; //0-1023 0-5


class SLP{ //servo,led,photoresistor
  private:
    Servo servo;
    short led_pin;
    short servo_pin;
    short pr_pin;
    int servoOff;
    unsigned long startMillisServo;  
    unsigned long currentMillisServo;
    bool turned = false;

  public:


    void setup(short ledPin, short servoPin, short prPin, int offset){
      servo_pin = servoPin;
      servo.attach(servoPin);
      servoOff = offset;
      servo.write(offset);
      led_pin = ledPin;
      pinMode(ledPin, OUTPUT);
      digitalWrite(ledPin, LOW);
      pr_pin = prPin;
      pinMode(prPin, INPUT);
    }

    void hitSequence(){
      for(int i=0; i<4; i++){
        servo.write(120+servoOff);
        digitalWrite(led_pin, LOW);
        delay(400);
        servo.write(60+servoOff);
        digitalWrite(led_pin, HIGH);
        delay(400);
      }
    }

    void isTurned(){
      if(!turned){
        if(random(chance)==0){
          Serial.println("enabled");
          startMillisServo = millis();
          turned = true;
          servo.write(servoOff+90);
          digitalWrite(led_pin, HIGH);
          currentMillisServo += millis() - startMillisServo;
          startMillisServo = millis();
        } 
      }
      else if(currentMillisServo<turned_time){
        currentMillisServo += millis() - startMillisServo;
        startMillisServo = millis();
      }
      else{
        currentMillisServo = 0;
        servo.write(servoOff+0);
        digitalWrite(led_pin, LOW);
        turned = false;   
      }
    }

    void isHit(){
      if(turned){
        if(analogRead(pr_pin)>volt_divider){
          hitSequence();
        }
      }
    } 
  
};


SLP slp;
Servo servo1;
Servo servo2;


void setup() {
  
  Serial.begin(9600);

  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);
  servo1.write(0);
  servo2.write(off2);
  
  //slp.setup(LED_PIN1, SERVO_2_PIN, PHOTORESISTOR_PIN, off2);
  
}

void loop() {

  slp.isTurned();
  slp.isHit();

}



