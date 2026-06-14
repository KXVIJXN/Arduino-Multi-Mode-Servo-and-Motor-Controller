#include <Servo.h>
#include <LiquidCrystal.h>

//lcd
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

//inputs
#define VRX A0
#define VRY A1
#define MOTOR 3
#define BUTTON 2

//servo
Servo myServo;

//state
int mode = 1;

//button
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

//manual
float currentFan = 0;

//auto sweep
int sweepAngle = 90;
int sweepDir = 1;
unsigned long lastSweepTime = 0;
const int sweepDelay = 30;

void setup()
{
  pinMode(MOTOR, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  myServo.attach(6);
  myServo.write(90);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("SYSTEM READY");
  delay(800);
  lcd.clear();
}

void handleButton()
{
  bool button = digitalRead(BUTTON);

  if (button == LOW && lastButtonState == HIGH &&
      millis() - lastDebounceTime > 300)
  {
    mode++;
    if (mode > 2) mode = 0;
    lastDebounceTime = millis();
  }

  lastButtonState = button;
}

void runOff()
{
  myServo.write(90);
  analogWrite(MOTOR, 0);

  lcd.setCursor(0, 0);
  lcd.print("MODE: OFF      ");
  lcd.setCursor(0, 1);
  lcd.print("SYSTEM STOPPED ");
}

void runManual()
{
  int xVal = analogRead(VRX);
  int yVal = analogRead(VRY);

  int angle = map(xVal, 0, 1023, 20, 160);
  myServo.write(angle);

  int targetFan = map(yVal, 0, 1023, 0, 255);
  targetFan = constrain(targetFan, 0, 255);

  currentFan += (targetFan - currentFan) * 0.1;
  analogWrite(MOTOR, (int)currentFan);

  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print(angle);
  lcd.print(" F:");
  lcd.print((int)currentFan);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  lcd.print("MODE: MANUAL   ");
}

void runAuto()
{
  if (millis() - lastSweepTime >= sweepDelay)
  {
    lastSweepTime = millis();

    sweepAngle += sweepDir;

    // SAFE RADAR RANGE FIX
    if (sweepAngle >= 160 || sweepAngle <= 20)
    {
      sweepDir = -sweepDir;
    }

    myServo.write(sweepAngle);
  }

  int autoFan = map(sweepAngle, 20, 160, 0, 255);
  analogWrite(MOTOR, autoFan);

  lcd.setCursor(0, 0);
  lcd.print("AUTO S:");
  lcd.print(sweepAngle);
  lcd.print(" F:");
  lcd.print(autoFan);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  lcd.print("MODE: AUTO     ");
}

void loop()
{
  handleButton();

  if (mode == 0) runOff();
  else if (mode == 1) runManual();
  else if (mode == 2) runAuto();

  delay(20);
}