/* One bug found is that in every time the arduino is reset, the first two rolls of random will always be 1 and the third will be 5, thus making it so when setLocationCells(); is used in setup, the location is not actually random */

//battleship field LEDs
const int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
//hit marker LED
int ledHit = 10;
//hit marker peizo
int speakerPin = 11;
//board and ship size constants
const int max_board_size = 8;
const int ship_size = 3;
//variable for hit count for ship
int numhits = 0;
//int array for ships location
int locationCells[ship_size];
//guess variable declared locally instead of inside checkYourself so it works with other functions for user feedback
int guess = 0;
//variables for peizo
int tempo = 300;
int tonelength = 0;

void setup() {
  //setup for the 8 green LEDs that make the grid field
  for (int i = 0; i < max_board_size; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  //setup for the red LED hitmarker
  pinMode(ledHit, OUTPUT);
  //setup for the peizo for sfx
  pinMode(speakerPin, OUTPUT);
  //setup for Serial Monitor for feedback and guessing
  Serial.begin(9600);
  //Just to make it not 123 every single time the code is run, random is not actually random
  randomSeed(analogRead(0));
  setLocationCells();
}

void loop() {
  //running the code to play the game
  String result = checkYourself();
  Serial.println(result);
  delay(500);
  if (result == "Destroyed!!!") {
    Serial.println("You Win!!!!!");
    //this delay is to make sure the You Win!!! line is actually printed
    delay(100);
    exit(1);
  }
}

//code to get the user's guess and making sure its correct
char getGuess() {
  //declaring the variable used as a non input kind of guess so people wouldnt accidentally type it in
  char readinput = '^';
  while (readinput > '8' || readinput < '1') {
    //Used to tell user to guess a number and retrieves that value
    Serial.println("Enter a Guess (1-8):");
    while (Serial.available() == 0) {
    }
    readinput = Serial.read();
    Serial.print("You have entered: ");
    Serial.println(readinput);
    if (readinput > '8' || readinput < '1') {
      Serial.println("Invalid input");
    }
  }
  //stores the value for use elsewhere in the code
  return readinput;
}

//code to set the ship's location pseudo-randomly
void setLocationCells() {
  int startLocation = random(0, 6);
  //commented out code is for debugging, if uncommented, it would defeat the point of the game
//  Serial.println("Location Cells generated are : ");
  for (int i = 0; i < ship_size; i++) {
    locationCells[i] = startLocation + i;
//    Serial.print(locationCells[i]);
  }
  Serial.println();
}

//code to see if the user guess is a hit miss or kill with the feedbacks
String checkYourself() {
  //takes the value taken from the user from the function getGuess()
  char userGuess = getGuess();
  String result = "You Miss!";
  //transforms the users guess into an int so it can be used to in the code
  guess = (userGuess - '0');
  guess -= 1;
  for (int i = 0; i < ship_size; i++) {
    //this code is added in so that the arduino produces feedback if there is a miss as well
    if (guess != locationCells[i]) {
      missCode();
    } else if (guess == locationCells[i]) {
      numhits++;
      locationCells[i] = -1;
      //this code is here for the feedback upon hit
      hitCode();
      if (numhits == ship_size) {
        result = "Destroyed!!!";
        //this code is here for the feedback upon kill
        killCode();
      } else result = "It's a Hit!";
      break;
    }
  }
  return result;
}

//this code is for the feedback on hit
void hitCode() {
  tonelength = 3;
  char notes[] = "cc ";
  int beats[] = { 1, 1, 1 };
  
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[guess], HIGH);
    digitalWrite(ledHit, HIGH);
    delay(50);
    digitalWrite(ledPins[guess], LOW);
    digitalWrite(ledHit, LOW);
    delay(50);
  }
  for (int i = 0; i < tonelength; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo);
    } else {
      playNote(notes[i], beats[i] * tempo);
    }
    delay(tempo / 2); 
  }
  digitalWrite(ledPins[guess], HIGH);
}

//this code is for the feedback on kill
void killCode() {
  tonelength = 5;
  char notes[] = "ccgg ";
  int beats[] = { 1, 1, 1, 1, 1};
  
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < max_board_size; j++) {
      digitalWrite(ledPins[j], HIGH);
      digitalWrite(ledHit, HIGH);
    }
    delay(50);
    for (int j = 0; j < max_board_size; j++) {
      digitalWrite(ledPins[j], LOW);
      digitalWrite(ledHit, LOW);
    }
    delay(50);
  }
  for (int i = 0; i < tonelength; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); //
    } else {
      playNote(notes[i], beats[i] * tempo);
    }
    delay(tempo / 2); 
  }
}

//this code is for the feedback on miss
void missCode() {
  tonelength = 2;
  char notes[] = "c ";
  int beats[] = { 1, 1};
  for (int i = 0; i < tonelength; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo);
    } else {
      playNote(notes[i], beats[i] * tempo);
    }
    delay(tempo / 2); 
  }
}

//this code is done to setup the peizo
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}
