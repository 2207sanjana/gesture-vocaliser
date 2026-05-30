/*
 * Gesture Vocalizer
 * Author: Sanjana Banala
 * Platform: Arduino Uno + HC-05 Bluetooth Module
 * Description: Electronic glove that reads flex sensor values from
 *              5 fingers and maps gesture patterns to sign language
 *              words, transmitting them via Bluetooth to a speech app.
 *
 * Pin Configuration:
 *   Flex Sensors: Thumb=A0, Index=A1, Middle=A2, Ring=A3, Pinky=A4
 *   Bluetooth TX: Pin 10 (SoftwareSerial RX)
 *   Bluetooth RX: Pin 11 (SoftwareSerial TX)
 */

#include <SoftwareSerial.h>

// ── Bluetooth Serial ───────────────────────────────────────────
SoftwareSerial bluetooth(10, 11); // RX, TX

// ── Flex Sensor Pins ───────────────────────────────────────────
#define THUMB   A0
#define INDEX   A1
#define MIDDLE  A2
#define RING    A3
#define PINKY   A4

// ── Threshold Values ──────────────────────────────────────────
// Calibrate these for your specific flex sensors
// Values below threshold = finger BENT
// Values above threshold = finger STRAIGHT
#define BENT_THRESHOLD 400

// ── Debounce & Timing ─────────────────────────────────────────
#define READ_DELAY    800   // ms between gesture reads
#define CONFIRM_READS 2     // consecutive matching reads to confirm

// ──────────────────────────────────────────────────────────────

struct Gesture {
  bool thumb;
  bool index;
  bool middle;
  bool ring;
  bool pinky;
  const char* word;
};

// Gesture library — true = finger BENT, false = STRAIGHT
// Extend this table to add more signs
const Gesture GESTURES[] = {
  // thumb  index  middle ring  pinky  word
  { false, false, false, false, false, "HELLO" },     // all straight
  { true,  true,  true,  true,  true,  "STOP" },      // all bent (fist)
  { false, true,  false, false, false, "YES" },        // index pointing up
  { true,  false, false, false, false, "NO" },         // thumb only
  { false, true,  true,  false, false, "PEACE" },      // index + middle
  { true,  false, false, false, true,  "I LOVE YOU" }, // thumb + pinky
  { false, false, false, false, true,  "PINKY" },      // pinky only
  { true,  true,  false, false, false, "GUN" },        // thumb + index
  { false, true,  true,  true,  true,  "FOUR" },       // four fingers up
  { true,  true,  true,  false, false, "THREE" },      // three bent
};

const int GESTURE_COUNT = sizeof(GESTURES) / sizeof(GESTURES[0]);

// ──────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);

  pinMode(THUMB,  INPUT);
  pinMode(INDEX,  INPUT);
  pinMode(MIDDLE, INPUT);
  pinMode(RING,   INPUT);
  pinMode(PINKY,  INPUT);

  Serial.println("Gesture Vocalizer Ready.");
  bluetooth.println("Gesture Vocalizer Ready.");
  delay(1000);
}

void loop() {
  // Read all 5 sensors
  int tVal = analogRead(THUMB);
  int iVal = analogRead(INDEX);
  int mVal = analogRead(MIDDLE);
  int rVal = analogRead(RING);
  int pVal = analogRead(PINKY);

  // Convert to bent/straight booleans
  bool tBent = tVal < BENT_THRESHOLD;
  bool iBent = iVal < BENT_THRESHOLD;
  bool mBent = mVal < BENT_THRESHOLD;
  bool rBent = rVal < BENT_THRESHOLD;
  bool pBent = pVal < BENT_THRESHOLD;

  // Debug output to Serial Monitor
  Serial.print("T:"); Serial.print(tVal);
  Serial.print(" I:"); Serial.print(iVal);
  Serial.print(" M:"); Serial.print(mVal);
  Serial.print(" R:"); Serial.print(rVal);
  Serial.print(" P:"); Serial.println(pVal);

  // Match gesture
  String detected = matchGesture(tBent, iBent, mBent, rBent, pBent);

  if (detected != "") {
    Serial.print("Gesture: "); Serial.println(detected);
    bluetooth.println(detected);
  } else {
    Serial.println("No match");
  }

  delay(READ_DELAY);
}

// ── Gesture Matching ──────────────────────────────────────────

String matchGesture(bool t, bool i, bool m, bool r, bool p) {
  for (int g = 0; g < GESTURE_COUNT; g++) {
    if (GESTURES[g].thumb  == t &&
        GESTURES[g].index  == i &&
        GESTURES[g].middle == m &&
        GESTURES[g].ring   == r &&
        GESTURES[g].pinky  == p) {
      return String(GESTURES[g].word);
    }
  }
  return "";
}
