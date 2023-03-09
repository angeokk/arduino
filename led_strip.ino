#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define LED_PIN 3        // Pin connected to the LED strip
#define NUM_LEDS 5       // Number of LEDs in the strip

#define BUTTON_PIN 4     // Pin connected to the button

#define MAGN_SENS_PIN 2     // Pin connected to the button

#define SERVO_1_PIN 8
#define SERVO_2_PIN 9

#define LASER_PIN 10

//#define OUT 11

const int rotating_time = 300;
const int charging_time = 1500;
const int trigger_time = 3000;


class Magnet {
  private:
    unsigned long startMillisMagnet;  
    unsigned long currentMillisMagnet;
    bool magnetState;
    bool magnetWait;
    
  public:
    
    bool magnetRotating(){
      if(magnetWait){
        currentMillisMagnet = millis() - startMillisMagnet;
        if(currentMillisMagnet > rotating_time){
          currentMillisMagnet=0;
          magnetWait=0;   
        }
        return 1;
      }
      if(digitalRead(MAGN_SENS_PIN) != magnetState){
        startMillisMagnet = millis();
        magnetState = !magnetState;
        magnetWait=1;
        return 1;
      }
      return 0;
    }
};


class LoadingBar{
  private:
    Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
    Magnet magnet;
    
    //used while loading the bar
    bool full = false;  
    unsigned long startMillis;  
    unsigned long currentMillis;
    
    //used for the laser charges
    unsigned long startMillisLed;  
    unsigned long currentMillisLed;
    short triggers = 0;
    short rounds = 0;

  public:

    void stripSetup(){
      strip.begin();        // Initialize the LED strip
      strip.setBrightness(1);  
      strip.show();           
    }

    void setCol (int num, short g, short r, short b){      
      for(int i=0; i<num; i++){
        strip.setPixelColor(i, g, r, b);
      }  
    }

    void setCol2 (){
      if(currentMillis<charging_time){
        setCol(0, 0, 0, 0);
      }
      else if(currentMillis<2*charging_time){
        setCol(1, 0, 255, 0);
      }
      else if(currentMillis<3*charging_time){
        setCol(2, 0, 255, 0);
      }
      else if(currentMillis<4*charging_time){
        setCol(3, 128, 255, 0);
      }
      else if(currentMillis<5*charging_time){
        setCol(4, 128, 255, 0);
      }
      else{
        setCol(5, 255, 0, 0);
        full=true;
        triggers = 5;
        rounds = 5;
        fullSequence();
      }
    }

    void fullSequence(){
      currentMillis=0;
      for(int i=0; i<3; i++){
        setCol(NUM_LEDS, 0, 0, 0);
        strip.show();
        delay(100);
        setCol(NUM_LEDS, 255, 0, 0);
        strip.show();
        delay(100);                  
      }
    }

    void charging(){
      if(magnet.magnetRotating() && !full){
        startMillis = millis();
        setCol2();
        strip.show();
        currentMillis += millis() - startMillis; 
      }
    }

    void isCharged(){
      if(full){
        if(currentMillisLed>=trigger_time && rounds>triggers){
          rounds-=1;
          digitalWrite(LASER_PIN, LOW);
        }
        if(rounds==triggers && digitalRead(BUTTON_PIN)){
          startMillisLed = millis(); 
          triggers = triggers - 1;
          digitalWrite(LASER_PIN, HIGH);
          strip.clear();
          setCol(triggers, 255, 0, 0); 
          strip.show();
          currentMillisLed=0;
        }      
        if(triggers==0){
          full = false; 
        }
        currentMillisLed += millis() - startMillisLed;
        startMillisLed = millis();  
      }
    }
};

// unsigned long startMillisServo1;  
// unsigned long currentMillisServo1;
// bool turned1 = false;
// Servo servo1;
// Servo servo2;

LoadingBar lb; 


void setup() {
  Serial.begin(9600);
  lb.stripSetup();
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor
  pinMode(MAGN_SENS_PIN, INPUT);
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW);
}

void loop() {
  lb.charging();
  lb.isCharged();
}








