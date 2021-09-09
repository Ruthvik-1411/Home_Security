# Home Intrusion Alert System
The aim of this project is to design an intruder alert system that notifies the user in case of a break in via mail. The concept can be applied to a get a phone call or a message as most of the people respond to them quickly. If the alarm is armed and if the door is opened then a buzzer starts making noise and the user gets a mail saying that there is a break in and the alarm is triggered.<br><br>
<b>Components Used :</b>
<li> ESP8266 NodeMCU Module
<li> IR Sensor
<li> Push button(4-pin/2-pin)
<li> Buzzer Module
<li> Led  (x1)
<li> Jumper Wires(not required if the setup should be compact)
<li> Power Supply
<li> Stable Internet Connection(+browser)<br><br>
<b> Setup :</b><br><br>
<img src="https://github.com/Ruthvik-1411/Home_Security/blob/main/IFTTT_Based_Home_Security_Monitor/ibhsm_ckt01.jpg" height="250" width="300" />
  <img src="https://github.com/Ruthvik-1411/Home_Security/blob/main/IFTTT_Based_Home_Security_Monitor/ibhsm_ckt02.jpg" height="250" width="300" /><br><br>
<b> Working :</b><br>
<img src="https://github.com/Ruthvik-1411/Home_Security/blob/main/IFTTT_Based_Home_Security_Monitor/ibhsm_d1.jpg" height="400" width="450" /><br><br>
The code is documented well, atleast I hope I did. It should explain most of the part. Below I just gave a brief description of what's happening.
  If the alarm goes on i.e when the alarm is armed and door is opened buzzer starts making noise. The status of house is updated and is sent to the thingspeak channel. After the data is entered into the corresponding field the value associated with it changes. Thingspeak provides two interesting services that can be found in the apps section.
  One is the Thinghttp and other is React. Thinghttp can be given a http request such that whenever something happens in the channel which we want to react on, it triggers this thinghttp request from thingspeak.<br><br>
  <img src="https://github.com/Ruthvik-1411/Home_Security/blob/main/IFTTT_Based_Home_Security_Monitor/ibhsm_ts01.jpg" height="500" width="550" /><br><br>
This is where <b>IFTTT</b>(IF This Then That) service comes in. The http request that is mentioned previously is of the ifttt. In IFTTT we can create applets that allows one to get notified or automate things when something happens or is happening. In this case my applet does <b>IF</b> a webhooks request is obtained(<b>This</b>) <b>Then</b> it triggers an event that was created previously, here it is sending a mail (which can be modified to make a call,sms or even enter the data to a sheet)(<b>That</b>).<br><img src="https://github.com/Ruthvik-1411/Home_Security/blob/main/IFTTT_Based_Home_Security_Monitor/ibhsm_ss01.jpg" height="600" width="450" /><br><br>
  <i>Updating alarm status using webpage from anywhere</i>: The html document above contains the codes to the website that can be used to change the alarm status and get the home status from anywhere in tha world. I keep saying world because even though the webpage is local host they transmit using APIs so it really doesn't matter if it is hosted or not.
  By entering the Channel Number, Read API Key and Write API Key of the thingspeak channel we are interested in, we can send the data to the channel directly. We can also read data from the channels and even write to fields individually on thingspeak.<br><br> There are two apis that I used, one to write data to the field which can be seen in the tx functions and the other is to read the last entered data from the field which can be seen in the rx function. These urls can be modified to the users convinence to get last data or complete data or anything similar. These apis are called using http get as seen in the code and incoming data is also parsed to the need. In the implementation video I added the api keys in the code so there was no need to enter them in the form on the website. The website looks like this:<br>
  <img src="https://github.com/Ruthvik-1411/Home_Security/blob/main/IFTTT_Based_Home_Security_Monitor/ibhsm_wp01.jpg" /><br>
  <img src="https://github.com/Ruthvik-1411/Home_Security/blob/main/IFTTT_Based_Home_Security_Monitor/ibhsm_wp02.jpg" height="800" width="450" /><br><br>
  <i> A free Thingspeak account has its limitations. The data can be entered only once every 15 seconds which makes it difficult to get real time data. There is some latency because of this. Also as it is of no use to send data multiple times in a short interval, the code in nodemcu makes sure that data is sent only once every 20 seconds. To have a reliable data this is a trade off. Even the data from webpage http requests should follow this and this can be seen in the implementation. The status remains same even though it changed in real time.</i><br> MQTT protocol has many broker services which are free and have little to no latency. A similar <a href="https://github.com/Ruthvik-1411/Home_Security/tree/main/Main_Door_Secuity_using%20MQTT">project</a> is done using mqtt protocol and the implementation video shows the quick response. That doesn't mean one is better, each has it's own advantage.
The implementation is a bit longer as it includes the latency in data entry which results in no action sometimes. So those parts were not edited to show that latency.<br>
The Link to the Video is here: <a href="https://drive.google.com/file/d/1gI2gAC7fOs1YzBK0jVthLUzF5k5Sv0sz/view?usp=sharing">Video Link</a>
