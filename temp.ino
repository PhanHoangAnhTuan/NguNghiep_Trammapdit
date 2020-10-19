#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>  
#include "RTClib.h"
#include "heltec.h"

const int pH = A0;
const int turbidity = A1;
float pHvalue,temValue,turValue;
bool errpH, errTem, errTur,statusAlarm1=true,statusAlarm2=false,statusAlarm3=false,statusAlarm4=false,statusAlarm5=false,man_hinh_chinh=false;

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
int mangAlarm1[4]={20,8,10,30};
int mangAlarm2[4]={0,0,0,0};
int mangAlarm3[4]={0,0,0,0};
int mangAlarm4[4]={0,0,0,0};
int mangAlarm5[4]={0,0,0,0};


//dinh nghia chan led error
#define LedErr 14
#define LedAlarm 12 

//dinh nghia ten nut nhan
#define buttonUP 35
#define buttonDOWN 34
#define buttonLEFT 39           
#define buttonRIGHT 38
#define buttonENTER 37
#define buttonBACK 36

const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;
const int ENTER = 4;
const int BACK = 5;

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;  //gpio4   

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup() {
  //setup serial
  Serial.begin(9600);

  //setup sensor
  sensors.begin();

  //setup RTC
  rtc.begin(4,15);
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(2020, 10, 18, 17, 50, 30));
  }
  rtc.adjust(DateTime(2020, 10, 18, 17, 50, 30));

  //setup OLED
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Serial Enable*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);

  //setup button
  pinMode(LedErr, OUTPUT);
  pinMode(LedAlarm, OUTPUT);
  pinMode(buttonENTER,INPUT);
  pinMode(buttonBACK,INPUT);
  pinMode(buttonLEFT,INPUT);   
  pinMode(buttonRIGHT,INPUT); 
  pinMode(buttonUP,INPUT);
  pinMode(buttonDOWN,INPUT);
}

float get_pH(){
  unsigned long delay10 = 0;
  unsigned long delay20 = 0;
  int pH_Value = 0;
  unsigned long int avgValue;
  int buf[10], temp_pH;
  float pHVol,phValue;

  for (int i = 0; i < 10; i++){
    if (millis() - delay10 > 10){                 //delay 10ms
      buf[i] = analogRead(pH);
      delay10 = millis();
    }
  }

  for (int i = 0; i < 9; i++){
    for (int j = 0; j < 10; j++){
      if (buf[i] != buf[j]){
        temp_pH = buf[i];
        buf[i] = buf[j];
        buf[j] = temp_pH;
      }
    }
  }

  avgValue = 0;
  for (int i = 2; i < 8; i++)
    avgValue += buf[i];
  if (millis() - delay20 > 20){                   //delay 20ms
    pHVol = (float)avgValue * 5.0 / 1024 / 6;
    phValue = -5.70 * pHVol + 21.34;
    Serial.print("sensor = ");
    Serial.println(phValue);
  }
  return pH_Value;
}

float get_Tem(){
  unsigned long delay5000 = 0;
  float temperatureC;
  if (millis() - delay5000 > 5000){               //delay 5s
    sensors.requestTemperatures(); 
    temperatureC = sensors.getTempCByIndex(0);
    Serial.print(temperatureC);
    Serial.println("ºC");
    delay5000 = millis();
  }
  return temperatureC;
}

float get_turbidity(){
  unsigned long delay500 = 0;
  float voltage;
  if (millis() - delay500 > 500){                 //delay 500ms
    int sensorValue = analogRead(turbidity);      // read the input on analog pin 0:
    voltage = sensorValue * (5.0 / 1024.0);       // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    Serial.println(voltage);                      // print out the value you read:
    delay500 = millis();
  }
  return voltage;
}

