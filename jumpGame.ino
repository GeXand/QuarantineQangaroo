//Digital Pins

// Ultrasonic sensors
#define echoPin1 2
#define trigPin1 3
#define echoPin2 4
#define trigPin2 5

// Arcade Button
#define arcadeButton 6
#define arcadeLight 7





// Pseudo queue that's always full
int queue1[] = {0,0,0,0,0};
void queue1Push(int x) {
  int queueSize = sizeof(queue1);
  
  for (int i = 0; i < queueSize - 1; i++) {
    queue1[i] = queue1[i+1];
  }

  queue1[queueSize - 1] = x;
}

int queue2[] = {0,0,0,0,0};
void queue2Push(int x) {
  int queueSize = sizeof(queue2);
  
  for (int i = 0; i < queueSize - 1; i++) {
    queue2[i] = queue2[i+1];
  }

  queue2[queueSize - 1] = x;
}




// Init components
void initUTSensor1() {
  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin1, INPUT); // Sets the echoPin as an INPUT
}

void initUTSensor2() {
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin2, INPUT); // Sets the echoPin as an INPUT
}

void initArcadeButton() {
  pinMode(arcadeButton, INPUT_PULLUP);
  pinMode(arcadeLight, OUTPUT);
}








// Actual component functionality


// Get the distance for the Ultrasonic sensor in centimeters
long duration;
int distance;

int getDistance1(){
  // Clears the trigPin condition
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin1, HIGH);
  
  // Calculating the distance
  distance = duration * 0.034 / 2.0; // Speed of sound wave divided by 2 (go and back)
  
  return distance;
}

int getDistance2(){
  // Clears the trigPin condition
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin2, HIGH);
  
  // Calculating the distance
  distance = duration * 0.034 / 2.0; // Speed of sound wave divided by 2 (go and back)
  
  return distance;
}

// Check if the button is pressed
bool buttonPressed() {
  if (digitalRead(arcadeButton) > LOW) {
    return false;
  } else {
    return true;
  }
}

bool lightState = false;
// Toggle the arcade light on or off
void toggleLight(bool on) {
  if (on) {
    digitalWrite(arcadeLight, HIGH);
    lightState = true;
  } else {
    digitalWrite(arcadeLight, LOW);
    lightState = false;
  }
}








// Game logic

bool gameStarted;
int score1;
int score2;
bool active1;
bool active2;

bool jump1registered;
bool jump2registered;

long startFlashTime;
bool curFlashing;

// Restart the game
void resetGame() {
  score1 = 0;
  score2 = 0;
  active1 = true;
  active2 = true;
  curFlashing = false;
  gameStarted = true;
  
  toggleLight(true);
  delay(5000); // give some time to get ready for the countdown to start
  toggleLight(false);
}

// Signal the start of the game by flashing three times
void signalGameStart() {
  for (int i = 0; i < 3; i++) {
    delay(1000);
    toggleLight(true);
    delay(1000);
    toggleLight(false);
  }
}

// Randomly pick intervals to wait between flashes
// Minimum interval time is 3 seconds, maximum is 8
// TODO: Maybe implement artificial distribution so they tend to be around average time but sometimes are longer
void randomDelayFlash() {
  long duration = 3000 + random(5000);
  delay(duration);
  toggleLight(true);
  curFlashing = true;
  startFlashTime = millis();
}

// Check when to stop flashing AND check for jumps
void flashCheck() {
  Serial.println(startFlashTime);
  Serial.println(millis());
  long timeSinceFlash = millis() - startFlashTime;
  Serial.println(timeSinceFlash);

  
  // TODO: May want to incrememntly decrease the length of the flash as time goes on
  if(timeSinceFlash >= 10000) {
    curFlashing = false;
    toggleLight(false);

    // Made jump = increment score and reset the var checking if jump was made
    if (jump1registered) {
      score1++;
      jump1registered = false;
    // Missed jump = no longer active
    } else {
      Serial.println("Player 1 Lost");
      active1 = false;
    }

    if (jump2registered) {
      score2++;
      jump2registered = false;
    } else {
      Serial.println("Player 2 Lost");
      active2 = false;
    }

    if (!active1 && !active2) {
      Serial.println("Both players lost, resetting...");
      gameStarted = false;
    }
  } else {
    Serial.println("Flash Check");
    queue1Push(getDistance1());
    queue2Push(getDistance2());

    if (jumped1()) {
      jump1registered = true;
    }
    if (jumped2()) {
      jump2registered = true;
    }
  
  } 
}

// Determine if jump registered 1
bool jumped1(){
  for (int i = 0; i < sizeof(queue1); i++) {
    // Doesn't count as jump if any of the past x readings are within a meter
    if (queue1[i] < 100) {
      return false;
    }
  }

  return true;
}

// Determine if jump registered 2
bool jumped2(){
  for (int i = 0; i < sizeof(queue2); i++) {
    // Doesn't count as jump if any of the past x readings are within a meter
    if (queue2[i] < 100) {
      return false;
    }
  }

  return true;
}





// Arduino structure

long loopCount;

void setup() {
  loopCount = 0;
  startFlashTime = 0;
  
  initUTSensor1();
  initUTSensor2();
  initArcadeButton();

  toggleLight(false);

  gameStarted = false;
  score1 = 0;
  score2 = 0;

  Serial.begin(9600);
}


void loop() {
  if (gameStarted) {
    if (!curFlashing) {     
      randomDelayFlash();
    } else {
      flashCheck();
    }
    
  } else {
    if (buttonPressed()) {
      resetGame();
      signalGameStart();
      Serial.println("Starting Game...");
    }
  }

  loopCount++;
}
