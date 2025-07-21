// Intelligent Lighting System – Grace Systems

#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Pin assignments
#define LED1 18  // Auto: Based on LDR
#define LED2 19  // Manual: Dashboard
#define LED3 21  // Manual: Dashboard
#define LDR_PIN 36  // Analog input

WebServer server(80);

bool led2State = false;
bool led3State = false;
int lightThreshold = 1000;  // Tweak depending on lighting conditions

// HTML UI
String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Grace Systems Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial, sans-serif; background: #121212; color: #f0f0f0; text-align: center; }
    h1 { color: #00e6e6; margin-top: 30px; }
    button {
      padding: 15px 25px; margin: 10px; font-size: 20px;
      background: #00bcd4; color: white; border: none; border-radius: 8px;
      transition: background 0.3s ease;
    }
    button:hover { background: #0097a7; }
    .footer { margin-top: 40px; font-size: 14px; color: #aaa; }
  </style>
</head>
<body>
  <h1>Grace Systems – Intelligent Lighting</h1>

  <p><b>LED1</b> is <span style="color:#ffcc00;">auto-controlled</span> by LDR 🌙</p>

  <p>
    <button onclick="toggleLED(2)">Toggle LED2</button>
    <button onclick="toggleLED(3)">Toggle LED3</button>
  </p>

  <div class="footer">
    Powered by <b>Grace Systems</b> | Team Spark Innovations
  </div>

  <script>
    function toggleLED(led) {
      fetch(`/toggle?led=${led}`)
        .then(() => location.reload());
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

  server.on("/", []() {
    server.send(200, "text/html", htmlPage);
  });

  server.on("/toggle", []() {
    int led = server.arg("led").toInt();
    if (led == 2) {
      led2State = !led2State;
      digitalWrite(LED2, led2State);
    } else if (led == 3) {
      led3State = !led3State;
      digitalWrite(LED3, led3State);
    }
    server.send(200, "text/plain", "LED toggled");
  });

  server.begin();
}

void loop() {
  server.handleClient();

  // LDR auto-lighting for LED1
  int lightLevel = analogRead(LDR_PIN);
  Serial.println("LDR reading: " + String(lightLevel));

  if (lightLevel < lightThreshold) {
    digitalWrite(LED1, HIGH); // It’s dark
  } else {
    digitalWrite(LED1, LOW); // It’s bright
  }

  delay(200); // Debounce for auto light switching
}
