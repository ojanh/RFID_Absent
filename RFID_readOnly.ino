#include <MFRC522.h>
#include <SPI.h>

/**
 * Proyek Mikon 
 * Sistem absensi dengan RFID dan sensor inframerah
 * PORT: MISO 50; MOSI 51; SCK 52; SDA 53
 */

//setup pin reset dan SDA
#define RST_PIN         9           
#define SS_PIN          53

//inisialisasi objek RFID
MFRC522 rfid522(SS_PIN,RST_PIN);
MFRC522::StatusCode status;

            
char dataRead[18]; //buffer reader RFID
byte blockAddr = 4; //selector block data (1 block berisi 16 Byte dengan 2 CRC)
byte sizeData = sizeof(dataRead); //SizeData (untuk write saja)
boolean isGetted = true;   //variable untuk cek Data
boolean isAuthented = true; //variable 
String RFIDdata;

//setup SSID dan Password 
String SSID_esp = "Vianindy"; //SSID nya
String password="vianciyan"; //Passwordnya

//inisialisasi objek key
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600); //Serial Debug ke PC
  Serial1.begin(9600);  //Komunikasi Ke Nuvoton
  Serial2.begin(9600); //Komunikasi ke ESP8266
  SPI.begin(); //Inisialisasi SPI ke RFID
  rfid522.PCD_Init(); 

// Setup Key nya untuk ototentifikasi Data
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

}

//Main Program
void loop() {
    //cek kartu
    if(!rfid522.PICC_IsNewCardPresent()) 
      return;
      
    //cek kartu
    if(!rfid522.PICC_ReadCardSerial())
      return;
    
    //ototentifikasi key
    isAuthented = authData();
    if (!isAuthented) return; 
    
    readData();

  // STOP RFID
    rfid522.PICC_HaltA();
   //Stop Enkripsi di PCD (berhubungan dengan authentifikasi key dengan UID nya)
    rfid522.PCD_StopCrypto1();
    
}

boolean authData(){
    //TrailerBlock untuk ototentifikasi Data
    byte trailerBlock = 7;

    
    // ototentifikasi UID dan key A
    status = (MFRC522::StatusCode) rfid522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid522.uid));
    if (status != MFRC522::STATUS_OK) {
        //Serial.print(F("PCD_Authenticate() failed: "));
        //Serial.println(rfid522.GetStatusCodeName(status));
        return false;
    }

    //otototentifikasi UID dan key B
    status = (MFRC522::StatusCode) rfid522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(rfid522.uid));
    if (status != MFRC522::STATUS_OK) {
        //Serial.print(F("PCD_Authenticate() failed: "));
        //Serial.println(rfid522.GetStatusCodeName(status));
        return false;
    }

    return true;
}
    
void readData() {
  status = (MFRC522::StatusCode) rfid522.MIFARE_Read(blockAddr, dataRead, &sizeData); //baca RFID dan get status
   if (status != MFRC522::STATUS_OK) { //jika gagal dibaca
        Serial.print(F("MIFARE_Read() gagal: "));
        Serial.println(rfid522.GetStatusCodeName(status));
        return;
   }

   //kirim data ke nuvoton dan build RFID string data
   for (byte i = 6; i<16; i++){
    Serial.print((int)dataRead[i]); //debugging ke PC
    Serial1.print("yes"); //kirim flag atau data ke nuvoton untuk baca sensor nya
    RFIDdata+=(int)dataRead[i]; //casting ke integer supaya dibuat string nya
   }
    Serial.print(RFIDdata); //Debug RFID dalam bentuk string (bukan byte)
   

     
}
//fungsi intterupt dari serial1 arduino saat menerima data dari nuvoton
void serialEvent1(){
  while(Serial1.available()){
      String data = Serial1.readStringUntil('$');
      Serial.println(data); //debugging 
  }
  Serial.println("go to ESP"); //debugging 
  ESPConnect();
  
}

void ESPConnect(){
  
  String cmd = "AT+CWJAP=\"" + SSID_esp +"\",\"" + password + "\""; //komunikasi ke WiFi

  Serial2.println(cmd);
  delay(4000); //menunggu input dari ESP8266
  if(Serial2.available()){
    Serial.println("ada input dari ESP");
  }

  //cek apa ESP8266 berhasil konek ke access point
  if (Serial2.find("OK")){ 
    Serial.println("berhasil!");
    //ESPsendPost();    
  }
  else{
    Serial.println("tidak bisa konek ke Wi-Fi");
  }
}

//fungsi untuk kirim GET ke server.
void ESPsendPost(){
  
}


