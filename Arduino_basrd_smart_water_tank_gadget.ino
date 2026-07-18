// --- Pin Configurations ---
const int sensor10 = A0;   // Bottom Sensor
const int sensor50 = A1;   // Middle Sensor
const int sensor90 = A2;   // Top Sensor

const int redLed = 2;      // 10% Indicator
const int blueLed = 3;     // 50% Indicator
const int greenLed = 4;    // 90% Indicator

const int buzzer = 5;      // Alert Buzzer
const int relayPump = 6;   // 2-Channel Relay (Channel 1)

// --- System Variables ---
bool pumpState = false;    // Tracks if pump is currently ON or OFF
bool topBuzzerDone = false; // Prevents the 4-beep alarm from looping indefinitely

void setup() {
  // Initialize Sensor Pins as Inputs
  pinMode(sensor10, INPUT);
  pinMode(sensor50, INPUT);
  pinMode(sensor90, INPUT);

  // Initialize Output Pins
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(relayPump, OUTPUT);

  // Initially turn off everything (Assuming Relay is Active LOW, change to HIGH if needed)
  digitalWrite(relayPump, HIGH); 
  digitalWrite(redLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(buzzer, LOW);

  Serial.begin(9600);
}

void loop() {
  // Soil Moisture sensors usually give LOW (0) when wet and HIGH (1) when dry
  // Agar aap digital output use kar rahe hain DO pin se:
  bool isWet10 = (digitalRead(sensor10) == LOW);
  bool isWet50 = (digitalRead(sensor50) == LOW);
  bool isWet90 = (digitalRead(sensor90) == LOW);

  // --- LEVEL 1: CRITICAL LOW (Below 10%) ---
  if (!isWet10) { 
    // Tank khali hai
    digitalWrite(redLed, HIGH);
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, LOW);
    
    // Continuous Beep (Non-blocking pattern to keep responsiveness)
    digitalWrite(buzzer, HIGH); 
    
    // Turn ON Water Pump
    pumpState = true;
    digitalWrite(relayPump, LOW); // Active LOW relay turns ON
    topBuzzerDone = false;        // Reset the top alarm flag
  }
  
  // --- LEVEL 2: MID LEVEL (50% Filled) ---
  else if (isWet10 && !isWet90) {
    digitalWrite(redLed, LOW);
    digitalWrite(blueLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(buzzer, LOW); // Turn off continuous low beep
    
    // Keep the pump state running if it started from 10%
    if (pumpState) {
      digitalWrite(relayPump, LOW); 
    } else {
      digitalWrite(relayPump, HIGH);
    }
  }
  
  // --- LEVEL 3: TANK FULL (90% or above) ---
  else if (isWet90) {
    digitalWrite(redLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(buzzer, LOW);
    
    // Turn OFF Water Pump
    pumpState = false;
    digitalWrite(relayPump, HIGH); // Active LOW relay turns OFF
    
    // 4-Times Beep Alarm (Triggers only once when it hits 90%)
    if (!topBuzzerDone) {
      for (int i = 0; i < 4; i++) {
        digitalWrite(buzzer, HIGH);
        delay(200);
        digitalWrite(buzzer, LOW);
        delay(200);
      }
      topBuzzerDone = true; 
    }
  }

  delay(500); // Small delay for system stability
}