#define BLYNK_TEMPLATE_ID "TMPL6usR_d0_l"
#define BLYNK_TEMPLATE_NAME "TuoiCay"
#define BLYNK_AUTH_TOKEN "AwEeKJALeoTWlAWZuIXAW0sYuIJ_3nxx"
#define BLYNK_PRINT Serial

#include <DHT.h>
#include <WiFi.h>  // Dùng ESP32 thì đổi thành <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0X27,16,2); //SCL A5 SDA A4
int num=0;

#define DHTPIN 27       // Chân DHT11 kết nối
#define DHTTYPE DHT11  // Loại cảm biến
#define SOIL_SENSOR_PIN 34  // Chân analog kết nối cảm biến độ ẩm đất HS0163
#define RELAY_PIN 26  // Chân Relay điều khiển bơm nước
// #define RELAY_PIN2 27  // Chân Relay bắn phân
int buzzer = 36;

//Cấu hình NTP 
WiFiUDP ntpUDP; // Tạo đối tượng WiFiUDP để truyền thông UDP 
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000); // Tạo đối tượng NTPClient để lấy thời gian, với múi giờ UTC+7 (Indochina Time)

const char* ssid = "Redmi9";      // Tên Wi-Fi của bạn
const char* password = "longvu42";  // Mật khẩu Wi-Fi

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  
  lcd.init();                    
  lcd.backlight(); 

  lcd.print("Do am: ");
  lcd.setCursor(0,1);
  //lcd.print("Do am: ");
  //lcd.setCursor(1,1);
  //lcd.createChar(1, degree);
 // dht11.begin();  

  pinMode(buzzer, OUTPUT);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Đang kết nối Wi-Fi...");
  }

  Serial.println("Đã kết nối Wi-Fi.");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);  // Khởi tạo Blynk

  // Bắt đầu NTP 
  //timeClient.begin(); // Khởi động NTPClient
}

void loop() {

  Blynk.run();

  float soilMoisture = analogRead(SOIL_SENSOR_PIN) * 100.0 / 4095;  // Chuyển đổi độ ẩm đất từ giá trị analog
  float humidity = dht.readHumidity();  // Độ ẩm không khí

  timeClient.update(); // Cập nhật thời gian từ NTP server
  int currentHour = timeClient.getHours(); // Lấy giờ hiện tại
  int currentMinute = timeClient.getMinutes(); // Lấy phút hiện tại
  
  //Kiểm tra nếu giờ hiện tại nằm ngoài khoảng 18:00 đến 6:00
  bool allowPumpOperation = !(currentHour >= 18 || currentHour < 6);
  

  //Kiểm tra giá trị của độ ẩm đất và không khí và điều kiện thời gian
  if (allowPumpOperation) {
  
    if (humidity < 50) {
      Serial.println("Bật máy bơm");

      Serial.println("Độ ẩm: " + String(humidity));
      lcd.setCursor(10,0);
      lcd.print(round(humidity));
      lcd.print(" %");
      digitalWrite(RELAY_PIN, HIGH);  // Bật bơm
    } else {
      // Ngập
      digitalWrite(RELAY_PIN, LOW);    // Tắt bơm
      Serial.println("Tắt máy bơm");
      Serial.println("Độ ẩm: " + String(humidity));
      lcd.setCursor(10,0);
      lcd.print(round(humidity));
      lcd.print(" %");
    }
  } 
  else {
    digitalWrite(RELAY_PIN, LOW); // Đảm bảo tắt bơm nếu nằm trong khoảng 18:00 đến 6:00
    Serial.println("Ngoài giờ hoạt động bơm (18:00 - 6:00)");
  }
  int WaterSensor=analogRead(25); // Incoming analog  signal read and appointed sensor
  Serial.println("Mực nước hiện tại: " + String(WaterSensor));

  if (WaterSensor < 10){
    analogWrite (buzzer, HIGH);
    delay(1000);
  }
  else {
    analogWrite (buzzer, LOW);
    delay(1000);
  }

  //int h= dht.readHumidity();
   // float h = dht11.readHumidity(); 
   // float t = dht.readTemperature();

  //if (isnan(humidity)) { 

      // Không làm gì 
      // Thông báo lỗi
    //Serial.println("Độ ẩm lỗi");
  //}
  

  
  delay(500);  // Độ trễ nửa giây giữa các lần đọc cảm biến

  // Kiểm tra thời gian hiện tại và bật/tắt bơm phân
  //if ((currentHour == 6 && currentMinute == 0) || (currentHour == 18 && currentMinute == 0)) { 
  //  digitalWrite(RELAY_PIN2, HIGH); 
  //  Serial.println("Turning pump ON"); 
  //  delay(300000); // Tưới nước trong 5 phút (300.000 milliseconds)
  //  digitalWrite(RELAY_PIN2, LOW); 
  //  Serial.println("Turning pump OFF"); 
  //} 
  //delay(60000); // chờ 1 phút
  
}
