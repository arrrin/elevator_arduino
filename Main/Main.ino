#include <Servo.h>
Servo myservo; //ประกาศตัวแปรแทน Servoกำหนดขา
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int IR01 = 4;
int IR02 = 3;
int IR03 = 2;

int motorPin1 = A3;   // Blue   - 28BYJ48 pin 1
int motorPin2 = A2;   // Pink   - 28BYJ48 pin 2
int motorPin3 = A1;  // Yellow - 28BYJ48 pin 3
int motorPin4 = A0;
int motorSpeed = 5;

char incomingByte = 0;
int choice = 0;

const int buttonOpen = 8;
const int buttonClose = 9;
int buttonStateOpen = 0;
int buttonStateClose = 0;

int receive_bits0 = 7;
int receive_bits1 = 6;

int q0Pin = 10;
int q1Pin = 11;
int q2Pin = 12;
int q3Pin = 13;

int q0 = 0;
int q1 = 0;
int q2 = 0;
int q3 = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  lcd.init(); // initialize the lcd
  lcd.backlight();

  lcd.home();
  myservo.attach(5);// กำหนดขาควบคุม Servo
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  myservo.write(10);
  //กำหนดขาปุ่ม close/open
  pinMode(buttonOpen, INPUT);
  pinMode(buttonClose, INPUT);

  // กำหนดขา Request from FPGA
  pinMode(q0Pin, INPUT);
  pinMode(q1Pin, INPUT);
  pinMode(q2Pin, INPUT);
  pinMode(q3Pin, INPUT);

  pinMode(IR01, INPUT);
  pinMode(IR02, INPUT);
  pinMode(IR03, INPUT);


  pinMode(receive_bits0, OUTPUT);
  pinMode(receive_bits1, OUTPUT);
}

