// libraries
#include <Keyboard.h>
#include <Mouse.h>
#include <EEPROM.h>

// macros
#define NUM_MACROS 4
#define MACRO_SIZE 128
#define EEPROM_SCROLL_ADDR 0
#define EEPROM_MACROS_START 1 // macros stored sequentially

#define LEFT_CLICK MOUSE_LEFT
#define RIGHT_CLICK MOUSE_RIGHT

int scrollSpeed = 5;

// Helper: EEPROM layout: byte 0 = scrollSpeed, bytes 1.. = macros (NUM_MACROS * MACRO_SIZE)
void loadFromEEPROM() {
  byte b = EEPROM.read(EEPROM_SCROLL_ADDR);
  if (b == 0xFF) scrollSpeed = 5;
  else scrollSpeed = b;
}

void saveToEEPROM() {
  EEPROM.write(EEPROM_SCROLL_ADDR, (byte)scrollSpeed);
}

// Utility to write macro to EEPROM index i (0..NUM_MACROS-1)
void writeMacroToEEPROM(int i, const char* data, int len) {
  if (i < 0 || i >= NUM_MACROS) return;
  int base = EEPROM_MACROS_START + i * MACRO_SIZE;
  int storeLen = len;
  if (storeLen > MACRO_SIZE - 1) storeLen = MACRO_SIZE - 1;
  EEPROM.write(base, (byte)storeLen); // store length
  for (int j = 0; j < storeLen; ++j) EEPROM.write(base + 1 + j, data[j]);
  // pad rest (optional)
}

// Utility to read macro from EEPROM index i into buffer, returns length
int readMacroFromEEPROM(int i, char* outBuf, int maxlen) {
  if (i < 0 || i >= NUM_MACROS) return 0;
  int base = EEPROM_MACROS_START + i * MACRO_SIZE;
  byte len = EEPROM.read(base);
  if (len == 0xFF) return 0;
  int storeLen = min((int)len, maxlen - 1);
  for (int j = 0; j < storeLen; ++j) outBuf[j] = (char)EEPROM.read(base + 1 + j);
  outBuf[storeLen] = 0;
  return storeLen;
}

void setup() {
  Serial.begin(115200);
  Keyboard.begin();
  Mouse.begin();
  loadFromEEPROM();
  Serial.println("READY");
}

void doDelay(int ms) {
  unsigned long start = millis();
  while (millis() - start < (unsigned long)ms) {
    if (Serial.available()) { /* allow processing serial while delaying if desired */ }
  }
}

void processToken(const String &token) {
  // token examples: PRESS:CTRL, RELEASE:CTRL, TYPE:abc, DELAY:50
  int colon = token.indexOf(':');
  String cmd = (colon == -1) ? token : token.substring(0, colon);
  String arg = (colon == -1) ? "" : token.substring(colon + 1);

  if (cmd == "PRESS") {
    if (arg == "CTRL") Keyboard.press(KEY_LEFT_CTRL);
    else if (arg == "ALT") Keyboard.press(KEY_LEFT_ALT);
    else if (arg == "SHIFT") Keyboard.press(KEY_LEFT_SHIFT);
    else if (arg == "GUI") Keyboard.press(KEY_LEFT_GUI);
    else if (arg.length() == 1) Keyboard.press(arg[0]);
  } else if (cmd == "RELEASE") {
    if (arg == "CTRL") Keyboard.release(KEY_LEFT_CTRL);
    else if (arg == "ALT") Keyboard.release(KEY_LEFT_ALT);
    else if (arg == "SHIFT") Keyboard.release(KEY_LEFT_SHIFT);
    else if (arg == "GUI") Keyboard.release(KEY_LEFT_GUI);
    else if (arg.length() == 1) Keyboard.release(arg[0]);
  } else if (cmd == "TYPE") {
    // send literal characters
    for (int i = 0; i < arg.length(); ++i) {
      Keyboard.write(arg[i]);
    }
  } else if (cmd == "DELAY") {
    int ms = arg.toInt();
    doDelay(ms);
  }
}

void playMacroString(const char* s) {
  // s is semi-colon separated tokens
  String str(s);
  int start = 0;
  while (start < str.length()) {
    int sep = str.indexOf(';', start);
    String token;
    if (sep == -1) {
      token = str.substring(start);
      start = str.length();
    } else {
      token = str.substring(start, sep);
      start = sep + 1;
    }
    token.trim();
    if (token.length() > 0) processToken(token);
  }
}

void loop() {
  static String line;
  if (Serial.available()) {
    line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    if (line.startsWith("SET_SCROLL ")) {
      int v = line.substring(11).toInt();
      if (v < 1) v = 1;
      if (v > 50) v = 50;
      scrollSpeed = v;
      Serial.print("SCROLL ");
      Serial.println(scrollSpeed);
    } else if (line == "GET_SCROLL") {
      Serial.print("SCROLL ");
      Serial.println(scrollSpeed);
    } else if (line == "SAVE") {
      saveToEEPROM();
      Serial.println("OK");
    } else if (line == "LOAD") {
      loadFromEEPROM();
      Serial.println("OK");
    } else if (line.startsWith("SET_MACRO ")) {
      // format: SET_MACRO <i> <length>\n<data bytes>
      int i;
      int sp1 = line.indexOf(' ');
      int sp2 = line.indexOf(' ', sp1 + 1);
      // simpler parse:
      // tokenize quickly:
      int firstSpace = line.indexOf(' ');
      int secondSpace = line.indexOf(' ', firstSpace + 1);
      if (firstSpace > 0 && secondSpace > firstSpace) {
        i = line.substring(firstSpace + 1, secondSpace).toInt();
        int len = line.substring(secondSpace + 1).toInt();
        // now read raw bytes
        char buf[MACRO_SIZE];
        int got = 0;
        unsigned long t0 = millis();
        while (got < len && millis() - t0 < 2000) {
          if (Serial.available()) {
            int c = Serial.read();
            if (c >= 0) {
              if (got < MACRO_SIZE - 1) buf[got++] = (char)c;
            }
          }
        }
        buf[min(got, MACRO_SIZE - 1)] = 0;
        writeMacroToEEPROM(i, buf, got);
        Serial.println("OK");
      } else {
        Serial.println("ERR");
      }
    } else if (line.startsWith("GET_MACRO ")) {
      int i = line.substring(10).toInt();
      char buf[MACRO_SIZE];
      int len = readMacroFromEEPROM(i, buf, MACRO_SIZE);
      Serial.print("MACRO ");
      Serial.print(i);
      Serial.print(" ");
      Serial.println(len);
      if (len > 0) {
        Serial.write((const uint8_t*)buf, len);
      }
    } else if (line.startsWith("PLAY_MACRO ")) {
      int i = line.substring(11).toInt();
      char buf[MACRO_SIZE];
      int len = readMacroFromEEPROM(i, buf, MACRO_SIZE);
      if (len > 0) {
        playMacroString(buf);
        Serial.println("OK");
      } else {
        Serial.println("EMPTY");
      }
    } else {
      Serial.println("UNKNOWN");
    }
  }

  // Example: use scrollSpeed when sending wheel events in your mouse logic
  // e.g., Mouse.move(0, 0, wheelValue * scrollSpeed);
}
