#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// switches:
#define MODEPIN   D4
#define PUSHPIN   D3
#define MOONPIN   D5
#define HEAVYPIN  D6
#define YESNOPIN  D7


// servo stuff:
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define MIN_PULSE_WIDTH       150
#define MAX_PULSE_WIDTH       450
#define FREQUENCY             50


bool stateMode;
bool statePush;
bool stateMoon;
bool stateHeavy;
bool stateYesNo;

void setup() {
  Serial.begin(9600);

  Serial.println("Setting up switches...");
  pinMode(D3, INPUT_PULLUP);
  pinMode(D4, INPUT_PULLUP);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);

  
  Serial.println("Starting PWM...");
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);  // Analog servos run at ~50 Hz updates
  delay(10);

  // random seed for selecting result
  randomSeed(analogRead(0));

  set_all_zero();
}


void loop() {
  // select mode
  stateMode = digitalRead(MODEPIN);
  if (stateMode) // oracle mode
  { 
    Serial.println("Oracle mode");
    statePush  = digitalRead(PUSHPIN);
    stateMoon  = digitalRead(MOONPIN);
    stateHeavy = digitalRead(HEAVYPIN);
    stateYesNo = digitalRead(YESNOPIN);
    if (!statePush) // button was pressed
    {
      Serial.println("Button was pressed");
    
      startTheShow(stateMoon, stateHeavy, stateYesNo);

    }
  } else { // weather mode
    Serial.println("Weather mode");
    Serial.println("not yet implemented");
    //delay(1000);
  }
  
  
  delay(50);
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void startTheShow(bool moon, bool heavy, bool yesNo) {
  int nRaise;  // number of time to raise the arms slowly
  int nRandom; // number of times to lift random arm
  int wait;    // millis to wait before raising the next arm
  if (heavy) {
    Serial.println("heavy is true -> nRandom set higher");
    nRandom = 19;
    wait = 1500;
    nRaise = 4;
  } else {
    nRandom = 12;
    wait = 500;
    nRaise = 2;
  }

  
  
  Serial.println("starting the show...");
  Serial.println("always start with rising the arms slowly!");
  for (int i = 0; i < nRaise; i++) {
    riseAllSlow(90);
  }

  Serial.println("Next, circle a little around, even more slowly when there is fullmoon!");
  for (int i = 0; i < 5; i++) {
    if (!moon) {
      circle(48, 8);
    } else {
      Serial.println("it is fullmoon");
      circle(96, 16);
    }
  }

  Serial.println("Now, rise once more and then do the oracle!");
  riseAllSlow(90);
  // now the real oracle!
  if (!yesNo) {
    Serial.println("only two options!");
    for (int i = 0; i < nRandom; i++) {
      raiseRandomTwo(wait);
    }
  } else {
    for (int i = 0; i < nRandom; i++) {
      raiseRandom(wait);
    }
  }
  
  
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////



int pulseWidthFromAngle(int angle)
{
  int pulseWidth;
  pulseWidth   = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  //Serial.println(pulseWidth);
  return pulseWidth;
}

void set_all_zero()
{
  for (int current_servo = 0; current_servo < 6; current_servo++) {
    pwm.setPWM(current_servo, 0, pulseWidthFromAngle(angleFromHorizontal(current_servo, 5)));
  }
  
}

int angleFromHorizontal (int servo, int angle)
// horizontal is the outwards pointing vector
{
  switch (servo) {
  case 0:
    // front right position
    return (180 - angle);
    break;
  case 1:
    // front left position
    return (angle);
    break;
  case 2:
    // middle right position
    return (180 - angle);
    break;
  case 3:
    // middle left position
    return (angle);
    break;
  case 4:
    // back right position
    return (angle);
    break;
  case 5:
    // back left position
    return (180 - angle);
    break;
  }
}


void riseAllSlow(int finalAngle)
{
  int angle = 0;
  angle = 0;
  for (int t = 0; t < 90; t++) {
    for (int current_servo = 0; current_servo < 6; current_servo++) {
      pwm.setPWM(current_servo, 0, pulseWidthFromAngle(angleFromHorizontal(current_servo, t)));
      //angle++;
      //Serial.println(t);
    }
    delay(50);
  }
  
}

int dsinus(int n, int M, int W)
{
  int sinus = int(90*sin(2*3.14 * (n+M)/W));
  //Serial.println(sinus);
  return int(map(sinus, -90, 90, 0, 90));
}

void circle(int N, int fac)
{
  for (int t = 0; t < N; t++) {
    for (int current_servo = 0; current_servo < 6; current_servo++) {
      int target = dsinus(t, current_servo*fac, N);
      //Serial.println(target);
      pwm.setPWM(current_servo, 0, pulseWidthFromAngle(angleFromHorizontal(current_servo, target)));

    }
    delay(50);
  }
  
}

void raiseRandom(int wait)
{
  int randNumber = random(6);
  Serial.println(randNumber);
  set_all_zero();
  raiseOne(randNumber);
  delay(wait);
}

void raiseRandomTwo(int wait)
{
  int randNumber = random(2);
  Serial.println(randNumber);
  set_all_zero();
  raiseOne(randNumber);
  delay(wait);
}

void raiseOne(int arm)
{
  pwm.setPWM(arm, 0, pulseWidthFromAngle(angleFromHorizontal(arm, 90)));
}
