#define BLYNK_TEMPLATE_ID "TMPL6PVX_lBKd"
#define BLYNK_TEMPLATE_NAME "Monitoring PH and TDS"
#define BLYNK_AUTH_TOKEN "hIQl1JDIytrolHgjKTwByS2O6gG-4sk0"
#define BLYNK_PRINT Serial  

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>



float ph, voltage;


int tds, adctds;  // TDS in ppm
int adcTurb, turbidity;  // Turbidity in NTU

BlynkTimer timer;
char ssid[] = "Mrais";
char pass[] = "25121999";

// Inisialisasi ADS1115
Adafruit_ADS1115 ads;

// Fungsi untuk mengonversi tegangan menjadi pH
float convertToPH(float voltage) {
  float m = -8.57;
  float c = 32.28;
  return m * voltage + c;
}

void sendSensor() {
  Blynk.virtualWrite(V0, ph);          // pH (unitless)
  Blynk.virtualWrite(V1, tds);         // TDS in ppm
  Blynk.virtualWrite(V2, turbidity);   // Turbidity in NTU
}

void setup(void) {  
  Serial.begin(115200);

  Serial.println("Menghubungkan ke WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung ke WiFi!");

  Serial.println("Menghubungkan ke Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Terhubung ke Blynk!");

  timer.setInterval(3000L, sendSensor);

  // Inisialisasi ADS1115
  ads.begin(); // default address 0x48
  ads.setGain(GAIN_TWOTHIRDS); // ±6.144V range (0.1875 mV/bit)
  Serial.println("Memeriksa sensor...");
}

void loop(void) {
  // Membaca nilai dari sensor pH (dengan ADS1115)
  int16_t adc0 = ads.readADC_SingleEnded(0);
  voltage = adc0 * 0.1875 / 1000.0; // Konversi dari ADC ke Volt
  ph = convertToPH(voltage);       // Panggil fungsi konversi ke pH
  
  // Membaca sensor TDS
  adctds = analogRead(35);              
  tds = constrain(map(adctds, 4095, 50, 0, 1500), 0, 1500);  // TDS in ppm

  // Membaca sensor turbidity
  adcTurb = analogRead(34);
  turbidity = map(adcTurb, 0, 4095, 3000, 0);  // Turbidity in NTU

  // Kirim data ke Blynk
  Blynk.virtualWrite(V0, ph);          // pH (unitless)
  Blynk.virtualWrite(V1, tds);         // TDS in ppm
  Blynk.virtualWrite(V2, turbidity);   // Turbidity in NTU

  // Menampilkan data ke Serial Monitor
  Serial.print("pH: "); Serial.print(ph); Serial.println(" pH");
  Serial.print("TDS: "); Serial.print(tds); Serial.println(" ppm");
  Serial.print("Turbidity: "); Serial.print(turbidity); Serial.println(" NTU");

  // 1. Blowdown jika pH > 7 & TDS > 300
  if ((ph > 7 && tds > 300)) {
    Serial.println("🔵 Blowdown dimulai: Valve terbuka 50% selama 10 menit");
    Blynk.logEvent("blowdown_alert", 
      String("Blowdown diperlukan! ") + 
      "pH: " + ph + " | " + 
      "TDS: " + tds + " ppm");
    
    // Tambahkan delay simulasi blowdown 10 menit (opsional)
    delay(500); // Hanya untuk testing, di real implementation gunakan non-blocking timer
    Serial.println("🟢 Blowdown selesai: Valve ditutup kembali");
  }

  // 2. Injeksi Chemical jika pH < 7 & TDS < 150
  if (ph < 7 && tds < 150) {
    Serial.println("🟠 Injeksi Chemical: AA 6033 C = 3kg, AA 602 C = 5kg");
    Blynk.logEvent("chemical_alert",
      String("Injeksi Chemical diperlukan! ") +
      "pH: " + ph + " | " +
      "TDS: " + tds + " ppm");
  }

  // 3. Injeksi AA 602 C jika Turbidity < 15 NTU
  if (turbidity < 15) {
    Serial.println("🔴 Injeksi AA 602 C sebanyak 3kg");
    Blynk.logEvent("turbidity_alert",
      String("Injeksi AA 602 C diperlukan! ") +
      "Turbidity: " + turbidity + " NTU");
  }

  // Menjalankan Blynk dan Timer
  Blynk.run();
  timer.run();
  delay(2000);
}
