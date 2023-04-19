#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

const char* ssid = "Karl Wifi"; // Enter your WIFI SSID
const char* password = "Dayagkarl"; // Enter your WIFI Password

#define BOTtoken "6087785156:AAG-sijm3PaVzusj56fhMkNnP2nmOvYbrFY" // Enter the bottoken you got from botfather
#define CHAT_ID "1556477585" // Enter your chatID you got from chatid bot

#define MQ6_ANALOG_PIN A0
#define MQ6_DIGITAL_PIN D2


// Define the gas threshold value (adjust as needed)
#define GAS_THRESHOLD 200

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

#define BLYNK_TEMPLATE_ID "TMPL6w_CJ3moN"
#define BLYNK_TEMPLATE_NAME "Gas Monitoring"
#define BLYNK_AUTH_TOKEN "9v1VbxO4X0jArQOL3b-lz46lwjrRugDc"
char auth[] = BLYNK_AUTH_TOKEN;

Servo servo;

void setup()
{
  Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");      
  client.setTrustAnchors(&cert);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int a = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    a++;
  }

  // Set up the MQ6 sensor pins
  pinMode(MQ6_ANALOG_PIN, INPUT);
  pinMode(MQ6_DIGITAL_PIN, INPUT_PULLUP);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  bot.sendMessage(CHAT_ID, "Wifi Connected!", "");
  bot.sendMessage(CHAT_ID, "System has Started!!", "");

  // Connect to Blynk server
  Blynk.begin(auth, ssid, password);

  // Set up the servo
  servo.attach(D1);
  servo.write(0);
  delay(2000);
}

void loop() {
  // Read the analog value from the MQ6 sensor
  int gasValue = analogRead(MQ6_ANALOG_PIN);

  // Check the digital value from the MQ6 sensor
  int gasPresence = digitalRead(MQ6_DIGITAL_PIN);

  // Print the gas value and presence status to the serial monitor
  Serial.print("Gas value: ");
  Serial.print(gasValue);
  Serial.print(", Gas presence: ");
  Serial.println(gasPresence);

  // Send the gas value to the Blynk value display widget
  // If the gas value exceeds the threshold, send a push notification and trigger the servo
if (gasValue > GAS_THRESHOLD) {
  bot.sendMessage(CHAT_ID, "Warning Gas Detected!!", "");
  if (servo.read() != 180) { // Check if servo is not already at 90 degree position
    servo.write(180);
    delay(1000);
  }
}
  Blynk.virtualWrite(V0, gasValue); // Update the Blynk value display widget

  // Wait for a moment before checking again
  delay(1000);
}