void loop()
{

  buttonStateOpen = digitalRead(buttonOpen);
  buttonStateClose = digitalRead(buttonClose);

    Serial.print("buttonStateOpen = ");
    Serial.println(buttonStateOpen);
    Serial.print("buttonStateClose = ");
    Serial.println(buttonStateClose);

  q0 = digitalRead(q0Pin);
  q1 = digitalRead(q1Pin);
  q2 = digitalRead(q2Pin);
  q3 = digitalRead(q3Pin);
//  Serial.print(q3);
//  Serial.print(q2);
//  Serial.print(q1);
//  Serial.println(q0);
  if (q3 == 0 && q2 == 0 && q1 == 1 && q0 == 0) { //0010
    lcd.clear(); //เคลียร์จอ LCD 
    lcd.print("   Opening <>  "); // แสดงผล Opening <> ออกทางจอ LCD
    OpenDoor();  // servo ทำงาน หมุนเปิดประตู
    delay(2000);  // รอให้คนเดินเข้าไปใน lift  
    buttonStateOpen = digitalRead(buttonOpen); // อ่านว่ามีการกดปุ่ม Open ที่อยู่ภายในลิฟต์ไหม -> เป็น 0 ไม่กด เป็น 1 กด
    buttonStateClose = digitalRead(buttonClose);
    while (buttonStateOpen == 1 && buttonStateClose == 0){
      lcd.clear();
      lcd.print("   Opening <>  ");
      delay(2000);
      buttonStateOpen = digitalRead(buttonOpen);
      buttonStateClose = digitalRead(buttonClose);
    }
    buttonStateOpen = digitalRead(buttonOpen);
    buttonStateClose = digitalRead(buttonClose);
    if(buttonStateOpen == 0 && buttonStateClose == 1){
      lcd.clear();
      lcd.print("   Closing ><   ");
      CloseDoor();
    }
    else if (buttonStateOpen == 0 && buttonStateClose == 0){
      lcd.clear();
      lcd.print("   Closing ><   ");
      delay(1000);  
      CloseDoor();
    }
    digitalWrite(receive_bits0, 1);
    digitalWrite(receive_bits1, 1);
  }
  else if (q3 == 0 && q2 == 0 && q1 == 1 && q0 == 1) { //0011
    lcd.clear();
    lcd.print("     Request    ");
  }
  else if (q3 == 0 && q2 == 1 && q1 == 1 && q0 == 0) { //0110
    lcd.clear();
    lcd.print("    Going Up    ");
    if (digitalRead(IR01) == 1) {
      Upfloor1to2();
    }
    else if (digitalRead(IR02) == 1) {
      UPfloor2to3();
    }
    digitalWrite(receive_bits0, 0);
    digitalWrite(receive_bits1, 1);
  }
  else if (q3 == 0 && q2 == 1 && q1 == 0 && q0 == 1) { //0101
    lcd.clear();
    lcd.print("   Going Down   ");
    // motor หมุนลงตาม block

    if (digitalRead(IR03) == 1) {
      Downfloor3to2();
    }
    else if (digitalRead(IR02) == 1) {
      Downfloor2to1();
    }
    //Serial.println("Down success");
    digitalWrite(receive_bits0, 0);
    digitalWrite(receive_bits1, 1);
  }
  else if (q3 == 0 && q2 == 1 && q1 == 0 && q0 == 0) { //0100
    lcd.clear();
    lcd.print("    Finished    ");
    // servo  หมุนเปิดประตู
    lcd.clear();
    lcd.print("   Opening <>  ");
    OpenDoor();
    delay(5000);  // รอให้คนเดินเข้าไปใน lift  ให้เสร็จก่อน
    buttonStateOpen = digitalRead(buttonOpen);
    buttonStateClose = digitalRead(buttonClose);
    while (buttonStateOpen == 1 && buttonStateClose == 0){
      lcd.clear();
      lcd.print("   Opening <>  ");
      delay(2000);
      buttonStateOpen = digitalRead(buttonOpen);
      buttonStateClose = digitalRead(buttonClose);
    }
    // servo  หมุนปิดประตู
    lcd.clear();
    lcd.print("   Closing ><   ");
    CloseDoor();
    digitalWrite(receive_bits0, 1);
    digitalWrite(receive_bits1, 0);
  }
  else if (q3 == 0 && q2 == 0 && q1 == 0 && q0 == 1) { //0001
    lcd.clear();
    lcd.print("      Idle      ");
  }
  else if (q3 == 1 && q2 ==1 && q1 == 0 && q0 == 0) { //0=1100 //Down
    lcd.clear();
    lcd.print("   Opening <>  ");
    OpenDoor();
    lcd.clear();
    lcd.print("   Closing ><   ");
    delay(4000);  
    CloseDoor();
    digitalWrite(receive_bits0,1);
    digitalWrite(receive_bits1, 1);
      
    }
    else if (q3 == 1 && q2 == 0 && q1 == 1 && q0 == 0) { //1010 //Down
    lcd.clear();
    lcd.print("   Opening <>  ");
    OpenDoor();
    lcd.clear();
    lcd.print("   Closing ><   ");
    delay(4000);  
    CloseDoor();
    digitalWrite(receive_bits0, 1);
    digitalWrite(receive_bits1, 1);
    }
  else {
    lcd.clear();
    lcd.print("      Idle      ");
  }
}

void CloseDoor() {
  myservo.write(10);
  delay(1000);
}
void OpenDoor() {
  myservo.write(180);
  delay(1000);
}
void Upfloor1to2() {
  for (int x = 0; x < 1000; x += 1) {
    // 1
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
    delay(motorSpeed);
    // 2
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(motorSpeed);
    // 3
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(motorSpeed);
    // 4
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, HIGH);
    delay(motorSpeed);
  }
}
void UPfloor2to3() {
  for (int x = 0; x < 760; x += 1) {
    // 1
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
    delay(motorSpeed);
    // 2
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(motorSpeed);
    // 3
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(motorSpeed);
    // 4
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, HIGH);
    delay(motorSpeed);
  }
}
void Downfloor2to1() {
  for (int x = 0; x < 1000; x += 1) {
    // 1
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, HIGH);
    delay(motorSpeed);
    // 2
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(motorSpeed);
    // 3
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(motorSpeed);
    // 4
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
  }
}
void Downfloor3to2() {
  for (int x = 0; x < 770; x += 1) {
    // 1
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, HIGH);
    delay(motorSpeed);
    // 2
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    delay(motorSpeed);
    // 3
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(motorSpeed);
    // 4
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
  }
}
