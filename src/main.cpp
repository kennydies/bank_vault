/**
 * main.cpp
 * ========
 * 
 * Projektarbeit Bank Vault.
 * 
 * Copyright (c) Claudio Haldner, 28.12.2021  
 * 
*/

#include <Arduino.h>
#include <Keypad.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <config.h>
#include <function_hoisting.h>

LiquidCrystal_I2C LcdOutput(0x27, 16, 2);
Servo VaultLockServo;
MFRC522 mfrc522(SS_PIN, RST_PIN);
Keypad KeypadInput = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), setAlarmFlag, RISING);

  pinMode(ALARM_PIN, OUTPUT);
  pinMode(MAIN_GATE_PIN, OUTPUT);
  pinMode(VAULT_GATE_LED_PIN, OUTPUT);

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  VaultLockServo.attach(VAULT_GATE_PIN);
  VaultLockServo.write(VAULT_GATE_SERVO_POSITION_CLOSED);

  LcdOutput.init();
  LcdOutput.backlight();

  displayClear();
  displayWrite("SYSTEM UP");
}

void loop() {
  if (isAlarmMode) {
    alarm();
  } else {
    checkMainGate();
    checkVaultGate();
  }
}

void displayWrite(String message) {
  LcdOutput.print(message);
}

void displayNewLine() {
  LcdOutput.setCursor(0, 1);
}

void displayClear() {
  LcdOutput.clear();
  LcdOutput.setCursor(0, 0);
}

void setAlarmFlag() {
    isAlarmMode = true;
}

void alarm() {
  while (isAlarmMode == true) {
    vaultLockClose();
    mainGateClose();
    displayNewLine();
    displayWrite("Alarm!");

    digitalWrite (ALARM_PIN, HIGH);
    delay(3000);
    digitalWrite (ALARM_PIN, LOW);

    displayClear();
    displayWrite("SYSTEM UP");
    isAlarmMode = false;
  }    
}

void vaultLockOpen() {
  isVaultUnlocked = true;
  digitalWrite (VAULT_GATE_LED_PIN, HIGH);
  displayClear();
  displayWrite("Vault Unlocked!");
  displayNewLine();
  displayWrite("Maingate Locked!");
  VaultLockServo.write(VAULT_GATE_SERVO_POSITION_OPEN);
}

void vaultLockClose() {
  isVaultUnlocked = false;
  digitalWrite (VAULT_GATE_LED_PIN, LOW);
  displayClear();
  displayWrite("Vault LOCKED!");
  VaultLockServo.write(VAULT_GATE_SERVO_POSITION_CLOSED);
}

void mainGateOpen() {
  digitalWrite(MAIN_GATE_PIN, HIGH);

}

void mainGateClose() {
  digitalWrite(MAIN_GATE_PIN, LOW);  
}

void checkMainGate() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() && isVaultUnlocked == false) {
    String rfidCodeInput = "";

    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rfidCodeInput += String(mfrc522.uid.uidByte[i]);
    }
    
    if (rfidCodeInput == RFID_CODE) {
      displayClear();
      displayWrite("Access GRANTED!");

      mainGateOpen();
      delay(3000);
      mainGateClose();

      displayClear();
      displayWrite("SYSTEM UP");
    }
  }
}

void checkVaultGate() {
  char keypadInputChar = KeypadInput.getKey();

  if (keypadInputChar) {
    if (keypadInputChar == KEYPAD_START_CODE_INPUT) {
      String codeInput = "";

      displayClear();
      displayWrite("Enter Code");
      displayNewLine();

      for (unsigned int i = 0; i <= PASSPHRASE.length(); i++) {
        char passphraseInput = KeypadInput.getKey();

        while (passphraseInput == NO_KEY) {
          passphraseInput = KeypadInput.getKey();
        }

        if (i == PASSPHRASE.length()) {      
          if (codeInput == PASSPHRASE && passphraseInput == KEYPAD_ABORT_CODE_INPUT) {
            vaultLockOpen();
          } else {
            vaultLockClose();
          }
          break;
        } else if (passphraseInput == KEYPAD_ABORT_CODE_INPUT) {
          vaultLockClose();
          break;
        } else {
          displayWrite("*");
          codeInput += passphraseInput;
        }
      }
    } else if (keypadInputChar == KEYPAD_LOCK_COMMAND) {
      vaultLockClose();
    }    
  }
}