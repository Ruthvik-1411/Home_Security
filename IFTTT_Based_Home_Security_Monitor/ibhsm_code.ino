#include <ESP8266WiFi.h>                                      //Libraries to use wifi functions
#include <WiFiClient.h>
#include <ThingSpeak.h>                                      //Libraries to use ThingSpeak IoT Platform

int buzpin = D8;                                            //Declaring buzzer, push button and ir sensor pins.
int irpin = D5;
int pbtn= D7;
int irval=0;                                              //Declaring variable to use later                                               
bool stat=0;
bool btn=0;
int num=0,state=1;
int alarm=0,garb=0;

unsigned long int tempt=0;                                //Variable to store time(ms) since the process began.

String astat="Disarm";                                    //Initial status of alarm is disarmed. astat and alarm variables essentially do the same thing. The reason they are used is to have two seperate varibale for decision making and transmitting data.
int home_stat=200;                                        //Status of Home is secure initially which means the value is 200 like in http requests. If intruder it changes to 404.

WiFiClient client;                                        //Creating an instance of client

const char* ssid="xxxxxxx";                               //Wifi ssid(name) and Password
const char* password="xxxxxxx";
unsigned long myChannelNumber=xxxxxxx;                    //ThingSpeak Channel number, Write API Key, Read API Key
const char* myWriteAPIKey="xxxxxxxxxxxxxxxx";
const char* myReadAPIKey="xxxxxxxxxxxxxxxx";

//Replace xxxx.... with the respective details.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);                                  //Begin Serial communication
  delay(100);
  pinMode(buzpin,OUTPUT);                               //Declaring that buzzer pin is output and push button, ir sensor output pin are inputs.
  pinMode(irpin,INPUT);
  pinMode(pbtn,INPUT);
  delay(1000);
  //Serial.println();                                   //To be used while calibrating to now the status of wifi connection
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  WiFi.begin(ssid,password);                            //Try to connect to wifi with this ssid,password
  while(WiFi.status()!=WL_CONNECTED){                   //If wifi is not connected keep waiting for 0.2s and check again until it is connected
    //Serial.print(".");
    delay(200);
  }
  //Serial.println("\n WiFi Connected!!");
  //Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);                             //establish connection to ThingSpeak with this device as client
}

void loop() {
  // put your main code here, to run repeatedly:
  irval=digitalRead(irpin);                             //Read the value from IR sensor to see if the door is open or closed
  btn=digitalRead(pbtn);                                //Read the value from push button pin to see if the button is pressed
  //Serial.println(irval);
  delay(500);
  if(btn){                                              //If the button was pressed
    digitalWrite(buzpin,HIGH);                          //Make the buzzer on for a short period and off it
    delay(200);
    digitalWrite(buzpin,LOW);
    tempt=millis();                                     //Get the time since the program started executing
    while(millis()<=(tempt+5000)){                      //While the time is less than 5 seconds since it was last measured 
      if(digitalRead(pbtn)){                            //Check if the button is pressed and check this every 1 second
        num+=1;                                         //Get the time for which the button was pressed
      }
      delay(1000);
    }
    //Serial.println(num);  
    if(num>=4){                                       //If the button was pressed for more than 4 seconds then it means that we want to arm the alarm so increase the buzzer noise slowly saying that it is armed now
      for(int i=0;i<255;i+=64){
        analogWrite(buzpin,i);
        delay(200);
      }
      astat="Arm";                                    //Change the status to arm and also change the variables to send this status to thingspeak channel
      alarm=1;garb=1;
      digitalWrite(buzpin,LOW);
    }
    else{                                           //If the button was pressed for less than 4 seconds then it means that we want to disarm the alarm, so decrease the buzzer noise slowly saying that it is disarmed now
      for(int i=255;i>=0;i-=64){
        analogWrite(buzpin,i);
        delay(200);
      }
      astat="Disarm";                               //Change the status to disarm and also change the variables to send this status to thingspeak channel 
      alarm=0;garb=0;
    }
    num=0;              //Reinitialise the variable 
  }
  if(garb==1 || astat=="Arm"){                      //If the button is not pressed but we want change the alarm status from webpage via thingspeak then this is used
    alarm=1;                                        //garb reads the data from field2 from the channel. This field has alarm status that is previously entered. By changing this the alarm status can be changed here
    astat="Arm";
  }
  if(garb==-1 || (garb==0 || astat=="Disarm")) {    //It is similar to above case but opposite. However if there is no entry in the field then it returns -1 so that case is also included.
    alarm=0;
    astat="Disarm";
  }
  if(irval==1){                                     //The usual case to identify a break is this part of the code. If the door is opened, IR sensor gives a high value in my setup.
    if(astat=="Arm"){                               //If the status obtained from button or thingspeak is arm then start the buzzer if the door is opened until the state changes to disarm irrespective of door open/close.
      digitalWrite(buzpin,HIGH);
      home_stat=404;                                //As the alarm is armed but door is opened change the home status to possible intrusion i.e 404 here(http reference :) ).
      delay(500);
    }
    else if(astat=="Disarm"){                       //If the status obtained from button or thingspeak is disarm then keep the buzzer off. 
      digitalWrite(buzpin,LOW);
      home_stat=200;                                //As the alarm is disarmed and door is opened it is ok condition. So change home status to secure i.e 200(http reference).
      delay(500);
    }
    if(stat==0){                                    //stat 0 means that the door was previously in closed state, but when door is opened print door is now opened and change stat to 1 saying that door is now open.
      //Serial.println("Door Now Opened");
      stat=1;
      delay(200);
    }
  }
  else{                                           //If the door is closed i.e ir sensor gives a low value in my setup. 
    if(astat=="Disarm"){                          //and if the alarm status obtained is disarm then keep the buzzer off and chnage the home status to secure i.e 200 in this case
      digitalWrite(buzpin,LOW);
      home_stat=200;
      delay(500);
    }
    if(stat==1){                                  //If the stat is 1 i.e the door was previously opened but now it is closed then print door is now closed and chnage stat to 0 saying that door is now close
      //Serial.println("Door Now Closed");
      stat=0;
      delay(200);
    }
  }
  //Serial.print(astat);            //To be used while calibrating
  //Serial.print(alarm);
  //Serial.println(home_stat);
  if (state==1) {                                                       //This variable is used to transmit data to thingspeak channel only one field at a time. Next field is transmitted in the next loop.
  ThingSpeak.writeField(myChannelNumber, 1, home_stat, myWriteAPIKey);  //Sending the home status to thingspeak channel using channel no, write api key and field number and data to be sent
  garb=ThingSpeak.readIntField(myChannelNumber, 2, myReadAPIKey);       //While we are at it also get the last entry in the field 2 using channel no and read api key in the form of int or convert to int when reading
  state = state+1;                                                      //Change the state so that in the next loop it won't be 1 and then we can send the data to another field
  //Serial.println(state);        //To be used while calibrating 
  //Serial.println("Channel 1 transmitted");
  }
  else {                                                              //If the state is not 1 then send the alarm status to thingspeak channel using channel no, write api key and field number and data to be sent
  ThingSpeak.writeField(myChannelNumber, 2, alarm, myWriteAPIKey);
  //Serial.println(state);                                             
  //Serial.println("channel 2 transmitted");
  state = 1;                                                         //Change the state to 1 so that in the next iteration field 1 is sent to the channel.
}
  delay(20000);        //Wait for 20s. Free thingspeak account allows the user to write the data once every 15 seconds so if data is sent more times in this interval, the last one is considered. To avoid that, this delay is added.
}
