#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11);   // RX | TX
int relays[] = {2,3};
const int BUTTON_1 = 8;
const int BUTTON_2 = 9;

int command[4];
int pos = 0  ; 
bool shouldProcess = false;
volatile bool RELAY1_STATE = true;
volatile bool RELAY2_STATE = true;
volatile bool FROM_BUTTON1 = false;
volatile bool FROM_BUTTON2 = false;
void sendCommand(const char * command){
  Serial.println(command);
  BTSerial.println(command);
  delay(100);
}

void setup() 
{
  BTSerial.begin(9600);
  sendCommand("AT+NAMEOplusRoom");
  sendCommand("AT+ROLE0");
  sendCommand("AT+UUIDFFE0");
  sendCommand("AT+CHARFFE1");
  pinMode(relays[0],OUTPUT);
  pinMode(relays[1],OUTPUT);
  pinMode(BUTTON_1,INPUT);
  pinMode(BUTTON_2,INPUT);
  digitalWrite(relays[0],HIGH);
  digitalWrite(relays[1],HIGH);
}

void logCommand(const char * str){
  Serial.println(str);
}
void readBLECommand(){
  while(BTSerial.available() && !shouldProcess){
    char dt = BTSerial.read(); 
    int b=  (int) dt;
    if(b == 00 && pos == 0){
        command[pos++] = b;
    }else{
      if(b == -1 && pos == 3){
        command[pos++] = b;
        shouldProcess = true;
        pos = 0 ; 
      }else if ( pos == 1 || pos == 2 ){
        command[pos++] = b;
      }
    }
  }
}
void processCommand(){
  Serial.print("Relay : ");
  Serial.print(command[1]);
  Serial.print(" Mode : " ); 
  Serial.println(command[2]);
  
  int relayNumber = command[1];
  int relayMode = command[2]; 

  if (relayNumber == 0 || relayNumber == 1){
    if(relayMode == 0|| relayMode == 1 ){
      if (relayMode == 1){
         digitalWrite(relays[relayNumber],HIGH);
      }else if (relayMode == 0){
        digitalWrite(relays[relayNumber],LOW);
      }
      digitalWrite(13,HIGH);
      delay(500);
      digitalWrite(13,LOW);
      delay(500);
      digitalWrite(13,HIGH);
    }
  }
  shouldProcess = false;
  pos = 0 ;
}
void loop()
{
  int r1 = digitalRead(BUTTON_1);
  int r2 = digitalRead(BUTTON_2);
  if(r1 == HIGH){
    RELAY1_STATE = !RELAY1_STATE;
    FROM_BUTTON1 = true;
  }
  if(r2 == HIGH){
    RELAY2_STATE = !RELAY2_STATE;
    FROM_BUTTON2 = true;
  }
  if(FROM_BUTTON1){
    FROM_BUTTON1 = false;
    Serial.println("SWAPPING RELAY 1");
    if(RELAY1_STATE){
      digitalWrite(relays[0],HIGH);
    }else{
      digitalWrite(relays[0],LOW);
    }
    delay(1000);
  }
  if(FROM_BUTTON2){
    FROM_BUTTON2 = false;
    Serial.println("SWAPPING RELAY 2");
    if(RELAY2_STATE){
      digitalWrite(relays[1],HIGH);
    }else{
      digitalWrite(relays[1],LOW);
    }
    delay(1000);
  }
  
  readBLECommand(); 
  if(shouldProcess)
    processCommand();
}
