# Home Security Status Monitoring
Security systems are pretty common these days and are necessary. Every one of us wants our homes, devices and data to be secure. Home security is one of those issues we look forward to.<br>
The idea of this project is to monitor the status of the main door or any door of interest if it is open or closed and thereby understanding the status of the home if it is secure or if there was any possible intruder and monitor it using mqqt dash app. If we have an intruder we get an alarm or an indication of sorts. We also want to Arm and Disarm the alarm as we don't always want to keep the door armed. When the user is home there is no need to have an alarm.<br><br>
<b>Components Used :</b><br>
<li>ESP8266 Nodemcu Module
<li>IR Sensor
<li>LED/Buzzer Module
<li>Jumper Wires
<li>Power Supply
<br><br>
  <img src="https://github.com/Ruthvik-1411/Home_Security/blob/main/Main_Door_Secuity_using%20MQTT/mdsum%20circuit%2001.jpg" width="500" height="400" /><br><br>
  <b>Working</b><br>
  The working is pretty much simple. The IR sensor is attached to the door and if you consider the setup as I have made, in that case the ir sensor detects an obstacle when
  the door is closed as there is wall infront of it. If the door is opened there won't be any obstacle and we can know that it is open. So if the sensor is high then the door is open, else it is closed. That's the basic part.<br><br>
  Now we have two cases here closed/open door and alarm. We want to know if the door is opened or closed and also an intrusion alert which can be achieved by using a buzzer module.
  So we tell the controller if the door should be armed or disarmed also.<br><br>
  If the door is armed and it is opened then we want an alert. If the door is closed after opening the door while it is armed then we want an alert. If the door is disarmed and we want to know if it is open or closed. We also want to lock the door and then arm it. All these can be achieved in this project.<br><br>
  First the mcu reads if the alarm is armed or disarmed and then reads if the door is opened or closed. Based on this a decision is made to start an alarm or remain calm. We send and recieve the data from the MQTT server. We use the mqtt broker dashboard to act as a channel to use the data. It is secure and easy to use. The Publish and Subscribe facilities 
  allow us to send and recieve data from multiple channels. The code explains how the mcu reads the data and uses it to make the decision.<br><br>
  The status of the door is sent to the mqtt server and this data can be seen in the mqtt dash app. You can see how it works in the implementation video below.
  <br>Implementaion Video : <a href="https://drive.google.com/file/d/1rXKODvqkw3WIfDLNOFRC8cANSENAxm1l/view?usp=sharing">Video Link</a><br>
  The code and mqtt dash app's screenshots can be found above
