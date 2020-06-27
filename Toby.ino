#include <VarSpeedServo.h>

// motor settings
int minPwm = 120;
int maxPwm = 255;

// motor one
int enA = 7;
int in1 = 13;
int in2 = 14;
int motor1Val = -1;
double spdF1 = 1.0;

// motor two
int enB = 8;
int in3 = 15;
int in4 = 16;
int motor2Val = -1;
double spdF2 = 1.0;

// bluetooth
String btData = "";

// servos
const int numGroups = 3;
const int servosPerGroup = 2;
int servoPins[numGroups][servosPerGroup] = {{11, 10}, {9, 12}, {6, 5}};
int initVals[numGroups][servosPerGroup] = {{0, 15}, {180, 165}, {40, 30}};
VarSpeedServo servos[numGroups][servosPerGroup];
int iceCreamServoPin = 53;
VarSpeedServo iceCreamServo;

const int minDelay = 0;
const int maxDelay = 100;
const int servoStep = 5;

// leds
int btLedPin = 22;
int cmdLedPin = 23;
int listLedPin = 24;

// program control flow
String command = "none";

void setup() {
  // serial
  Serial.begin(9600);
  Serial.println("INIT");

  // bluetooth
  Serial1.begin(9600);
  
  // motors
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // servos
  for (int i = 0; i < numGroups; i++) {
    for (int j = 0; j < servosPerGroup; j++) {
      servos[i][j].write(initVals[i][j]);
      servos[i][j].attach(servoPins[i][j]);
    }
  }

  iceCreamServo.write(10);
  iceCreamServo.attach(iceCreamServoPin);

  // leds
  pinMode(btLedPin, OUTPUT);
  digitalWrite(btLedPin, LOW);
  pinMode(cmdLedPin, OUTPUT);
  digitalWrite(cmdLedPin, LOW);
  pinMode(listLedPin, OUTPUT);
  digitalWrite(listLedPin, LOW);
}

