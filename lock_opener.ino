/*
 Nathan Luong and Erick Romero
 January 20, 2020 (Blue Monday)
 Logic for button inputs and motor controlling
 This code is written specifically for SparkFun's ProMicro
 Part Usage:
   2 Motor Drivers: L293D(9v) and A4988(9v)
   8-bit shift register: SN74HC595
   7-segments display.
   3 push buttons for input
   2 9v batteries and 5v power suply(power bank)
*/
//-------------------Variable assignment--------------------//
//Assign digital pin to the data line, output clock 
//and input clockof the 8-bit shift register
int dataLine = 7; // blue wire (digital pin 7)
int outputClock = 8;// yellow wire (digital pin 8)
int inputClock = 9; // red wire (digital pin 9)
//Assign digital output pins to the STEP 
//and Direction of the sterrper motor-driver
int stepPin = 10;
int dirPin = 16;
// Assign digital output pins to the L293D motor driver
// to control the lift-up motor
int um = 6; // Up-Motor

// Assign digital input pins for the buttons 
int b1 = 4;
int b2 = 3;
int b3 = 2;
// initiate counter at 0 for button holding mechanism
int counter = 0;

int r = A2;
int g = A1;
int b = A0;

// Create 5 bytes displaying the number 1, 2, 3
// the letter L and R  
byte one = B01100000;
byte two = B11011010;
byte three = B11110010;
byte L = B00011100;
byte R = B11001100;
// Create an array to store all 3 of them
int numList[3] = {one, two, three};
byte goLeftLed = B00000110;
// Create a variable to remeber the index of the lock
int lock = 0;
// boolean called choosing lock
bool choosingLock = 0;

void setup(){
  // Set up motors pins,leds and shift register pin as output
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(outputClock, OUTPUT);
  pinMode(dataLine, OUTPUT);
  pinMode(inputClock, OUTPUT);
  pinMode(um, OUTPUT);
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
  //Set up buttons pin as input
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  // display nothing at the beginning
  led_display(B00000000);
  //Set the random seed
  randomSeed(analogRead(A3));
  // Begin Serial communication at 9600 baud
  Serial.begin(9600);
}

void loop(){
  // Phase one : Aligning the lock to 0
  while(choosingLock == 0){
    // Check if button 1 is pressed
    if(digitalRead(b1)){
      // if True then switch to phase 2 (choosing Lock)
      choosingLock = 1;
      }
      // While button two is pressed
      // Use "while" to prevent pushing 2 buttons at once
    while(digitalRead(b2)){
      Serial.println("Going Left");
      //Display the letter L
      led_display(L);
      // spin left 5 steps (one number)
      spinLeft(5);
      rgb(0,0,0);
      //stop all motors for half a second
      stopM(500);
      }
      // While button is pressed
    while(digitalRead(b3)){
      Serial.println("Going Right");
      //Display the letter R
      led_display(R);
      // Spin right 5 steps (one number)
      spinRight(5);
      rgb(0,0,0);
      // stop motors for half a second
      stopM(500);
      }
  }
  // Phase 2: Choosing Locks 
  while(choosingLock){
    // While any button is pressed
    while(digitalRead(b1)||digitalRead(b2)||digitalRead(b3)){
      // if button 1 is pressed, change the variable of "lock" to 0
      if(digitalRead(b1)){lock = 0;}
      // if button 1 is pressed, change the variable of "lock" to 1
      if(digitalRead(b2)){lock = 1;}
      // if button 1 is pressed, change the variable of "lock" to 2
      if(digitalRead(b3)){lock = 2;}
      // Display the number 1, 2 or 3 depends on the value of lock 
      led_display(numList[lock]);
      // Icrease conuter when any button is hold down
      counter++;
      // If counter passes 3000, then display a red indicator
      if(counter > 3000){rgb(236, 77, 52);}
      // if any button is hold down long enough
      if(counter > 6000){
        // then solve the corresponding lock 
        switch(lock){
          case 0: lock1(); break;
          case 1: lock2(); break;
          case 2: lock3(); break;
        }
        // After solving the lock, lift the arm
        lift();
        // stop the program
        stopM(1000000000000);
      }
    }
    // Reset rgb light and counter to 0 when nothing is pressed
    rgb(0,0,0);
    counter = 0;
  }
}

