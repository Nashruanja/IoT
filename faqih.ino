#include <Servo.h>  //library Servo
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "CTBot.h"
CTBot myBot;

//Telegram dan Wifi
char ssid[] = "DRAGON";                                            //nama wifi
char password[] = "qwerty1234";                                    //password wifi
String token = "6010383923:AAFccCOnS2aAImT4pphBsltxL26zZqEiXQw" ; //token bot telegram
const int id = 1234219656 ;                                     //idbot

WiFiClientSecure client;

//motor servo
Servo myservo;
int sudut = 75;  //berputar sampai 75 drajat
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
  //motor servo
  myservo.attach(0);  //Hubungan ke pin D3
  //serial print
  Serial.begin(9600);
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
    myservo.write(sudut);  //servo berputar 75 drajat
    delay(15 * 60000);     // Delay selama 15 menit (15 x 60000 milisecond)
  }
  else {
    myservo.write(normal);  //servo Kembali
  }
  //Notifikasi Sumbatan Kanan
  if (jarak_kanan_meter > 1) { // jika ultrasonik kanan mendeteksi sumbatan lebih dari 1 M
    String kanan = "TERDAPAT SUMBATAN PADA SISI KANAN!\n";
    kanan += "Jarak Sumbatan Pada Sisi Kanan : ";
    kanan += int(jarak_kanan_meter);
    kanan += " M\n";
    myBot.sendMessage(id, kanan, "");
    Serial.println("Mengirim data sensor ke telegram");
  }
  //Notifikasi Sumbatan Kiri
  else if (jarak_kiri_meter > 1 && myservo.read() != sudut) { // jika ultrasonik Kiri mendeteksi sumbatan lebih dari 1 M dan servo tidak disudut
    String kiri = "TERDAPAT SUMBATAN PADA SISI KIRI!\n";
    kiri += "Jarak Sumbatan Pada Sisi Kiri : ";
    kiri += int(jarak_kiri_meter);
    kiri += " M\n";
    myBot.sendMessage(id, kiri, "");
    Serial.println("Mengirim data sensor ke telegram");
  } 

  delay(1000);  // Menunda selama 1 detik
}