void _display(){
  DateTime now = rtc.now();    
  String data7 ="";
  String data8 ="";
  String data1 ="";
  String data2 ="";
  String data3 ="";
  String data4 ="";
  String data5 ="";
  Heltec.display->clear();
  if(man_hinh_chinh==true){
    data7 = (String)daysOfTheWeek[now.dayOfTheWeek()] + "  ,   " + (String)now.day() +"   /   " +(String)now.month()+"   /   "+(String)now.year();
    data8 = (String)now.hour() +"   :   " +(String)now.minute()+"   :   "+(String)now.second();
    Heltec.display->drawString(0,0, data7);
    Heltec.display->drawString(30,10, data8);
    Heltec.display->drawString(0,20, "Nhiet :                *C");
    Heltec.display->drawString(40,20, (String)temValue);
    Heltec.display->drawString(0,30, "Do duc :            NPN");
    Heltec.display->drawString(40,30, (String)turValue);
    Heltec.display->drawString(0,40, "pH :");
    Heltec.display->drawString(40,40, (String)pHvalue);
    if(statusAlarm1==true || statusAlarm2==true || statusAlarm3==true || statusAlarm4==true || statusAlarm5==true){
      Heltec.display->drawString(0,50, "Alarm  :  On");
    }
    Heltec.display->drawString(110,50, "=>");
  }
  else{
    if(statusAlarm1==true){
    Heltec.display->drawString(0,0, "Alarm1:");
    data1 = (String)mangAlarm1[0] + " / " + (String)mangAlarm1[1] + " - " + (String)mangAlarm1[2] + " : " + (String)mangAlarm1[3];
    Heltec.display->drawString(40,0, data1);
    }
    if(statusAlarm2==true){
      Heltec.display->drawString(0,10, "Alarm2:");
      data2 = (String)mangAlarm2[0] + " / " + (String)mangAlarm2[1] + " - " + (String)mangAlarm2[2] + " : " + (String)mangAlarm2[3];
      Heltec.display->drawString(40,10, data2);
    }
    if(statusAlarm3==true){
      Heltec.display->drawString(0,20, "Alarm3:");
      data3 = (String)mangAlarm3[0] + " / " + (String)mangAlarm3[1] + " - " + (String)mangAlarm3[2] + " : " + (String)mangAlarm3[3];
      Heltec.display->drawString(40,20, data3);
    }
    if(statusAlarm4==true){
      Heltec.display->drawString(0,30, "Alarm4:");
      data4 = (String)mangAlarm4[0] + " / " + (String)mangAlarm4[1] + " - " + (String)mangAlarm4[2] + " : " + (String)mangAlarm4[3];
      Heltec.display->drawString(40,30, data4);
    }
    if(statusAlarm5==true){
      Heltec.display->drawString(0,40, "Alarm5:");
      data5 = (String)mangAlarm5[0] + " / " + (String)mangAlarm5[1] + " - " + (String)mangAlarm5[2] + " : " + (String)mangAlarm5[3];
      Heltec.display->drawString(40,40, data5);
    }
  Heltec.display->drawString(0,50, "<=");  
  } 
  Heltec.display->display();
  }

void checkErr(){
  if(pHvalue<5 || pHvalue>8){           //pH ngoai khoang [5;8] thi bao led
    errpH=true;
    digitalWrite(LedErr,HIGH);
  }
  if(temValue<20 || temValue>27){           //nhiet do ngoai khoang [20;27] thi bao den
    errTem=true;
    digitalWrite(LedErr,HIGH);
  }
  if(turValue>80){                          //do duc lon hon 80 NTU thi bao den
    errTur=true;
    digitalWrite(LedErr,HIGH);
  }
  if(pHvalue>=5 && pHvalue<=8 && temValue>=20 && temValue<=27 && turValue<=80){
    errpH=false;
    errTem=false;
    errTur=false;
    digitalWrite(LedErr,LOW);
  }
}

int button(){
  int demEnter=0;
  
  
}


void loop() {
  //pHvalue = get_pH();
  pHvalue = 7;
  //temValue = get_Tem();
  temValue =23;
  //turValue = get_turbidity();
  turValue =20;
  checkErr();
  _display();  
}