void led_display(byte num){
  // Function to send a binary pause 
  // to the 8-bit shift register

  // First set the output clock LOW
  // to disable outputing current
    digitalWrite(outputClock, LOW);
    // loop through the given "num" byte
    for(int i = 0; i <=7 ; i++){
      //set the input clock to LOW
      // to enable taking in input pause
      digitalWrite(inputClock, LOW);
      // Send the shift register the "num" byte sequence
      digitalWrite(dataLine, bitRead(num, i));
      // Set inputClock back to HIGH to remember the input
      digitalWrite(inputClock, HIGH);
    }
    // Now set the outputCLock to HIGH
    // to output all of the 8 bits
    digitalWrite(outputClock, HIGH);
}

// A function to light up the rgb light
// with the actual rgb value
void rgb(int red, int green, int blue){
  // output the current analogly to the 3 channel
  analogWrite(r, red);
  analogWrite(g, green);
  analogWrite(b, blue);
}

// A function to spin Right the amount of step we want
void spinRight(int steps){
  // Set the direction of the stepper Motor to Right
  digitalWrite(dirPin, LOW);
  // A for loop to step the amount of steps we need
  for(int i = 0; i < steps; i++){
    // Step one full step
    digitalWrite(stepPin, HIGH);
    delay(1);
    // Set the step pin to Low for the next step
    digitalWrite(stepPin, LOW);
    delay(1);
  }
  // After spinning, randomly pick 3 value of red
  // blue and green for light show
  int rr = random(100, 256);
  int rg = random(100, 256);
  int rb = random(100, 256);
  rgb(rr, rg, rb); 
}

// Similar to spinRight
void spinLeft(int steps){
  // Set the direction for the motor to spin right
  digitalWrite(dirPin, HIGH);
  // step the amount of steps we want
  for(int i = 0; i < steps; i++){
    digitalWrite(stepPin, HIGH);
    delay(1);
    digitalWrite(stepPin, LOW);
    delay(1);
  }
  //random color for light show
  int rr = random(0, 256);
  int rg = random(0, 256);
  int rb = random(0, 256);
  rgb(rr, rg, rb);  
}

// A function to stop all motors 
// in a "waitT" amount of time
void stopM(float waitT){
  digitalWrite(dirPin, LOW);
  digitalWrite(stepPin,LOW);
  digitalWrite(um, LOW);
  delay(waitT);
}

// A function to solve lock one
void lock1(){ //0,30,20
  Serial.println("Doing Lock 1");
  // Make the light green
  rgb(136, 225, 69);
  spinLeft(600); // 3 rotations 
  spinLeft(0); // spin Left to 0
  spinRight(350); // spin right to 30
  spinLeft(50); // spin Left to 20
}

// A function to solve lock two
void lock2(){ // 32, 02, 16
  Serial.println("Doing Lock 2");
  // Make the light green
  rgb(136, 225, 69);
  spinLeft(600); // 3 rotations
  spinLeft(40); // spin Left to 32
  spinRight(250); // spin Right to 02
  spinLeft(130); // spin Left to 16
}

// A function to solve lock three
void lock3(){ // 02, 28, 06
  Serial.println("Doing Lock 3");
  // Make the light green
  rgb(136, 225, 69);
  spinLeft(600); // 3 rotations
  spinLeft(190); // spin Left to 02
  spinRight(330); // spin Right to 28
  spinLeft(110); // spin Left to 06
}

void lift(){
  Serial.println("Lifting");
  // Make the light Amber
  rgb(244, 209, 17);
  // Send signal to the transistor
  // to open-up the connection between motor and 9v battery
  digitalWrite(um, HIGH);
  delay(3000);
}
