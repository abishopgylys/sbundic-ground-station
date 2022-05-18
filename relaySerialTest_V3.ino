// this sketch initializes all pins to LOW, then takes serial input to change pins to HIGH to actuate relays
//#define ACTIVE 0  // circuit on
//#define INACTIVE 1  // circuit off
//#define SHORT 2  // part of circuit on
//#define UNKNOWN 3  // no predefined conditions met


void setup() {
  // set pins 2-13 to output
  for (int i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }


  // assuming relays will be wired with a max of four pins from each of the groups 2-5, 6-9, 10-13
  // boolean indicates whether pin from group is currently active


  // begin serial connection
  Serial.begin(9600);

}

// define reset function
void(* resetFunc) (void) = 0; //declare reset function @ address 0

void loop() {
  bool relayGroup[14];
  int onPin[14];

  int tgDownRadio = 2;  // TinyGS downlink relay connection closest to radio
  int tgDownAmp = 6;  // TinyGS downlink relay connection between the PA and LNA
  int sdrDown = 3;  // SDR relay connection
  int tgUpRadio = 4;  // TinyGS uplink relay connection closest to radio
  int tgUpAmp = 8;  // TinyGS uplink connection between the PA and LNA
  int backupTgDown = 9;  // backup downlink-only TinyGS relay connection
  
  // print out current pin states:
  Serial.println("The current pin states are: ");
  
  String downlinkState = returnstate(digitalRead(tgDownRadio), digitalRead(tgDownAmp));
  String uplinkState = returnstate(digitalRead(tgUpRadio), digitalRead(tgUpAmp));
  String sdrState = returnstate(digitalRead(sdrDown));
  String backupState = returnstate(digitalRead(backupTgDown));

  Serial.println();
  Serial.println("Downlink: " + downlinkState);
  Serial.println("Uplink: " + uplinkState);
  Serial.println("SDR: " + sdrState);
  Serial.println("Backup: " + backupState);
  Serial.println();
  
  Serial.println("Pin states: ");
  for (int i = 2; i <= 13; i++) {
    if (((i-2)%4) == 0) {
      Serial.println();
    }
    Serial.println(String(i) + " = " + String(digitalRead(i)));
  }
  Serial.println();

  // update relayGroup states
  for (int i = 2; i <= 13; i++) {
    relayGroup[i] = false;
    onPin[i] = 0;
  }
  for (int i = 2; i <= 13; i++) {
    if (digitalRead(i) == HIGH) {
      for (int j = (floor((i - 2)/4)*4) + 2; j <= (ceil((i - 1)/4)*4) + 1; j++) {
        if (relayGroup[j]) {
          Serial.println("CRITICAL ERROR: two relay pins simultaneously active. Resetting...");
          resetFunc();
        }
        relayGroup[j] = true;
        onPin[j] = i;
      }
    }
  }


  // relay configuration
  Serial.println("Enter 'downlink', 'uplink', 'sdr', 'backup', 'off', or 'restart'.");
  while (Serial.available() == 0) {}  // wait for input
  String newState = String(Serial.readString());  // read new state from buffer and remove from buffer
  Serial.println(newState);  // print new state in monitor for visual confirmation by user
  newState.trim();  // remove newline from entered command

  // set pin to appropriate state; if incorrect input, ask user to correct
  while (true) {
    if (newState.equals("downlink")) {
      for (int i = 2; i <= 13; i++) {
        digitalWrite(i, LOW);
      }
      digitalWrite(tgDownRadio, HIGH);
      digitalWrite(tgDownAmp, HIGH);    
    }
    else if (newState.equals("uplink")) {
      for (int i = 2; i <= 13; i++) {
        digitalWrite(i, LOW);
      }
      digitalWrite(tgUpRadio, HIGH);
      digitalWrite(tgUpAmp, HIGH);
    }
    else if (newState.equals("sdr")) {
      for (int i = 2; i <= 13; i++) {
        digitalWrite(i, LOW);
      }
      digitalWrite(sdrDown, HIGH);
    }
    else if (newState.equals("backup")) {
      for (int i = 2; i <= 13; i++) {
        digitalWrite(i, LOW);
      }
      digitalWrite(backupTgDown, HIGH);     
    }
    else if (newState.equals("off")) {
      for (int i = 2; i <= 13; i++) {
        digitalWrite(i, LOW);
      }
    }
    else if (newState.equals("restart")) {
      for (int i = 2; i <= 13; i++) {
        digitalWrite(i, LOW);
      }
      resetFunc();      
    }
    else {
      Serial.println("Invalid input. Please enter 'downlink', 'uplink', 'sdr', 'backup', 'off', or 'restart'.");
      while (Serial.available() == 0) {}
      newState = String(Serial.readString());
      newState.trim();
      continue;
    }
    break;
  }
}

String returnstate(bool x) {
  if (x) {
    return "ACTIVE";
  }
  else if (not x) {
    return "INACTIVE";
  }
  else {
    return "UNKNOWN";
  }
}

String returnstate(bool x, bool y) {
  if (x and y) {
    return "ACTIVE";
  }
  else if (x or y) {
    return "SHORT";
  }
  else if (not(x or y)) {
    return "INACTIVE";
  }
  else {
    return "UNKNOWN";
  }
}
