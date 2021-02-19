#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

constexpr uint8_t RST_PIN = 5;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 4;     // Configurable, see typical pin layout above

byte i = 0;
String id_tag = "";
String link = "";
 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class 
MFRC522::MIFARE_Key key; 

void setup() { 
  Serial.begin(115200);
    // Serial.setDebugOutput(true);
  
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  delay(1000);

  WiFi.hostname("MyNode");
  WiFi.begin("3A", "Triple3A");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("."); 
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Wi-Fi Connected");
}
 
void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {

      WiFiClient client;
      
      MFRC522::MIFARE_Key key;
      if ( ! mfrc522.PICC_IsNewCardPresent()) {return;} //mendetksi keberadaan kartu
      if ( ! mfrc522.PICC_ReadCardSerial()) //membaca nomor seri kartu
        return;
        for (i = 0; i < mfrc522.uid.size; i++){
            id_tag += mfrc522.uid.uidByte[i];
        }
        
      HTTPClient http;
      link = "http://192.168.1.13:8080/peminjaman_alat/kirimkode.php?kode_rfid=" + id_tag;
      http.begin(client, link);

      int httpCode = http.GET();
      String payload = http.getString();
      Serial.println(payload);
      http.end();
          
      Serial.println(id_tag);
      id_tag="";
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      
      delay(1000);
  }
}
