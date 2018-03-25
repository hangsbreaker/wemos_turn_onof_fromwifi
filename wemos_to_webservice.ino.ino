#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "___Your WiFi SSID___";
const char* password = "___Your Password WiFi___";
int value = HIGH;

WiFiServer server(80);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
}

void loop() {
  // put your main code here, to run repeatedly:

  // Check if a client has connected
  WiFiClient client = server.available();
  if(!client){
      HTTPClient http;

      // ganti dengan URL API Last Feed punyamu sendiri
      http.begin("__ YOUR WEB SERVER ___","___ IF HTTPS ADD WEBSITE CERTIFICATE FINGER PRINT ___ ");
      //http.addHeader("Content-Type","plain/text");
      //http.addHeader("Content-Type","application/json");
      // mulai koneksi dan ambil HTTP Header
      int httpCode = http.GET();

      // httpCode akan bernilai negatif bila error
      if(httpCode > 0){
          // cetak httpCode ke Serial
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);

          // bila nilai dari server diterima
          if(httpCode == HTTP_CODE_OK){
              // cetak string json dari server
              String json = http.getString();
              Serial.println(json);

              // cek value json
              if(json == "{\"code\":200,\"value\":\"1\"}"){
                  // set ke LOW untuk menyalakan
                  value = LOW;
              } else {
                  // set ke HIGH untuk mematikan
                  value = HIGH;
              }
          }
      }else{
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      // tutup koneksi HTTP
      http.end();

      digitalWrite(LED_BUILTIN, value);
      delay(5000);
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request

  if(request.indexOf("/LED=ON") != -1){
    value = LOW;
  }
  if(request.indexOf("/LED=OFF") != -1){
    value = HIGH;
  }



  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.print("Led pin is now: ");

  if(value == HIGH) {
    client.print("Off");
  }else{
    client.print("On");
  }
  digitalWrite(LED_BUILTIN, value);

  client.println("<br><br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED ON<br>");
  client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED OFF<br>");
  client.println("</html>");

  delay(5);
  Serial.println("Client disconnected");
  Serial.println("");
}
