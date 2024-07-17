const int numRows = 2;
const int numCols = 2;

const int rowPins[numRows] = {12, 10}; // Connect to the row pins of the matrix
const int colPins[numCols] = {8, 6}; // Connect to the column pins of the matrix

void setup() {
  for (int row = 0; row < numRows; row++) {
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], HIGH);
  }

  for (int col = 0; col < numCols; col++) {
    pinMode(colPins[col], INPUT_PULLUP);
  }

  Serial.begin(9600);
}

void loop() {
  for (int row = 0; row < numRows; row++) {
    digitalWrite(rowPins[row], LOW);
    for (int col = 0; col < numCols; col++) {
      if (digitalRead(colPins[col]) == LOW) {
        Serial.print("Button pressed at row ");
        Serial.print(row);
        Serial.print(", column ");
        Serial.println(col);
      }
    }
    digitalWrite(rowPins[row], HIGH);
  }
  delay(100);
}
