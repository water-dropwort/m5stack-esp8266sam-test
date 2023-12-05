#include <M5Unified.h>
#include <AudioOutputI2S.h>
#include <ESP8266SAM.h>

#define LCD_WIDTH 320
#define LCD_HEIGHT 240
const int CIRCLE_R = 30;
const int CIRCLE_X = LCD_WIDTH / 2;
const int CIRCLE_Y = LCD_HEIGHT / 2;

AudioOutputI2S *out;
ESP8266SAM *sam;
ESP8266SAM::SAMVoice voices[] = {
  ESP8266SAM::SAMVoice::VOICE_SAM,
  ESP8266SAM::SAMVoice::VOICE_ELF,
  ESP8266SAM::SAMVoice::VOICE_ET,
  ESP8266SAM::SAMVoice::VOICE_OLDLADY,
  ESP8266SAM::SAMVoice::VOICE_ROBOT,
  ESP8266SAM::SAMVoice::VOICE_STUFFY,
};
const int LENGTH_VOICES = 6;
int current_voice = 0;

const char* messages[] = {
  "Hello, world.",
  "Trendy!",
  "M 5 stack core 2.",
  "The current temperature is twenty five point six degrees.",
};
const int LENGTH_MESSAGES = 4;
int current_message = 0;

const char* VOICE_CHANGED_MSG = "Voice was changed to ";

#define OUTPUT_GAIN 8

void sam_say(const char*);
const char* get_voice_name(ESP8266SAM::SAMVoice);
void draw_voice_name(const char*);

bool isSing = false;

void setup() {
  M5.begin();
  M5.Power.Axp192.setGPIO2(true);

  M5.Lcd.drawCircle(CIRCLE_X, CIRCLE_Y, CIRCLE_R, WHITE);
  M5.Lcd.setTextFont(1);
  M5.Lcd.setTextSize(5);

  delay(500);
  out = new AudioOutputI2S(0, 0);
  out->SetPinout(12, 0, 2);
  out->SetOutputModeMono(true);
  out->SetGain((float)OUTPUT_GAIN/100.0);

  sam = new ESP8266SAM();
  sam->SetVoice(voices[current_voice]);
  draw_voice_name(get_voice_name(voices[current_voice]));
  sam->SetSingMode(isSing);
}

void sam_say(const char* message) {
  M5.Lcd.fillCircle(CIRCLE_X, CIRCLE_Y, CIRCLE_R - 1, MAGENTA);
  sam->Say(out, message);
  M5.Lcd.fillCircle(CIRCLE_X, CIRCLE_Y, CIRCLE_R - 1, BLACK);
}

const char* get_voice_name(ESP8266SAM::SAMVoice voice) {
  switch(voice) {
    case ESP8266SAM::SAMVoice::VOICE_ELF:
      return "Elf";
    case ESP8266SAM::SAMVoice::VOICE_ET:
      return "E.T.";
    case ESP8266SAM::SAMVoice::VOICE_OLDLADY:
      return "Old lady";
    case ESP8266SAM::SAMVoice::VOICE_ROBOT:
      return "Robot";
    case ESP8266SAM::SAMVoice::VOICE_SAM:
      return "Sam";
    case ESP8266SAM::SAMVoice::VOICE_STUFFY:
      return "Stuffy";
    default:
      return "Undefined";
  }
}

void draw_voice_name(const char* voice_name) {
  M5.Lcd.fillRect(0, 0, LCD_WIDTH, 40, BLACK);
  M5.Lcd.drawString(voice_name, 0, 0);
}

void loop() {
  if(M5.BtnA.wasPressed()) {
    sam_say(messages[current_message]);
    current_message = (current_message + 1) % LENGTH_MESSAGES;
  }
  else if(M5.BtnB.wasPressed()) {
    current_voice = (current_voice + 1) % LENGTH_VOICES;
    sam->SetVoice(voices[current_voice]);
    const char* voice_name = get_voice_name(voices[current_voice]);
    draw_voice_name(voice_name);
    char msg[strlen(VOICE_CHANGED_MSG) + strlen(voice_name) + 1];
    strcpy(msg, VOICE_CHANGED_MSG);
    strcat(msg, voice_name);
    strcat(msg, ".");
    sam_say(msg);
  }
  else if(M5.BtnC.wasPressed()) {
    isSing = !isSing;
    sam->SetSingMode(isSing);
    if(isSing) {
      M5.Lcd.drawString("Sing", 200, LCD_HEIGHT - 40);
      sam_say("Sing mode was enabled.");
    }
    else {
      M5.Lcd.fillRect(200, LCD_HEIGHT - 40, LCD_WIDTH - 120, 40, BLACK);
      sam_say("Sing mode was disabled.");
    }
  }
  else {
  }
  M5.update();
}
