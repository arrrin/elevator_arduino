// IR 3 ตัว
int IR01 = 2;
int IR02 = 3;
int IR03 = 4;
int valIR01 = 0;
int valIR02 = 0;
int valIR03 = 0;

int floor0 = 13;
int floor1 = 12;
int floor2 = 11;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // กำหนดขา IR
  pinMode(IR01, INPUT);
  pinMode(IR02, INPUT);
  pinMode(IR03, INPUT);

  pinMode(floor0, OUTPUT);
  pinMode(floor1, OUTPUT);
  pinMode(floor2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  valIR01 = digitalRead(IR01);  //อ่านค่าสัญญาณ digital ขา8 ที่ต่อกับ เซ็นเซอร์ตรวจจับวัตถุ IR Infrared
  valIR02 = digitalRead(IR02);
  valIR03 = digitalRead(IR03);

//  Serial.print("valIR01 = "); // พิมพ์ข้อมความส่งเข้าคอมพิวเตอร์ "val = "
//  Serial.println(valIR01); // พิมพ์ค่าของตัวแปร val
//  Serial.print("valIR02 = "); // พิมพ์ข้อมความส่งเข้าคอมพิวเตอร์ "val = "
//  Serial.println(valIR02); // พิมพ์ค่าของตัวแปร val
//  Serial.print("valIR03 = "); // พิมพ์ข้อมความส่งเข้าคอมพิวเตอร์ "val = "
//  Serial.println(valIR03); // พิมพ์ค่าของตัวแปร val

  if (valIR01 == 0) { // ค่า เป็น 0  ตรวจจับเจอวัตถุ 
    Serial.println("1");
    digitalWrite(floor0,1);
    digitalWrite(floor1,0);
    digitalWrite(floor2,0);
  }else if (valIR02 == 0){
    Serial.println("2");
    digitalWrite(floor0,0);
    digitalWrite(floor1,1);
    digitalWrite(floor2,0);
  }else if (valIR03 == 0){
    Serial.println("3");
    digitalWrite(floor0,0);
    digitalWrite(floor1,0);
    digitalWrite(floor2,1);
  }else{
    Serial.println("Don't have");
  }
  delay(10);
  
}