void loop() {
  // read bluetooth serial
  if (Serial1.available())  {
    digitalWrite(btLedPin, HIGH);
    
    // add characters from serial to bluetooth data
    char c = Serial1.read();
    btData += c;

    // when ending bracket received, data packet is complete
    if (c == '>') {
      // extract bluetooth data information: command id and command specifics
      String data = btData.substring(1, btData.length() - 1);
      String id = data.substring(0, data.indexOf(" "));
      String specs = data.substring(data.indexOf(" ") + 1, data.length());
      
      Serial.println(btData);

      // run respective commands
      if (id.equals("listening")) {
        digitalWrite(listLedPin, HIGH);
      } else if (id.equals("speaking")) {
        digitalWrite(listLedPin, LOW);
        
        if (specs.equals("exit_command")) {
          // reset based on exited command
          if (command.equals("follow")) {
            lowerArms();
          }
          
          command = "none";
          digitalWrite(cmdLedPin, LOW);
        } else {
          digitalWrite(cmdLedPin, HIGH);
          
          // set command to received command
          if (specs.equals("greet")) {
            command = "greet";
            leftWave();
          } else if (specs.equals("intro")) {
            digitalWrite(btLedPin, HIGH);
            digitalWrite(cmdLedPin, HIGH);
            digitalWrite(listLedPin, HIGH);
            servos[2][0].write(initVals[2][0], 50);
            servos[2][1].write(initVals[2][1], 50);

//          liftArms();
            delay(3000);
            digitalWrite(btLedPin, LOW);
            digitalWrite(cmdLedPin, LOW);
            digitalWrite(listLedPin, LOW);

            servos[2][0].write(0, 50);

//            runMotorOne(120);
//            runMotorTwo(120);
//            delay(3000);
//            runMotorOne(0);
//            runMotorTwo(0);
//            delay(500);
//            leftWave();
//            delay(1000);
//            servos[2][0].write(70, 30);
//            delay(3000);
//            servos[2][0].write(initVals[2][0], 30);
          } else if (specs.equals("ice")) {
            command = "ice";
            delay(1500);
            talkGest(4);  
          } else if (specs.equals("smile")) {
            command = "smile";
            iceCreamServo.write(10, 120);
          } else if (specs.equals("talk")) {
            command = "talk";
          } else if (specs.equals("ap")) {
            command = "ap";
            talkGest(4);
            delay(2000);
            talkGest(1);
            delay(2500);
            talkGest(2);
            delay(1000);
            talkGest(3);
            delay(5500);
            talkGest(0);
          } else if (specs.equals("chill")) {
            servos[2][0].write(initVals[2][0]);
            command = "chill";
          } else if (specs.equals("party")) {
            servos[2][0].write(initVals[2][0]);
            command = "party";
            delay(16000);
            int beatInterval = 472;

            ymca(beatInterval);
          } else if (specs.equals("follow")) {
            command = "follow";
            liftArms();
          } else if (specs.equals("dab")) {
            command = "dab";
            dab();  
          } else if (specs.equals("meaning_of_life")) {
            command = "meaning";
            talkGest(random(0, 5));
            delay(1000);
            talkGest(random(0, 5));
          } else if (specs.equals("alive1") || specs.equals("alive2") || specs.equals("alive3") || specs.equals("alive4")) {
            servos[2][0].write(initVals[2][0]);
            command = "alive";
            int beatInterval = 583;
            int part = specs.substring(specs.length() - 1, specs.length()).toInt();

            alive(beatInterval, part);
          }
        }
      }

      // complete action based on received bluetooth message id
      if (id.equals("face_tracking")) {
        int moveX = specs.substring(0, specs.indexOf(" ")).toInt();
        int moveY = specs.substring(specs.indexOf(" ") + 1, specs.length()).toInt();

        headTrackX(moveX);
        headTrackY(moveY);
      } else if (id.equals("cat_tracking")) {
        int moveX = specs.substring(0, specs.indexOf(" ")).toInt();
        int moveY = specs.substring(specs.indexOf(" ") + 1, specs.lastIndexOf(" ")).toInt();
        int moveZ = specs.substring(specs.lastIndexOf(" ") + 1, specs.length()).toInt();

        trackObj(moveX, moveY, moveZ);
      }

      // command specific actions
      if (command.equals("talk")) {
        if (id.equals("speaking")) {
          if (specs.equals("hello")) {
            leftWave();
          } else if (specs.equals("goodbye")) {
            rightWave();
          } else {
            talkGest(random(0, 5));
          }
        }
      } else if (command.equals("smile")) {
        if (id.equals("speaking")) {
          if (specs.equals("smile_intro")) {
            leftWave();
            talkGest(1);
          } else if (specs.equals("smile_frown")) {
            talkGest(4);
          } else if (specs.equals("smile_bye")) {
            talkGest(0);
            delay(3000);
            talkGest(4);
            delay(500);
            iceCreamServo.write(90, 120);
            delay(1200);
            rightWave();
          }
        }
      }

      btData = "";
    }
  } else {
    digitalWrite(btLedPin, LOW);
  }

  // continuous dancing for dancing commands
  if (command.equals("chill")) {
      bobHead(666);
  }
}

// motor control
// map motor value
int mapMotorVal(int val) {
  int sign = val/abs(val);
  int newVal = map(abs(val), 0, 255, minPwm, maxPwm);

  if (newVal < minPwm + 5) {
    newVal = 0;
  }

  return sign * newVal;
}

