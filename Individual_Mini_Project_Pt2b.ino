/* One bug found is that in every time the arduino is reset, the first two rolls of random will always be 1, thus making it so when setLocationCells(); is used in setup, the location is always 123 */

//potentiometer setup
int sensorPin = A0;
int sensorValue = 0;
//battleship field LEDs
const int ledPins[] = {2, 3, 4, 5, 6, 7, 8, 9};
//hit marker LED
int ledHit = 10;
//hit marker peizo
int speakerPin = 11;
//lower button to choose
int choosebtn = 12;
int buttonState = 0;
//board and ship size constants
const int max_board_size = 8;
const int ship_size = 3;
//variable to count how many times a ship has been hit
int numhits = 0;
//int array for ships location
int locationCells[ship_size];
//declaring userGuess here so it can be used in other functions apart from checkYourself()
int userGuess = 0;
//peizo setup
int tempo = 300;
int tonelength = 0;
//Array to determine if the LED has been hit or not
int hitornot[] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  //8 Green LEDs for the grid setup
  for (int i = 0; i < max_board_size; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  //red hit marker LED set up
  pinMode(ledHit, OUTPUT);
  //peizo setup for sfx
  pinMode(speakerPin, OUTPUT);
  //setup for button
  pinMode(choosebtn, INPUT);
  //Still going to use Serial Monitor to debug
  Serial.begin(9600);
  //This for loop is for debugging the setLocationCells() that always produces 123 as its location the first two times the code is run.
  for (int i = 0; i < 3; i++) {
    setLocationCells();
  }
}

void loop() {
  //running the game using the other functions
  String result = checkYourself();
  //debugging using the serial monitor
  Serial.println(userGuess + 1);
  Serial.println(result);
  //done to make sure debugging works on endgame
  delay(500);
  //code to stop the game
  if (result == "kill") {
    exit(1);
  }
}

//function to get the guess from the board
int getGuess() {
  //setting int guess here so it's easier to use as its only used here, also stops this function 
  //from constantly looping and playing the miss code as it resets guess to -1 after every guess.
  int guess = -1684867487;
  //this makes it so it only takes the guesses when the button is pressed
  while ( guess > 7 || guess < 0) {
    //getting the value of the potentiometer
    sensorValue = analogRead(sensorPin);
    //setting up a loop to setup each LED with the position the potentiometer is in
    for (int i = 0; i < max_board_size; i++) {
      //reading button state to choose which LED to hit
      buttonState = digitalRead(choosebtn);
      if (sensorValue >= 128 * i && sensorValue <= 128 * (i + 1)) {
        //code for the red LED marker to light up if this LED has produced a hit before or not
        if (hitornot[i] == 1) {
          digitalWrite(ledHit, HIGH);
        } else {
          digitalWrite(ledHit, LOW);
        }
        //code to show which LED is being selected
        digitalWrite(ledPins[i], HIGH);
        //code to select your guess with the button
        if (buttonState == LOW) {
          guess = i;
        }
      } else {
        digitalWrite(ledPins[i], LOW);
      }
    }
  }
  //returns value of the guess
  return guess;
}

//code to set the ship position
void setLocationCells() {
  int startLocation = random(0, 6);
  //debugging code here
  Serial.println(startLocation);
  Serial.println("Location Cells generated are : ");
  for (int i = 0; i < ship_size; i++) {
    locationCells[i] = startLocation + i;
    //more debugging code
    Serial.print(locationCells[i]);
  }
  Serial.println();
}

//code to check if its a miss/hit/kill
String checkYourself() {
  //retrieving guess from getGuess();
  userGuess = getGuess();
  //default miss result
  String result = "miss";
  for (int i = 0; i < ship_size; i++) {
    //if its a miss or the ship has already been hit in this spot, it will run the miss code
    if (userGuess != locationCells[i] || hitornot[locationCells[i]] == 1) {
      missCode();
    } else if (userGuess == locationCells[i]) {
      //if its a hit and the place hasnt been hit before, it will run the hit code or kill code depending on if its the final hit
      if (hitornot[locationCells[i]] == 0) {
        numhits++;
        result = "hit";
        hitCode();
      }
      //recording that this LED has been hit for future reference,  e.g if the same LED is hit again
      hitornot[locationCells[i]] = 1;
      //runs the kill code if the ship is destroyed
      if (numhits == ship_size) {
        result = "kill";
        killCode();
      }
      break;
    }
  }
  return result;
}

//this code is for the feedback on miss using the buzzer
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

//this code is for the feedback on kill using the peizo and LEDs
void killCode() {
  tonelength = 5;
  char notes[] = "ccgg ";
  int beats[] = { 1, 1, 1, 1, 1};
  //these for loops are to flash the LEDs to show game over
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
  //game over tune
  for (int i = 0; i < tonelength; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); //
    } else {
      playNote(notes[i], beats[i] * tempo);
    }
    delay(tempo / 2);
  }
}

//this code is for feedback on hit using peizo and LED's
void hitCode() {
  tonelength = 3;
  char notes[] = "ee ";
  int beats[] = { 1, 1, 1 };
  // this for loop flashes the hit LED and hit indicator LED to show a hit
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPins[userGuess], HIGH);
    digitalWrite(ledHit, HIGH);
    delay(50);
    digitalWrite(ledPins[userGuess], LOW);
    digitalWrite(ledHit, LOW);
    delay(50);
  }
  //hit melody
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
