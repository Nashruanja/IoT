#include <Servo.h>  //library Servo
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "CTBot.h"
CTBot myBot;

//Telegram dan Wifi
char ssid[] = "DRAGON";                                            //nama wifi
char password[] = "qwerty1234";                                    //password wifi
String token = "6010383923:AAFccCOnS2aAImT4pphBsltxL26zZqEiXQw" ; //token bot telegram
const int id = 1363753003 ;                                     //idbot

WiFiClientSecure client;

Servo myservo1;
Servo myservo2;

//motor servo
int sudut = 85;  //berputar sampai 75 drajat
int normal = 0;  //berputar kembali

//sensor Air hujan
const int sensor_hujan = 2;  // Hubungan ke pin D4

//Ultrasonik Kanan
#define triggerPin_kanan 15  // Pin yang terhubung ke trigger ke D8
#define echoPin_kanan 13     // Pin yang terhubung ke echo ke D7
//Ultrasonik Kiri
#define triggerPin_kiri 12  // Pin yang terhubung ke trigger ke D6
#define echoPin_kiri 14     // Pin yang terhubung ke echo ke D5

void setup() {
  //motor servo1
  myservo1.attach(0);  //Hubungan ke pin D3
  myservo2.attach(4);  //Hubungan ke pin D2
  // Mengatur posisi awal servo2
  myservo1.write(normal);
  myservo2.write(sudut);
  //serial print
  Serial.begin(115200);
  //Ultrasonik kanan
  pinMode(triggerPin_kanan, OUTPUT);
  pinMode(echoPin_kanan, INPUT);
  //Ultrasoik Kiri
  pinMode(triggerPin_kiri, OUTPUT);
  pinMode(echoPin_kiri, INPUT);
  //Sensor Air Hujan
  pinMode(sensor_hujan, INPUT);
  //Telegram dan Wifi
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(5000);

  Serial.println("Starting TelegramBot...");
  myBot.wifiConnect(ssid, password);
  myBot.setTelegramToken(token);

  if (myBot.testConnection()) {
    Serial.println("Koneksi Bagus");
  } else {
    Serial.println("Koneksi Jelek");
  }
}

void loop() {
  long duration_kanan, jarak_kanan;  // Variabel untuk menyimpan durasi dan jarak sisi kanan
  long duration_kiri, jarak_kiri;    // Variabel untuk menyimpan durasi dan jarak sisi kiri

  // Mengukur jarak sisi kanan
  digitalWrite(triggerPin_kanan, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin_kanan, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin_kanan, LOW);
  duration_kanan = pulseIn(echoPin_kanan, HIGH);
  jarak_kanan = (duration_kanan / 2) / 29.1;

  // Mengukur jarak sisi kiri
  digitalWrite(triggerPin_kiri, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin_kiri, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin_kiri, LOW);
  duration_kiri = pulseIn(echoPin_kiri, HIGH);
  jarak_kiri = (duration_kiri / 2) / 29.1;

  // Konversi jarak ke meter
  float jarak_kanan_meter = jarak_kanan / 100.0;
  float jarak_kiri_meter = jarak_kiri / 100.0;

  // Menampilkan hasil pembacaan jarak
  Serial.println("Jarak Ultrasonik Kanan:");
  Serial.print(jarak_kanan_meter);
  Serial.println(" meter");

  Serial.println("Jarak Ultrasonik Kiri:");
  Serial.print(jarak_kiri_meter);
  Serial.println(" meter");

  //Sensor Air Hujan
  int air = digitalRead(sensor_hujan);  //Baca sensor

  if (air == LOW) {        //jika terdeteksi ada air
    myservo1.write(sudut);  //servo berputar 85 drajat
    //delay(1 * 5000);     // Delay selama 5 detik (1 x 5000 milisecond)
    myservo2.write(normal);  //servo berputar 85 drajat
    //delay(1 * 5000);     // Delay selama 5 detik (1 x 5000 milisecond)
  }
  //Mengembalikan posisi alat setelah tidak ada air
  else if (jarak_kanan_meter > 0.25 && myservo1.read() == sudut && myservo2.read() == normal) { // Jika Ultrasonik Kanan mendeteksi jarak lebih dari 25 cm dan servo diatas maka akan kembali ke posisi normal alat
    myservo1.write(normal);  //servo Kembali
    myservo2.write(sudut);  //servo kembali
  }
  //Notifikasi Sumbatan Kanan
  else if (jarak_kanan_meter <= 1) { // jika ultrasonik kanan mendeteksi sumbatan kurang dari 1 M
    String kanan = "TERDAPAT SUMBATAN PADA SISI KANAN!\n";
    kanan += "Jarak Sumbatan Pada Sisi Kanan : ";
    kanan += int(jarak_kanan_meter);
    kanan += " M\n";
    myBot.sendMessage(id, kanan, "");
    Serial.println("Mengirim data sensor ke telegram -Ultrasonik Kanan- ");
  }
  //Notifikasi Sumbatan Kiri
  else if (jarak_kiri_meter <= 1 && myservo1.read() != sudut && myservo2.read() != normal) { // jika ultrasonik Kiri mendeteksi sumbatan lebih dari 1 M dan servo tidak disudut
    String kiri = "TERDAPAT SUMBATAN PADA SISI KIRI!\n";
    kiri += "Jarak Sumbatan Pada Sisi Kiri : ";
    kiri += int(jarak_kiri_meter);
    kiri += " M\n";
    myBot.sendMessage(id, kiri, "");
    Serial.println("Mengirim data sensor ke telegram -Ultrasonik Kiri-");
  }

  delay(1000);  // Menunda selama 1 detik
}