// motor control methods
void runMotorOne(int speed) {
  // adjust speed based on speed adjustment factor
  int newSpeed = spdF1 * mapMotorVal(speed);

  // control motor direction
  if (speed < 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if (speed > 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }

  // set motor speed
  analogWrite(enA, abs(newSpeed));
}

void runMotorTwo(int speed) {
  // adjust speed based on speed adjustment factor
  int newSpeed = spdF2 * mapMotorVal(speed);
  
  // control motor direction
  if (speed < 0) {
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } else if (speed > 0) {
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } else {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
  
  // set motor speed
  analogWrite(enB, abs(newSpeed));
}

void headTrackX(int moveX) {
  int speed = 15;
  int step = 3;
  int deg = servos[2][0].read() + moveX * step;

  if (deg > 180) {
    deg = 180;
  } else if (deg < 0) {
    deg = 0;
  }
  
  servos[2][0].write(deg, speed);
}


void headTrackY(int moveY) {
  int speed = 15;
  int step = 2;
  int deg = servos[2][1].read() - moveY * step;
  int minY = 20;
  int maxY = 40;

  if (deg > maxY) {
    deg = maxY;
  } else if (deg < minY) {
    deg = minY;
  }
  
  servos[2][1].write(deg, speed);
}

void trackObj(int moveX, int moveY, int moveZ) {
  int speedX = 20;
    digitalWrite(listLedPin, LOW);

  if (moveX == 0) {
    int maxPwm = 100;
    int speedZ = map(moveZ, 0, 9, 0, (maxPwm * 4)/4);
    digitalWrite(listLedPin, HIGH);
    runMotorOne(speedZ);
    runMotorTwo(speedZ);
  } else {
    /*if (moveX < 0) {
      runMotorOne(speedX);
    } else if (moveX > 0) {
      runMotorTwo(speedX);
    }*/
  }
}

// servo gestures
void leftWave() {
  int speed = 100;
  
  for (int i = 0; i < 3; i++) {
    servos[1][0].write(135, speed, true);
    servos[1][0].write(45, speed, true);
  }

  servos[1][0].write(initVals[1][0], 50, true);
}

void rightWave() {
  int speed = 100;
  
  for (int i = 0; i < 3; i++) {
    servos[0][0].write(45, speed, true);
    servos[0][0].write(135, speed, true);
  }

  servos[0][0].write(initVals[0][0], 50, true);
}

void bothWave() {
  int speed = 100;
  
  for (int i = 0; i < 3; i++) {
    servos[0][0].write(45, speed);
    servos[1][0].write(135, speed);
    servos[0][0].wait();
    servos[1][0].wait();
    servos[0][0].write(135, speed);
    servos[1][0].write(45, speed);
    servos[0][0].wait();
    servos[1][0].wait();
  }

  servos[0][0].write(initVals[0][0], 50);
  servos[1][0].write(initVals[1][0], 50);
}

void nodHead() {
  int speed = 20;

  servos[2][1].write(20, speed, true);
  servos[2][1].write(initVals[2][1], speed, true);
}

void bobHead(int interval) {
  int speed = 50;
  servos[2][1].write(50, speed);
  delay(interval/3);
  servos[2][1].write(initVals[2][1], speed/2);
  delay((interval * 2)/3);
}

void bobLeftArm(int interval) {
  int speed = 80;
  
  servos[1][1].write(60, speed);
  delay(interval/3);
  servos[1][1].write(100, speed/2);
  delay((interval * 2)/3);
}

void bobRightArm(int interval) {
  int speed = 80;
  
  servos[0][1].write(100, speed);
  delay(interval/3);
  servos[0][1].write(60, speed/2);
  delay((interval * 2)/3);
}

void bobLeftShoulder(int interval) {
  int speed = 80;
  
  servos[1][0].write(100, speed);
  delay(interval/3);
  servos[1][0].write(130, speed/2);
  delay((interval * 2)/3);
}

void bobRightShoulder(int interval) {
  int speed = 80;
  
  servos[0][0].write(80, speed);
  delay(interval/3);
  servos[0][0].write(50, speed/2);
  delay((interval * 2)/3);
}

void bobBothShoulders(int interval) {
  int speed = 80;
  
  servos[1][0].write(100, speed);
  servos[0][0].write(80, speed);
  delay(interval/3);
  servos[1][0].write(130, speed/2);
  servos[0][0].write(50, speed/2);
  delay((interval * 2)/3);
}

void discoHead(int interval, int reps) {
  // servos[2][0].write(initVals[2][0]);
            
  for (int i = 0; i < reps; i++) {
    bobHead(interval);
  }
}

void discoLeft(int interval, int reps) {
  servos[2][0].write(60);
  servos[1][1].write(100);

  for (int i = 0; i < reps; i++) {
    bobLeftArm(interval);
  }

  servos[2][0].write(initVals[2][0]);
  servos[1][1].write(initVals[1][1]);
}

void discoRight(int interval, int reps) {
  servos[2][0].write(20);
  servos[0][1].write(60);

  for (int i = 0; i < reps; i++) {
    bobRightArm(interval);
  }

  servos[2][0].write(initVals[2][0]);
  servos[0][1].write(initVals[0][1]);
}

void discoLift(int interval) {
  int speed = 30;

  servos[0][1].write(100, speed);
  servos[1][1].write(80, speed);
  delay(interval * 4);
  servos[0][1].write(initVals[0][1], speed * 4);
  servos[1][1].write(initVals[1][1], speed * 4);
}

void discoBoth(int interval) {
  int speed = 100;
  
  servos[0][0].write(70, speed);
  delay(interval * 2);
  servos[1][0].write(110, speed);
  delay(interval * 2);
  servos[0][0].write(initVals[0][0], speed);
  delay(interval * 2);
  servos[1][0].write(initVals[1][0], speed);
  delay(interval);
  servos[0][0].write(70, speed * 2);
  servos[1][0].write(110, speed * 2);
  delay(interval);
  servos[0][0].write(initVals[0][0], speed * 2);
  servos[1][0].write(initVals[1][0], speed * 2);
}

void discoLeftPoint(int interval, int reps) {
  servos[2][0].write(60);
  servos[1][0].write(130);

  for (int i = 0; i < reps; i++) {
    bobLeftShoulder(interval);
  }

  servos[2][0].write(initVals[2][0]);
  servos[1][0].write(initVals[1][0]);
}

void discoRightPoint(int interval, int reps) {
  servos[2][0].write(40);
  servos[0][0].write(50);

  for (int i = 0; i < reps; i++) {
    bobRightShoulder(interval);
  }

  servos[2][0].write(initVals[2][0]);
  servos[0][0].write(initVals[0][0]);
}

void discoFlap(int interval, int reps) {
  for (int i = 0; i < reps; i++) {
    bobBothShoulders(interval);
  }
}

void discoYmca(int interval) {
  int speed = 120;
  
  servos[0][1].write(180, speed);
  servos[1][1].write(0, speed);
  delay(interval * 2);
  servos[0][1].write(150, speed);
  servos[1][1].write(30, speed);
  delay(interval);
  servos[1][1].write(initVals[1][1], speed * 3);
  delay((interval * 4)/16);
  servos[1][0].write(90, speed * 2);
  delay((interval * 4)/16);
  servos[1][0].write(initVals[1][0], speed * 3);
  delay((interval * 4)/16);
  servos[1][1].write(30, speed * 2);
  delay((interval * 4)/16);
  delay(interval);

  servos[0][0].write(initVals[0][0], speed/2);
  servos[0][1].write(initVals[0][1], speed/2);
  servos[1][0].write(initVals[1][0], speed/2);
  servos[1][1].write(initVals[1][1], speed/2);

  delay(interval * 3);
}

void discoReset() {
  int speed = 220;

  for (int i = 0; i < numGroups; i++) {
    for (int j = 0; j < servosPerGroup; j++) {
      servos[i][j].write(initVals[i][j], speed);
    }
  }
}

void ymca(int interval) {
  // intro
  discoHead(interval, 24);

  // verse 1
  discoLeft(interval, 8);
  discoRight(interval, 4);
  discoLift(interval);
  discoLeft(interval, 8);
  discoBoth(interval);

  // verse 2
  discoLeft(interval, 8);
  discoRight(interval, 8);
  discoLeftPoint(interval, 4);
  discoRightPoint(interval, 4);
  discoBoth(interval);

  // chorus
  discoFlap(interval, 4);
  delay(interval * 4);
  discoReset();
  discoYmca(interval);
  discoYmca(interval);
  discoFlap(interval * 4, 3);
  discoFlap(interval, 3);
  discoReset();
  delay(interval);
  discoYmca(interval);
  discoYmca(interval);
  discoFlap(interval * 4, 3);
  discoFlap(interval, 4);
  discoReset();
  
  // verse 3
  discoLeft(interval, 8);
  discoRight(interval, 8);
  discoLeft(interval, 8);
  discoBoth(interval);

  // verse 4
  discoFlap(interval, 8);
  discoLeftPoint(interval, 4);
  discoRightPoint(interval, 4);
  discoLift(interval);
  discoFlap(interval, 4);
  discoBoth(interval);
}

void discoLeftWithHead(int interval, int reps) {
  int speed = 30;

  servos[1][1].write(90, speed);
  servos[2][0].write(70);

  discoHead(interval, reps);

  servos[1][1].write(initVals[1][1]);
  servos[2][0].write(initVals[2][0]);
}

void discoRightWithHead(int interval, int reps) {
  int speed = 30;

  servos[0][1].write(90, speed);
  servos[2][0].write(10);

  discoHead(interval, reps);

  servos[0][1].write(initVals[0][1]);
  servos[2][0].write(initVals[2][0]);
}

void discoAh(int interval) {
  int speed = 255;

  servos[2][0].write(initVals[2][0]);
  servos[0][0].write(120, speed);
  servos[1][1].write(0, speed);
  servos[2][1].write(70, speed);
  delay(interval/2);
  
  servos[0][0].write(initVals[0][0], speed);
  servos[1][1].write(initVals[1][1], speed);
  servos[2][1].write(initVals[2][1], speed);
  delay(interval/2);
  
  servos[1][0].write(60, speed);
  servos[0][1].write(180, speed);
  servos[2][1].write(70, speed);
  delay(interval/2);
  
  servos[1][0].write(initVals[1][0], speed);
  servos[0][1].write(initVals[0][1], speed);
  servos[2][1].write(initVals[2][1], speed);
  delay(interval/2);
  
  servos[0][0].write(60, speed);
  servos[2][1].write(10, speed);
  delay(interval/2);
  
  servos[0][0].write(initVals[0][0], speed);
  servos[2][1].write(initVals[2][1], speed);
  delay(interval/2);
  
  servos[1][0].write(120, speed);
  servos[2][1].write(70, speed);
  delay(interval/2);
  
  servos[1][0].write(initVals[1][0], speed);
  servos[2][1].write(initVals[2][1], speed);
  delay(interval/2);
}

void discoRaise(int interval, int reps) {
  int speed = 5;

  servos[2][0].write(initVals[2][0]);
  servos[0][0].write(120, speed);
  servos[1][0].write(60, speed);

  discoHead(interval, reps);
  
  servos[0][0].write(initVals[0][0]);
  servos[1][0].write(initVals[1][0]);
}

void discoSlowFlap(int interval, int reps) {
  int speed = 50;

  servos[2][0].write(initVals[2][0]);
  servos[0][0].write(90, speed);
  servos[1][0].write(90, speed);

  discoHead(interval, reps/2);
  
  servos[0][0].write(initVals[0][0], speed);
  servos[1][0].write(initVals[1][0], speed);

  discoHead(interval, reps/2);
}

void discoWave(int interval) {
  int speed = 100;

  servos[0][0].write(90, speed);

  discoHead(interval, 1);

  servos[1][0].write(90, speed);

  discoHead(interval, 1);

  servos[0][0].write(initVals[0][0], speed);

  discoHead(interval, 1);

  servos[1][0].write(initVals[1][0], speed);

  discoHead(interval, 1);
}

void discoSweep(int interval) {
  int speed = 255;

  servos[2][0].write(initVals[2][0]);
  servos[2][1].write(70, speed);
  servos[0][0].write(90, speed);
  servos[1][0].write(90, speed);

  delay(interval);

  servos[2][1].write(initVals[2][1], speed);
  servos[0][0].write(initVals[0][0], speed);
  servos[1][0].write(initVals[1][0], speed);

  delay(interval/2);
  
  servos[0][1].write(120, speed);
  servos[1][1].write(70, speed);

  delay(interval/2);

  servos[2][1].write(70, speed);
  servos[0][1].write(initVals[0][1], speed);
  servos[1][1].write(initVals[1][1], speed);

  delay(interval/2);

  servos[0][0].write(90, speed);
  servos[1][0].write(90, speed);

  delay(interval/2);

  servos[2][1].write(initVals[2][1], speed);
  servos[0][0].write(initVals[0][0], speed);
  servos[1][0].write(initVals[1][0], speed);

  delay(interval/2);
  
  servos[0][1].write(120, speed);
  servos[1][1].write(70, speed);

  delay(interval/2);

  servos[0][1].write(initVals[0][1], speed);
  servos[1][1].write(initVals[1][1], speed);
}

void discoCross(int interval) {
  int speed = 255;

  servos[2][0].write(initVals[2][0]);
  servos[2][1].write(70, speed);
  servos[0][0].write(30, speed);
  servos[1][0].write(150, speed);
  servos[0][1].write(90, speed);
  servos[1][1].write(90, speed);

  delay(interval);
  
  servos[2][1].write(initVals[2][1], speed);
  servos[0][1].write(0, speed);
  servos[1][1].write(180, speed);

  delay(interval);
  
  servos[2][1].write(70, speed);
  servos[0][1].write(90, speed);
  servos[1][1].write(90, speed);

  delay(interval);
  
  servos[2][1].write(initVals[2][1], speed);
  servos[0][0].write(initVals[0][0], speed);
  servos[1][0].write(initVals[1][0], speed);
  servos[0][1].write(initVals[0][1], speed);
  servos[1][1].write(initVals[1][1], speed);

  delay(interval);
}

void alive(int interval, int part) {
  if (part == 1) {
    // intro
    delay(interval * 20);
    
    // verse 1
    delay(interval * 20);
    discoHead(interval, 4);
    servos[2][0].write(10, 170);
    discoHead(interval, 4);
    servos[2][0].write(70, 170);
    discoHead(interval, 4);
    servos[2][0].write(initVals[2][0], 170);
    discoHead(interval, 4);

    // chorus
    discoRightWithHead(interval, 4);
    discoLeftWithHead(interval, 2);
    discoRightWithHead(interval, 2);
    discoLeftWithHead(interval, 4);
    discoRightWithHead(interval, 2);
    discoLeftWithHead(interval, 2);
    discoAh(interval);
    discoFlap(interval * 2, 2);
    discoAh(interval);
    discoRaise(interval, 16);

//    runMotorOne(255);
//    runMotorTwo(-255);
  } else if (part == 2) {
    delay(650);
    
    // verse 2
    discoHead(interval, 40);

    // chorus
    discoSlowFlap(interval, 4);
    discoFlap(interval * 2, 2);
    discoWave(interval);
    discoFlap(interval * 2, 2);
    discoSweep(interval);
    discoFlap(interval * 2, 2);
    discoSweep(interval);
    discoRaise(interval, 16);
  } else if (part == 3) {
    delay(2500);
    
    // verse 3
    discoHead(interval, 20);
    servos[2][0].write(10, 170);
    discoHead(interval, 4);
    servos[2][0].write(70, 170);
    discoHead(interval, 4);
    servos[2][0].write(initVals[2][0], 170);
    discoHead(interval, 4);

    // chorus
    discoRightWithHead(interval, 4);
    discoLeftWithHead(interval, 2);
    discoRightWithHead(interval, 2);
    discoLeftWithHead(interval, 4);
    discoRightWithHead(interval, 2);
    discoLeftWithHead(interval, 2);
    discoCross(interval);
    discoFlap(interval * 2, 2);
    discoCross(interval);
    discoRaise(interval, 16);
  } else if (part == 4) {
    
  }
}

void liftArms() {
  int speed = 30;

  servos[0][1].write(100, speed);
  servos[1][1].write(80, speed);
}

void lowerArms() {
  int speed = 30;

  servos[0][1].write(initVals[0][1], speed);
  servos[1][1].write(initVals[1][1], speed);
}

void talkGest(int key) {
  int speed = 40;

  if (key == 0) {
    servos[0][1].write(100, speed, true);
    servos[0][1].write(80, speed, true);
    servos[0][0].write(40, speed, true);
    servos[0][1].write(100, speed, true);
    servos[0][1].write(80, speed, true);
    servos[0][0].write(initVals[0][0], speed, true);
    servos[0][1].write(initVals[0][1], speed, true);
  } else if (key == 1) {
    servos[0][1].write(120, speed, true);
    servos[0][1].write(initVals[0][1], speed, true);
  } else if (key == 2) {
    servos[1][1].write(60, speed, true);
    servos[1][1].write(initVals[1][1], speed, true);
  } else if (key == 3) {
    servos[0][1].write(120, speed);
    servos[1][1].write(60, speed);
    servos[0][1].wait();
    servos[1][1].wait();
    servos[0][1].write(initVals[0][1], speed);
    servos[1][1].write(initVals[1][1], speed);
    servos[0][1].wait();
    servos[1][1].wait();
  } else if (key == 4) {
    servos[0][0].write(70, speed);
    servos[1][0].write(110, speed);
    servos[0][0].wait();
    servos[1][0].wait();
    servos[0][0].write(initVals[0][0], speed);
    servos[1][0].write(initVals[1][0], speed);
    servos[0][0].wait();
    servos[1][0].wait();
  }
}

void dab() {
  int speed = 20;

  servos[0][0].write(60, speed);
  servos[1][0].write(90, speed);
  servos[0][0].wait();
  servos[1][0].wait();
  servos[0][1].write(80, speed);
  servos[1][1].write(170, speed);
  servos[2][0].write(20, speed);
  servos[2][1].write(50, speed);
  delay(2000);

  servos[0][0].write(initVals[0][0], speed);
  servos[0][1].write(initVals[0][1], speed);
  servos[1][0].write(initVals[1][0], speed);
  servos[1][1].write(initVals[1][1], speed);
  servos[2][0].write(initVals[2][0], speed);
  servos[2][1].write(initVals[2][1],speed);
}
