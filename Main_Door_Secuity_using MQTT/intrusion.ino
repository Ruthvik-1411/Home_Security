#include <ESP8266WiFi.h>                                //To connect to WiFi
#include <PubSubClient.h>                               //To establish communication with mqtt broker

int ledpin=D8;                                         //Declaring info device pin that acts as an alarm(generally a buzzer is used, here I used an LED instead) 
int irpin=D5;                                          //Declaring IR sensor output pin
int irval=0;                                          //Variable to store data from ir sensor
int stat=0;                                           //Variable that helps in understanding if the door was already open or closed(0 means it was closed) 
char* door_stat="";                                   //To store status of door to be published at the mqtt server side
char* home_stat="Secure";                             //To store status of house, Secure or possible intrusion and to publish on server side

String astat="Arm";                                   //Alarm status whether the alarm is armed or disarmed
const char* ssid="xxxxxxx";                           //To store wifi name(ssid)
const char* password="xxxxxx";            //To store wifi password
const char* mqtt_server="broker.mqtt-dashboard.com";  //To store the mqtt server address broker.mqtt-dashboard.com is a free one
long lastMsg=0;                                       //To store the time last message was recieved from the mqtt server
char detail[50],h_stat[50];                           //Character arrays to store data to be sent to the server
int value=0;

WiFiClient espClient;                                 //Declaring an instance of WiFi client as espclient and using this to connect to mqtt server using pubsubclient instance client
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);                              //Begin serial communication, while callibrating
  pinMode(ledpin,OUTPUT);                            //Declaring the mode of alarm module(buzzer or led) and ir pin
  pinMode(irpin,INPUT);
  delay(1000);
  //Serial.print("Connecting to ");                 //To be used while callibrating
  //Serial.println(ssid);                           //To be used while callibrating
  WiFi.begin(ssid, password);                       //To connect to the wifi with name "ssid" and "password"
  while (WiFi.status() != WL_CONNECTED) {         //Until connected to wifi keep printing this message and wait for 250ms each time
    delay(250);
    //Serial.print(".");                          //To be used while callibrating
  }
  //Serial.println("WiFi connected");             //To be used while callibrating
  client.setServer(mqtt_server, 1883);            //to connect to the mqtt server, when the protocol is mqtt/tcp (mostly used) the port number will be 1883 most of the times
  client.setCallback(callback);                   //A function that is declared to be executed by nodemcu everytime it this happens 
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {                         //If client is not connected then try reconnecting using that function
    reconnect();
  }
  client.loop();                                    //Repeat the operations as we want to client to constantly update the data on the server at the earliest
  long now = millis();                              //Get currently elapsed time
  if (now - lastMsg > 5000) {                       //Print the message every 5 seconds
    lastMsg = now;
    ++value;
    getirval();                                     //get the status of ir sensor(door status) 
  }
    if(irval==1){                                   //If there is no obstacle infront of the sensor i.e opened in my setup then,
      if(astat=="Arm"){                            //If the alarm is armed i.e there was a break in
        digitalWrite(ledpin,HIGH);                  //Start the alarm i.e on the led in this case, can be replaced with a buzzer to make unpleasant sound when broke in
        home_stat="Possible Intrusion";             //Change the status of house to Possible Intrusion
        delay(500);
      }
      else if(astat=="Disarm"){                   //But if the alarm is disarmed i.e alarm is off and the door was opened
        digitalWrite(ledpin,LOW);                 //Do nothing or keep the led low or keep buzzer off
        home_stat="Secure";                       //Keep the status of house to Secure as we disarmed the alarm, we are not expecting any intrusion
        delay(500);
      }
      if(stat==0){                                //If stat is 0 i.e door was previously closed, that is it was opened right now from that state
        //Serial.println("Door Now Opened");      //To be used while calibrating
        stat=1;                                   //Change the stat to 1 i.e that door is now open, this helps in knowing if the door was already open later in time
        delay(200);
      }
    }
    else{                                         //If there is an obstacle infront of the sensor, i.e closed in my setup then,
      if(astat=="Disarm"){                        //If the alarm is disarmed i.e alarm is off and the door is closed
        digitalWrite(ledpin,LOW);                //Do nothing or keep the led low or keep buzzer off
        home_stat="Secure";                      //Keep the status of house to Secure as we disarmed the alarm, we are not expecting any intrusion
        delay(500);
      }
      if(stat==1){                              //If stat is 1 i.e door was previously open, that is it was closed right now from that state
        //Serial.println("Door Now Closed");    //To be used while calibrating
        stat=0;                                 //Change stat to 0 i.e that door is now closed, this helps in knowing if the door was already closed later in time 
        delay(200);
      }
    }
}

//Function used by the client to perform actions on the mqtt server
void callback(char* topic, byte* message_i_o, unsigned int length) {  //The topic name, message, length of message is recieved 
  Serial.print("Message arrived [");                                  //To be used while callibrating
  Serial.print(topic);                        //The topic name is printed
  Serial.print("] ");
  for (int i = 0; i < length; i++) {          //The message is printed
    Serial.print((char)message_i_o[i]);
  }
  Serial.println();
  if ((char)message_i_o[0] == 'D') {        //To act on the data, the first character is compared as we only send two messages here, Arm and Disarm
    astat="Disarm";
  }
  else if ((char)message_i_o[0] == 'A') { 
    astat="Arm";
  }
  else {                                  //Just in case we sent something other than these, we want the led to blink saying that there was something wrong
    for(int i=0;i<10;i++){
      digitalWrite(ledpin,HIGH);
      delay(500);
      digitalWrite(ledpin,LOW);
      delay(500);
    }
  }
}
//Function to get the status of door and publish the status in the subscription channels
void getirval(){
  irval=digitalRead(irpin);
  if(irval==1){                 //If the door was opened then make door_stat as opened else make it closed
    door_stat="Opened";
  }
  else{
    door_stat="Closed";
  }
  snprintf(detail,50,door_stat);    //snprintf writes to buffer but we can use it like this also to transfer data from door_stat to detail...
  snprintf(h_stat,50,home_stat);
  if(isnan(irval)){                 //If the ir sensor value is NaN that means there was an error so publish the corresponding message this is useful if we are using an accelerometer or some similar movement sensors
    Serial.println("Failed to read from the sensor!");
    client.publish("DOOR_STATUS","Failed to read from the sensor!");
  }
  else{                             //If the sensor has a good value then publish the following status in the respective channels
    client.publish("DOOR_STATUS",detail);
    client.publish("HOME_STATUS",h_stat);
    Serial.print(detail);Serial.print(",");Serial.println(h_stat);    //To be used while callibrating
  }
  return;
}
//Function to reconnect to the mqtt 
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection....");
    if (client.connect("xxxxxxxxxx")) { //Any random name like esp8266client or a client id
      Serial.println("Successfully Connected");
      client.subscribe("SECURITY_STATUS");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 2 seconds");
      delay(2000);
    }
  }
}
