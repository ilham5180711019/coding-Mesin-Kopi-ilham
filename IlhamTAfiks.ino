/*
 * KEBUTUHAN
 * 1. Masuk File > Preference >  Masukan : https://dl.espressif.com/dl/package_esp32_index.json > OK
 * 2. Masuk Tools > Boards > Board Manager > Tuliskan : ESP32 > Install
 * 3. Masuk Tools > Boards > ESP32 Dev Module 
 * 4. Masuk Sketch > Include Library > Manages Library > Ketikan : Blynk > Instal
 * 5. Masuk Sketch > Include Library > Manages Library > Ketikan : ESP32Servo > Instal
 -------------------------+
| SERVO             | ESP |
+-------------------------+
| VIN               | VIN |
| GND               | GND |
| OUT SER. KOPI 1   | D19 |
| OUT SER. KOPI 2   | D18 |
| OUT SER. KOPI 3   | D5  |
| OUT SER. GULA     | D16 |
| OUT SER. KRIMER   | D17 |
+-------------------+-----+
| STEPPER GELAS     | ESP |
+-------------------+-----+
| VIN               | VIN |
| GND               | GND |
| STEP              | D4  |
| DIRECTION         | D2  |
+-------------------+-----+
| STEPPER ADUK      | ESP |
+-------------------+-----+
| IN 1              | VIN |
| IN 2              | GND |
| IN 3              | D4  |
| IN 4              | D2  |
+-------------------+-----+
| IR SENSOR         | ESP |
+-------------------+-----+
| VIN               | VIN |
| GND               | GND |
| OUT               | D35 |
+-----------------+-+-----+
| BLINK           | PIN V |
+-----------------+-------+
| PILIH KOPI      |  V1   |
| PILIH TAMBAHAN  |  V2   |
| LCD             |  V3   |
| TOMBOL          |  V4   |
+-----------------+-------+

*/

#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Stepper.h>

#define BLYNK_AUTH_TOKEN    "7Rj9c5e59aweN5GyzEiXARa39-byDAQK"

// Modul Steper 28BYJ dengan ESP
#define IN1 13 
#define IN2 12 
#define IN3 14 
#define IN4 27 
const int stepsPerRevolution = 1600; // Mengatur putaran steper pengaduk | Maks. 2048 (Satu Putaran)

char auth[] = BLYNK_AUTH_TOKEN;

// Ganti Wifi
char ssid[] = "iPhone";
char pass[] = "ilham12345";

//--PIN STEPER
int stepPin = 4; 
int dirPin  = 2; 

//--PIN IR SENSOR
int IRSensor = 35; //OUT -  D35

//INISIALISASI VARIABEL
int kopi = 5;
int GuladanCream = 5;
int tombol = 0;
int IrStatus;
 
WidgetLCD lcd(V3);//INISIALISASI LCD
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4); //INISIALISASI STEPPER PENGADUK

//INISIALISASI SERVO
Servo Kopi1;
Servo Kopi2;
Servo Kopi3;
Servo Krimer;
Servo Gula;

// Mengatur kopi DARI BLYNK
BLYNK_WRITE(V1) {
  switch (param.asInt())
  {
    case 1: // Item 1
      Serial.println("Kopi 1");
      kopi  = 1;
      break;
    case 2: // Item 2
      Serial.println("Kopi 2");
      kopi  = 2;
      break;
    case 3: // Item 3
      Serial.println("Kopi 2");
      kopi = 3;
      break;
    default:
      kopi = 5;
      Serial.println("Unknown item selected");
  }
}

//Mengatur TAMBAHAN DARI BLYNK
BLYNK_WRITE(V2) {
  switch (param.asInt())
  {
    case 1: // Item 1
      Serial.println("Tambah Gula");
      GuladanCream = 1;
      break;
    case 2: // Item 2
      Serial.println("Tambah Cream ");
      GuladanCream = 2;
      break;
    case 3: // Item 3
      Serial.println("Tambah Gula+Cream");
      GuladanCream = 3;
      break;
    case 4: // Item 4
      Serial.println("Tanpa Keduannya");
      GuladanCream = 4;
      break;
    default:
    GuladanCream = 5;
      Serial.println("Unknown item selected");
  }
}


