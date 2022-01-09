/**
 * config.h
 * ========
 * 
 * Basis Konfiguration für die genutzen Pins sowie die verwendeten Konstanten und Variablen.
 * 
 * Copyright (c) Claudio Haldner, 28.12.2021  
 * 
*/

#include <Arduino.h>

const int INTERRUPT_PIN = 18;
const int SS_PIN = 53;
const int RST_PIN = 10;
const int MAIN_GATE_PIN = 12;
const int VAULT_GATE_PIN = 30;
const int VAULT_GATE_LED_PIN = 13;
const int ALARM_PIN = 22;

const byte COLS = 4;
const byte ROWS = 4;
const char KEYPAD_START_CODE_INPUT = '*';
const char KEYPAD_ABORT_CODE_INPUT = '#';
const char KEYPAD_LOCK_COMMAND = 'D';

const String PASSPHRASE = "A180";
const String RFID_CODE = "156463074";

const int VAULT_GATE_SERVO_POSITION_OPEN = 180;
const int VAULT_GATE_SERVO_POSITION_CLOSED = 0;

bool isAlarmMode = false;
bool isVaultUnlocked = false;
char hexaKeys[COLS][ROWS] = {
  {'D','#','0','*'},
  {'C','9','8','7'},
  {'B','6','5','4'},
  {'A','3','2','1'},
};
byte colPins[COLS] = {2, 3, 4, 5};
byte rowPins[ROWS] = {6, 7, 8, 9};