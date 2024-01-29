//setup for LEDs
const int ledPins[] = {2,3,4,5,6,7,8,9};
//setup for board size
const int max_board_size = 8;
//setup to choose ship size
const int ship_size = 3;
//setup to count total hits so a kill can be declared
int numhits = 0;
//setup for the ships location
int locationCells[ship_size];

void setup() {
  //setting up the LEDs as outputs
  for(int i = 0; i<max_board_size; i++){
    pinMode(ledPins[i],OUTPUT);
  }
  //starting the serial monitor
  Serial.begin(9600);
  //This for loop is for debugging the setLocationCells() that always produces 123 as its location the first two times the code is run.
  for(int i = 0; i<3;i++){
    setLocationCells();
  }
}

//actually running the game using the other functions
void loop() {
  String result = checkYourself();
  Serial.println(result);
  delay(500);
  if(result == "kill"){
    exit(1);
  }
}

//function to get the users guess
char getGuess(){
  char readinput = '^';
  while(readinput > '8' || readinput < '1'){
    //telling the user to enter a guess
    Serial.println("Enter a Guess (1-8):");
    while(Serial.available() == 0){
    }
    //reading the guess
    readinput = Serial.read();
    Serial.print("You have entered: ");
    Serial.println(readinput);
    //checking if the guess can be used
    if(readinput > '8' || readinput < '1'){
      Serial.println("Invalid input");
    }
  }
  //stores the guess
  return readinput;
}

//used to set the location of the ship
void setLocationCells(){
  int startLocation = random(0,6);
  //used for debugging
  Serial.print("Location Cells generated are : ");
  for(int i = 0; i <ship_size; i++){
    locationCells[i] = startLocation + i;
    //used for debugging
    Serial.print(locationCells[i]);
  }
  Serial.println();
}

// used to determine if its a hit or miss or kill
String checkYourself(){
  //retrieves guess from before
  char userGuess = getGuess();
  String result = "miss";
  //converts guess to an int so it can be used
  int guess = (userGuess - '0');
  guess -= 1;
  //checks for hit to show hit or kill, otherwise the result will stay as miss
  for(int i = 0; i<ship_size; i++){
    if(guess == locationCells[i]){
      numhits++;
      locationCells[i] = -1;
      digitalWrite(ledPins[guess],HIGH);
      if(numhits == ship_size){
        result = "kill";
      } else result = "hit";
      break;
    }
  }
  //returns the result after checking
  return result;
}