//MENGATUR TOMBOL START DARI BLINK
BLYNK_WRITE(V4) {
  tombol = param.asInt();
  
  if (GuladanCream <=4){
    if ((tombol == 1)&&(IrStatus == 1)){
    lcd.clear();
    lcd.print(1, 0, "Mohon Masukan");
    lcd.print(1, 1, "   Gelas");
    delay(1000);
    Reset();
    }
    else if ((tombol == 1)&&(IrStatus == 0)){
      switch (kopi)
      {
        case 1: 
        lcd.clear();
        Serial.println("1. kopi1");
        layar();
          Steper30(); //STEPER KE KOPI 1
          delay(2000);
//--------------------------------------------------------
//        Koding Servo Kopi 1 
//        Atur delay() untuk membuka lama / cepat
//--------------------------------------------------------
          Kopi1.write(120);
          delay (1000);
          Kopi1.write(0);
          delay (1000);
//--------------------------------------------------------
          Steper150(); // STEPER KE SUDUT 180 DERAJAT
         // delay(5);

          Tambahan(); // MEMANGGIL KODING TAMBAHAN GULA / KRIMER
          AirdanAduk(); // MEMANGGIL KODING MNGADUK
          Steper30();
          lcd.clear();
          lcd.print(1, 0, "PESANAN");
          lcd.print(1, 1, "SELESAI");     
          Reset();
          break;
          
        case 2:
        lcd.clear();
        Serial.println("1. kopi2");
        layar();
          Steper90(); // STEPER KE KOPI 2
          delay(2000);
//--------------------------------------------------------
//        Koding Servo ke Kopi 2
//        Atur delay() untuk membuka lama / cepat
//--------------------------------------------------------
          Kopi2.write(120);
          delay (1000); 
          Kopi2.write(0);
          delay (1000);
//--------------------------------------------------------
          Steper90(); // STEPER KE SUDUT 180 DERAJAT
         // delay(5);

          Tambahan(); 
          AirdanAduk();
          Steper30();
          lcd.clear();
          lcd.print(1, 0, "PESANAN");
          lcd.print(1, 1, "SELESAI");
          Reset();
          break;

        case 3:
        lcd.clear();
        Serial.println("1. kopi3");
        layar();
          Steper150();//STEPER KE KOPI 3
          delay(2000);
//--------------------------------------------------------
//        Koding Servo Kopi 3
//        Atur delay() untuk membuka lama / cepat
//--------------------------------------------------------
          Kopi3.write(120);
          delay (1000);
          Kopi3.write(0);
          delay (1000);
//--------------------------------------------------------
          Steper30();// STEPER KE SUDUT 180 DERAJAT
        //  delay(5);
   
          Tambahan(); 
          AirdanAduk();
          Steper30();
          lcd.clear();
          lcd.print(1, 0, "PESANAN");
          lcd.print(1, 1, "SELESAI");
          Reset();
          break;
          
        case 5:
          Serial.println("KOPI BELUM DIISI");
          lcd.clear();
          lcd.print(1, 0, "MOHON MENGISI");
          lcd.print(1, 1, "MENU KOPI");
          Reset();
        default:  
          Reset();
        }
        
  }
  }
  else{
      lcd.clear();
      lcd.print(1, 0, "MOHON MENGISI");
      lcd.print(1, 1, "MENU TAMBAHAN");
      Reset(); 
  }
}

void setup()
{
  Serial.begin(115200);

 pinMode(stepPin, OUTPUT); 
 pinMode(dirPin,  OUTPUT); 

 pinMode (IRSensor, INPUT);
  
 digitalWrite(stepPin, LOW); 
 digitalWrite(dirPin, LOW);

 tombol = 0;
  GuladanCream = 0;
  myStepper.setSpeed(5);
  
Kopi1.attach(19);
Kopi2.attach(18);
Kopi3.attach(5);
Krimer.attach(17);
Gula.attach(16);
Kopi1.write(0);
Kopi2.write(0);
Kopi3.write(0);
Krimer.write(0);
Gula.write(0);

 Blynk.begin(auth, ssid, pass);
 
}

void loop(){
IrStatus = digitalRead (IRSensor);
Serial.print("IRsensor");
Serial.println(IrStatus);
if(IrStatus == 1){
  Serial.println("masukan gelas");
  lcd.clear();
  lcd.print(1, 0, "Masukan");
  lcd.print(1, 1, "Gelas");
  Blynk.run();
}else if (IrStatus == 0){
  lcd.clear();
  lcd.print(1, 0, "Menunggu");
  lcd.print(1, 1, "Pesanan");
  Blynk.run();
}
}

