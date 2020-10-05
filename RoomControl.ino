#include <ESP8266WiFi.h>

const char* ssid     = "Oplus - Micro Devices";
const char* password = "mr.oplus@yahoo.fr";
WiFiServer server(80);
String header;

void setup() {
  Serial.begin(9600); 
  Serial.println("Starting ..."); 
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  digitalWrite(D2 ,HIGH); 
  digitalWrite(D3 ,HIGH);
  Serial.println("Turning OFF all relays"); 
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

}
bool STATE1 = false;
bool STATE2 = false;
void loop() {
  int mode1 = digitalRead(D0); 
  int mode2 = digitalRead(D1); 
  if(mode1 == HIGH){
    STATE1= !STATE1;
    swapState(D2,STATE1);
  }
  if(mode2 == HIGH){
    STATE2= !STATE2;
    swapState(D3,STATE2);
  }
  HandleWifi();
  delay(1000);
}
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;
void HandleWifi(){
  WiFiClient client = server.available();
  if (client) {                            
    Serial.println("New Client.");          
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();         
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);
        header += c;
        if (c == '\n') { 
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            if (header.indexOf("GET /1/on") >= 0) {
              Serial.println("REALY 1 on");
              STATE1 = true;
              digitalWrite(D2, LOW);
            } else if (header.indexOf("GET /1/off") >= 0) {
              Serial.println("RELAY 1 off");
              STATE1 = false;
              digitalWrite(D2, HIGH);
            } else if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("RELAY 2 on");
              STATE2 = true;;
              digitalWrite(D3, LOW);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("RELAY 2 off");
              STATE2 = false;
              digitalWrite(D3, HIGH);
            }
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            client.println("<body><h1>Oplus Room</h1>");
            if (STATE1){
              client.println("<p>RELAY1 - State ON</p>");  
            }else{
              client.println("<p>RELAY1 - State OFF</p>");  
            }
            
            if (STATE1 == false) {
              client.println("<p><a href=\"/1/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/1/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            if (STATE2){
              client.println("<p>RELAY2 - State ON</p>");  
            }else{
              client.println("<p>RELAY2 - State OFF</p>");  
            }

            if (STATE2==false) {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }  
}
void swapState(int pinNumber , bool mod){
  Serial.print("Swapping PinNumber#");
  Serial.print(pinNumber);
  Serial.print(" New Mode : " );
  Serial.print(mod);
  Serial.println();
  if(mod == true){
    digitalWrite(pinNumber,LOW);
  }else{
    digitalWrite(pinNumber,HIGH);
  }
  
}
