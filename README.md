# 💧 ESP32 Monitoring pH, TDS, dan Turbidity (Blynk + ADS1115)

Sistem monitoring kualitas air menggunakan ESP32 yang membaca sensor **pH**, **TDS**, dan **Turbidity**, lalu mengirim data ke aplikasi **Blynk** secara real-time. Sistem juga melakukan **pengambilan keputusan otomatis** untuk melakukan **Blowdown**, **Injeksi Chemical**, dan **Injeksi AA 602 C** berdasarkan ambang batas yang telah ditentukan.

---

## 🚀 Fitur

- Pembacaan pH menggunakan sensor pH dengan modul **ADS1115 (I2C 16-bit ADC)**
- Pembacaan TDS dan Turbidity menggunakan input analog langsung dari ESP32
- Pengiriman data real-time ke **Blynk Cloud**
- Notifikasi otomatis via **Blynk Events**
- Logika otomatis:
  - Blowdown saat pH > 7 dan TDS > 300
  - Injeksi Chemical saat pH < 7 dan TDS < 150
  - Injeksi AA 602 C saat Turbidity < 15 NTU

---

## 🛠️ Perangkat Keras

- ESP32 Dev Board  
- Sensor pH (dengan output analog)  
- ADS1115 (16-bit ADC I2C)  
- Sensor TDS (pin analog)  
- Sensor Turbidity (pin analog)  
- WiFi dengan koneksi internet

---

## 📲 Aplikasi & Platform

- **Blynk IoT Platform**  
  - Template ID, Template Name, dan Auth Token disesuaikan dengan akun kamu
  - Menggunakan event log: `blowdown_alert`, `chemical_alert`, dan `turbidity_alert`

---

## 🧪 Kalibrasi dan Konversi

```cpp
// Konversi ADC to Voltage untuk ADS1115
voltage = adc0 * 0.1875 / 1000.0;

// Konversi tegangan ke pH (linear)
float convertToPH(float voltage) {
  float m = -8.57;
  float c = 32.28;
  return m * voltage + c;
}