/// Air dan Pengaduk
void AirdanAduk(){
  Serial.println("3.Menambah Air dan Mengaduk");
  lcd.clear();
  lcd.print(1, 0, "Proses");
  lcd.print(1, 1, "Mixing");

    myStepper.step(stepsPerRevolution); //steper pengaduk turun
    delay(500);

//--------------------------------------------------------
//  Atur delay() untuk lamanya menambah air dan mengaduk
//--------------------------------------------------------
    Serial.println("proses mengaduk");
    delay(5000);
//--------------------------------------------------------
    
    Serial.println("counterclockwise");
    myStepper.step(-stepsPerRevolution); //steper pengaduk naik

  //delay(2000);
 // Reset();
  lcd.clear();
}
void layar(){
  lcd.clear();
  lcd.print(1, 0, "Proses");
  lcd.print(1, 1, "Pembuatan");
  
  }
void Reset(){
// Mengembalikan nilai  blynk ke netral / 0
  Blynk.virtualWrite(V1, "Pilih Kopi Anda", 0); 
  Blynk.virtualWrite(V2, "Tambahan Gula?", 0); 
  Blynk.virtualWrite(V4,LOW);
  tombol = 0;
  GuladanCream = 5;
  kopi = 5;
  Serial.println("4. Reset");
  delay(5000);
}

void Tambahan(){
  if (GuladanCream == 1){
    Serial.println("2. Tambah Gula");
    Steper30();     //Dari SUdut 180 derajat STEPPER PUTAR ke Gula
    delay(2000);
//MOTOR SERVO GULA AKTIF
    Gula.write(120); 
    delay (1000);
    Gula.write(0);
    delay (1000);
    
    Steper120();//Dari GULA STEPPER PUTAR ke PENGADUK
    delay(2000);
  }else if(GuladanCream == 2){
    Serial.println("2. Tambah Cream");
    Steper90(); //Dari SUdut 180 derajat STEPPER PUTAR ke KRIMER
//MOTOR SERVO KRIMER AKTIF
    delay(2000);
    Krimer.write(120);
    delay (1000);
    Krimer.write(0);
    delay (1000);
    Steper60();//Dari KRIMER STEPPER PUTAR ke PENGADUK
    delay(2000);
  }else if(GuladanCream == 3){
    Serial.println("2. Tambah Cream + Gula");
    Steper30();//Dari SUdut 180 derajat STEPPER PUTAR ke Gula
//MOTOR SERVO GULA AKTIF
    delay(2000);
    Gula.write(120);
    delay (1000);
    Gula.write(0);
    delay (1000);
    Steper60(); //Dari GULA, STEPPER PUTAR ke KRIMER
    delay(2000);
//MOTOR SERVO KRIMER AKTIF
    Krimer.write(120);
    delay (1000);
    Krimer.write(0);
    delay (1000); 
    Steper60();//Dari GULA STEPPER PUTAR ke PENGADUK
    delay(2000);
  }else if(GuladanCream == 4){
    Serial.println("2. Tanpa Keduannya");
    Steper150();//Dari GULA STEPPER PUTAR ke PENGADUK
    delay(2000);
  }else{
    Serial.println("2. Gagal gula kopi");
  lcd.print(1, 0, "MOHON MENGISI");
  lcd.print(1, 1, "SELURUH MENU");
  }
}


void Steper30(){
  digitalWrite(dirPin,HIGH);
  for(int x=0;x<32;x++){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(5000);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(5000);
  }
}
void Steper60(){
  digitalWrite(dirPin,HIGH);
  for(int x=0;x<64;x++){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(5000);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(5000);
  }
}
void Steper90(){
  digitalWrite(dirPin,HIGH);
  for(int x=0;x<96;x++){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(5000);
    digitalWrite(stepPin,LOW);
   delayMicroseconds(5000);
  }
}
void Steper120(){
  digitalWrite(dirPin,HIGH);
  for(int x=0;x<128;x++){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(5000);
    digitalWrite(stepPin,LOW);
   delayMicroseconds(5000);
  }
}
void Steper150(){
  digitalWrite(dirPin,HIGH);
  for(int x=0;x<160;x++){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(5000);
    digitalWrite(stepPin,LOW);
   delayMicroseconds(5000);
  }
}
void Steper180(){
  digitalWrite(dirPin,HIGH);
  for(int x=0;x<192;x++){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(5000);
    digitalWrite(stepPin,LOW);
   delayMicroseconds(5000);
  }
}
