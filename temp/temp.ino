#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>  
#include "RTClib.h"
#include "heltec.h"


const int pH = 33;
const int turbidity = 32;
float pHvalue,temValue,turValue;
bool errpH, errTem, errTur,man_hinh_chinh=true;
int dem=0,i=1;
unsigned long delay1s=0,delay2s=0,delay500=0,_delay1s=0,delay3s=0,delay5s=0,delay10=0,delay20=0,_delay200=0;

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
int mangAlarm1[5]={1,1,0,0,0};
int mangAlarm2[5]={1,1,0,0,0};
int mangAlarm3[5]={1,1,0,0,0};
int mangAlarm4[5]={1,1,0,0,0};
int mangAlarm5[5]={1,1,0,0,0};
String setAlarm[5]={"Set Day","Set Mon","Set Hour","Set Min","Set State"};

//dinh nghia chan led error
#define LedErr 14
#define LedAlarm 12 

const int UP = 0;
const int DOWN = 1;
const int LEFT = 2;
const int RIGHT = 3;
const int ENTER = 4;

const int SW_pin = 39; // digital pin connected to switch output
const int X_pin = 34; // analog pin connected to X output
const int Y_pin = 35; // analog pin connected to Y output

// GPIO where the DS18B20 is connected to
const int oneWireBus = 27;  

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

  pinMode(SW_pin,INPUT);
}

float get_pH(){
  unsigned long endTime = millis();
  int pH_Value = 0;
  unsigned long int avgValue;
  int buf[10], temp_pH;
  float pHVol,phValue;

  for (int i = 0; i < 10; i++){
    if (endTime - delay10 > 10){                 //delay 10ms
      buf[i] = analogRead(pH);
      delay10 = endTime;
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
  if (endTime - delay20 > 20){                   //delay 20ms
    pHVol = (float)avgValue * 5.0 / 1024 / 6;
    phValue = -5.70 * pHVol + 21.34;
    Serial.print("sensor = ");
    Serial.println(phValue);
    delay20=endTime;
  }
  return pH_Value;
}

float get_Tem(){
  unsigned long endTime= millis();
  float temperatureC;
  if (endTime - delay5s > 5000){               //delay 5s
    sensors.requestTemperatures(); 
    temperatureC = sensors.getTempCByIndex(0);
    Serial.print(temperatureC);
    Serial.println("ºC");
    delay5s = endTime;
  }
  return temperatureC;
}

float get_turbidity(){
  unsigned long endTime = millis();
  float voltage;
  if (endTime - delay3s > 3000){                 //delay 500ms
    int sensorValue = analogRead(turbidity);      // read the input on analog pin 0:
    voltage = sensorValue * (5.0 / 1024.0);       // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    Serial.println(voltage);                      // print out the value you read:
    delay3s = endTime;
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
  if(man_hinh_chinh==true){
    if(errpH==true || errTem==true || errTur==true){
      unsigned long endTime = millis();
      if(endTime-delay500>500){
        Heltec.display->clear();
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
        if(mangAlarm1[4]==1 || mangAlarm2[4]==1 || mangAlarm3[4]==1 || mangAlarm4[4]==1 || mangAlarm5[4]==1){
          Heltec.display->drawString(0,50, "Alarm : On");
        }
        Heltec.display->drawString(110,50, "=>");
        if(errpH==true){
          Heltec.display->drawString(60,50, "Err : pH");
        }
        if(errTem==true){
          Heltec.display->drawString(60,50, "Err : Tem");
        }
        if(errTur==true){
          Heltec.display->drawString(60,50, "Err : Tur");
        }
        delay500=endTime;
      }
      if(endTime-_delay1s>1000){
        Heltec.display->clear();
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
        if(mangAlarm1[4]==1 || mangAlarm2[4]==1 || mangAlarm3[4]==1 || mangAlarm4[4]==1 || mangAlarm5[4]==1){
          Heltec.display->drawString(0,50, "Alarm : On");
        }
        Heltec.display->drawString(110,50, "=>");
        _delay1s=endTime;
      }
    }
    else{
      Heltec.display->clear();
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
      if(mangAlarm1[4]==1 || mangAlarm2[4]==1 || mangAlarm3[4]==1 || mangAlarm4[4]==1 || mangAlarm5[4]==1){
        Heltec.display->drawString(0,50, "Alarm : On");
      }
      Heltec.display->drawString(110,50, "=>");
    }
  }
  else{
    Heltec.display->clear();
    Heltec.display->drawString(0,0, "Alarm1:");
    String x="";
    if(mangAlarm1[4]==0){
      x = "Off";
    }
    if(mangAlarm1[4]==1){
      x = "On";
    }
    data1 = (String)mangAlarm1[0] + " / " + (String)mangAlarm1[1] + " - " + (String)mangAlarm1[2] + " : " + (String)mangAlarm1[3] + "  " + x;
    Heltec.display->drawString(40,0, data1);

    String y="";
    if(mangAlarm2[4]==0){
      y = "Off";
    }
    if(mangAlarm2[4]==1){
      y = "On";
    }
    Heltec.display->drawString(0,10, "Alarm2:");
    data2 = (String)mangAlarm2[0] + " / " + (String)mangAlarm2[1] + " - " + (String)mangAlarm2[2] + " : " + (String)mangAlarm2[3] + "  " + y;
    Heltec.display->drawString(40,10, data2);

    String z="";
    if(mangAlarm3[4]==0){
      z = "Off";
    }
    if(mangAlarm3[4]==1){
      z = "On";
    }
    Heltec.display->drawString(0,20, "Alarm3:");
    data3 = (String)mangAlarm3[0] + " / " + (String)mangAlarm3[1] + " - " + (String)mangAlarm3[2] + " : " + (String)mangAlarm3[3] + "  " + z;
    Heltec.display->drawString(40,20, data3);

    String t="";
    if(mangAlarm4[4]==0){
      t = "Off";
    }
    if(mangAlarm4[4]==1){
      t = "On";
    }
    Heltec.display->drawString(0,30, "Alarm4:");
    data4 = (String)mangAlarm4[0] + " / " + (String)mangAlarm4[1] + " - " + (String)mangAlarm4[2] + " : " + (String)mangAlarm4[3] + "  " + t;
    Heltec.display->drawString(40,30, data4);

    String u="";
    if(mangAlarm5[4]==0){
      u = "Off";
    }
    if(mangAlarm5[4]==1){
      u = "On";
    }
    Heltec.display->drawString(0,40, "Alarm5:");
    data5 = (String)mangAlarm5[0] + " / " + (String)mangAlarm5[1] + " - " + (String)mangAlarm5[2] + " : " + (String)mangAlarm5[3] + "  " + u;
    Heltec.display->drawString(40,40, data5);

    Heltec.display->drawString(20,50, setAlarm[i]);
    Heltec.display->drawString(70,50, "Alarm");
    Heltec.display->drawString(100,50, String(dem));
    Heltec.display->drawString(0,50, "<=");
  } 
  Heltec.display->display();
  }

void checkErr(){
  if(pHvalue<5 || pHvalue>8){               //pH ngoai khoang [5;8] thi bao led
    errpH=true;
    unsigned long endTime = millis();
    if(endTime-delay1s>1000){
      digitalWrite(LedErr,HIGH);
      delay1s=endTime;
    }
    if(endTime-delay2s>2000){
      digitalWrite(LedErr,LOW);
      delay2s=endTime;
    } 
  }
  if(temValue<20 || temValue>27){           //nhiet do ngoai khoang [20;27] thi bao den
    errTem=true;
    unsigned long endTime = millis();
    if(endTime-delay1s>1000){
      digitalWrite(LedErr,HIGH);
      delay1s=endTime;
    }
    if(endTime-delay2s>2000){
      digitalWrite(LedErr,LOW);
      delay2s=endTime;
    } 
  }
  if(turValue>80){                          //do duc lon hon 80 NTU thi bao den
    errTur=true;
    unsigned long endTime = millis();
    if(endTime-delay1s>1000){
      digitalWrite(LedErr,HIGH);
      delay1s=endTime;
    }
    if(endTime-delay2s>2000){
      digitalWrite(LedErr,LOW);
      delay2s=endTime;
    } 
  }
  if(pHvalue>=5 && pHvalue<=8 && temValue>=20 && temValue<=27 && turValue<=80){
    errpH=false;
    errTem=false;
    errTur=false;
    digitalWrite(LedErr,LOW);
  }
}

int joystick(){
    uint x=map(analogRead(X_pin),0,4095,0,255);
    uint y=map(analogRead(Y_pin),0,4095,0,255);
    int sw=digitalRead(SW_pin);
    if(x>150 && y>65 && y<75){      //up
      return UP;
    }
    if(x==0 && y>65 && y<75){       //down
      return DOWN;
    }
    if(y>150 && x>65 && x<75){      //left
      return LEFT;
    }
    if(y==0 && x>65 && x<75){       //right
      return RIGHT;
    }
    if(!sw){               //enter
      return ENTER;   
    }
}

void alarm(){
  unsigned long endTime=millis();
  if(endTime - _delay200 >200){
    int temp = joystick();
    if(temp==ENTER){
      dem=dem+1;
    }
    if (temp==LEFT)
    {
      i--;
      if (i<0) i=4;   
    }
    if (temp==RIGHT)
    {
      i++;
      if(i>4) i=0;
    }
    if(dem==1){
      man_hinh_chinh=false;
      if (temp==UP)
      {
        if(i==1){
          mangAlarm1[1]++;
          if(mangAlarm1[1]>12) mangAlarm1[1]=1;
        }
        if(i==0){
          if(mangAlarm1[1]==1 || mangAlarm1[1]==3 || mangAlarm1[1]==5 || mangAlarm1[1]==7 || mangAlarm1[1]==8 || mangAlarm1[1]==10 || mangAlarm1[1]==12){
            mangAlarm1[0]++;
            if(mangAlarm1[0]>31)  mangAlarm1[0]=1;
          }
          if(mangAlarm1[1]==4 || mangAlarm1[1]==6 || mangAlarm1[1]==9 || mangAlarm1[1]==11){
            mangAlarm1[0]++;
            if(mangAlarm1[0]>30)  mangAlarm1[0]=1;
          }
          if(mangAlarm1[1]==2){
            mangAlarm1[0]++;
            if(mangAlarm1[0]>28)  mangAlarm1[0]=1;
          }
        }
        if(i==2){
          mangAlarm1[2]++;
          if(mangAlarm1[2]>23)  mangAlarm1[2]=0;
        }
        if(i==3){
          mangAlarm1[3]++;
          if(mangAlarm1[3]>59)  mangAlarm1[3]=0;
        }
        if(i==4){
          mangAlarm1[4]++;
          if(mangAlarm1[4]>1) mangAlarm1[4]=0;
        }
      }
      if (temp==DOWN)
      {
        if(i==1){
          mangAlarm1[1]--;
          if(mangAlarm1[1]<1) mangAlarm1[1]=12;
        }
        if(i==0){
          if(mangAlarm1[1]==1 || mangAlarm1[1]==3 || mangAlarm1[1]==5 || mangAlarm1[1]==7 || mangAlarm1[1]==8 || mangAlarm1[1]==10 || mangAlarm1[1]==12){
            mangAlarm1[0]--;
            if(mangAlarm1[0]<1)  mangAlarm1[0]=31;
          }
          if(mangAlarm1[1]==4 || mangAlarm1[1]==6 || mangAlarm1[1]==9 || mangAlarm1[1]==11){
            mangAlarm1[0]--;
            if(mangAlarm1[0]<1)  mangAlarm1[0]=30;
          }
          if(mangAlarm1[1]==2){
            mangAlarm1[0]--;
            if(mangAlarm1[0]<1)  mangAlarm1[0]=28;
          }
        }
        if(i==2){
          mangAlarm1[2]--;
          if(mangAlarm1[2]<0)  mangAlarm1[2]=23;
        }
        if(i==3){
          mangAlarm1[3]--;
          if(mangAlarm1[3]<0)  mangAlarm1[3]=59;
        }
        if(i==4){
          mangAlarm1[4]--;
          if(mangAlarm1[4]<0) mangAlarm1[4]=1;
        }
      }
    }
    if(dem==2){
      if (temp==UP)
      {
        if(i==1){
          mangAlarm2[1]++;
          if(mangAlarm2[1]>12) mangAlarm2[1]=1;
        }
        if(i==0){
          if(mangAlarm2[1]==1 || mangAlarm2[1]==3 || mangAlarm2[1]==5 || mangAlarm2[1]==7 || mangAlarm2[1]==8 || mangAlarm2[1]==10 || mangAlarm2[1]==12){
            mangAlarm2[0]++;
            if(mangAlarm2[0]>31)  mangAlarm2[0]=1;
          }
          if(mangAlarm2[1]==4 || mangAlarm2[1]==6 || mangAlarm2[1]==9 || mangAlarm2[1]==11){
            mangAlarm2[0]++;
            if(mangAlarm2[0]>30)  mangAlarm2[0]=1;
          }
          if(mangAlarm2[1]==2){
            mangAlarm2[0]++;
            if(mangAlarm2[0]>28)  mangAlarm2[0]=1;
          }
        }
        if(i==2){
          mangAlarm2[2]++;
          if(mangAlarm2[2]>23)  mangAlarm2[2]=0;
        }
        if(i==3){
          mangAlarm2[3]++;
          if(mangAlarm2[3]>59)  mangAlarm2[3]=0;
        }
        if(i==4){
          mangAlarm2[4]++;
          if(mangAlarm2[4]>1) mangAlarm2[4]=0;
        }
      }
      if (temp==DOWN)
      {
        if(i==1){
          mangAlarm2[1]--;
          if(mangAlarm2[1]<1) mangAlarm2[1]=12;
        }
        if(i==0){
          if(mangAlarm2[1]==1 || mangAlarm2[1]==3 || mangAlarm2[1]==5 || mangAlarm2[1]==7 || mangAlarm2[1]==8 || mangAlarm2[1]==10 || mangAlarm2[1]==12){
            mangAlarm2[0]--;
            if(mangAlarm2[0]<1)  mangAlarm2[0]=31;
          }
          if(mangAlarm2[1]==4 || mangAlarm2[1]==6 || mangAlarm2[1]==9 || mangAlarm2[1]==11){
            mangAlarm2[0]--;
            if(mangAlarm2[0]<1)  mangAlarm2[0]=30;
          }
          if(mangAlarm2[1]==2){
            mangAlarm2[0]--;
            if(mangAlarm2[0]<1)  mangAlarm2[0]=28;
          }
        }
        if(i==2){
          mangAlarm2[2]--;
          if(mangAlarm2[2]<0)  mangAlarm2[2]=23;
        }
        if(i==3){
          mangAlarm2[3]--;
          if(mangAlarm2[3]<0)  mangAlarm2[3]=59;
        }
        if(i==4){
          mangAlarm2[4]--;
          if(mangAlarm2[4]<0) mangAlarm2[4]=1;
        }
      }
    }
    if(dem==3){
      if (temp==UP)
      {
        if(i==1){
          mangAlarm3[1]++;
          if(mangAlarm3[1]>12) mangAlarm3[1]=1;
        }
        if(i==0){
          if(mangAlarm3[1]==1 || mangAlarm3[1]==3 || mangAlarm3[1]==5 || mangAlarm3[1]==7 || mangAlarm3[1]==8 || mangAlarm3[1]==10 || mangAlarm3[1]==12){
            mangAlarm3[0]++;
            if(mangAlarm3[0]>31)  mangAlarm3[0]=1;
          }
          if(mangAlarm3[1]==4 || mangAlarm3[1]==6 || mangAlarm3[1]==9 || mangAlarm3[1]==11){
            mangAlarm3[0]++;
            if(mangAlarm3[0]>30)  mangAlarm3[0]=1;
          }
          if(mangAlarm3[1]==2){
            mangAlarm3[0]++;
            if(mangAlarm3[0]>28)  mangAlarm3[0]=1;
          }
        }
        if(i==2){
          mangAlarm3[2]++;
          if(mangAlarm3[2]>23)  mangAlarm3[2]=0;
        }
        if(i==3){
          mangAlarm3[3]++;
          if(mangAlarm3[3]>59)  mangAlarm3[3]=0;
        }
        if(i==4){
          mangAlarm3[4]++;
          if(mangAlarm3[4]>1) mangAlarm3[4]=0;
        }
      }
      if (temp==DOWN)
      {
        if(i==1){
          mangAlarm3[1]--;
          if(mangAlarm3[1]<1) mangAlarm3[1]=12;
        }
        if(i==0){
          if(mangAlarm3[1]==1 || mangAlarm3[1]==3 || mangAlarm3[1]==5 || mangAlarm3[1]==7 || mangAlarm3[1]==8 || mangAlarm3[1]==10 || mangAlarm3[1]==12){
            mangAlarm3[0]--;
            if(mangAlarm3[0]<1)  mangAlarm3[0]=31;
          }
          if(mangAlarm3[1]==4 || mangAlarm3[1]==6 || mangAlarm3[1]==9 || mangAlarm3[1]==11){
            mangAlarm3[0]--;
            if(mangAlarm3[0]<1)  mangAlarm3[0]=30;
          }
          if(mangAlarm3[1]==2){
            mangAlarm3[0]--;
            if(mangAlarm3[0]<1)  mangAlarm3[0]=28;
          }
        }
        if(i==2){
          mangAlarm3[2]--;
          if(mangAlarm3[2]<0)  mangAlarm3[2]=23;
        }
        if(i==3){
          mangAlarm3[3]--;
          if(mangAlarm3[3]<0)  mangAlarm3[3]=59;
        }
        if(i==4){
          mangAlarm3[4]--;
          if(mangAlarm3[4]<0) mangAlarm3[4]=1;
        }
      }
    }
    if(dem==4){
      if (temp==UP)
      {
        if(i==1){
          mangAlarm4[1]++;
          if(mangAlarm4[1]>12) mangAlarm4[1]=1;
        }
        if(i==0){
          if(mangAlarm4[1]==1 || mangAlarm4[1]==3 || mangAlarm4[1]==5 || mangAlarm4[1]==7 || mangAlarm4[1]==8 || mangAlarm4[1]==10 || mangAlarm4[1]==12){
            mangAlarm4[0]++;
            if(mangAlarm4[0]>31)  mangAlarm4[0]=1;
          }
          if(mangAlarm4[1]==4 || mangAlarm4[1]==6 || mangAlarm4[1]==9 || mangAlarm4[1]==11){
            mangAlarm4[0]++;
            if(mangAlarm4[0]>30)  mangAlarm4[0]=1;
          }
          if(mangAlarm4[1]==2){
            mangAlarm4[0]++;
            if(mangAlarm4[0]>28)  mangAlarm4[0]=1;
          }
        }
        if(i==2){
          mangAlarm4[2]++;
          if(mangAlarm4[2]>23)  mangAlarm4[2]=0;
        }
        if(i==3){
          mangAlarm4[3]++;
          if(mangAlarm4[3]>59)  mangAlarm4[3]=0;
        }
        if(i==4){
          mangAlarm4[4]++;
          if(mangAlarm4[4]>1) mangAlarm4[4]=0;
        }
      }
      if (temp==DOWN)
      {
        if(i==1){
          mangAlarm4[1]--;
          if(mangAlarm4[1]<1) mangAlarm4[1]=12;
        }
        if(i==0){
          if(mangAlarm4[1]==1 || mangAlarm4[1]==3 || mangAlarm4[1]==5 || mangAlarm4[1]==7 || mangAlarm4[1]==8 || mangAlarm4[1]==10 || mangAlarm4[1]==12){
            mangAlarm4[0]--;
            if(mangAlarm4[0]<1)  mangAlarm4[0]=31;
          }
          if(mangAlarm4[1]==4 || mangAlarm4[1]==6 || mangAlarm4[1]==9 || mangAlarm4[1]==11){
            mangAlarm4[0]--;
            if(mangAlarm4[0]<1)  mangAlarm4[0]=30;
          }
          if(mangAlarm4[1]==2){
            mangAlarm4[0]--;
            if(mangAlarm4[0]<1)  mangAlarm4[0]=28;
          }
        }
        if(i==2){
          mangAlarm4[2]--;
          if(mangAlarm4[2]<0)  mangAlarm4[2]=23;
        }
        if(i==3){
          mangAlarm4[3]--;
          if(mangAlarm4[3]<0)  mangAlarm4[3]=59;
        }
        if(i==4){
          mangAlarm4[4]--;
          if(mangAlarm4[4]<0) mangAlarm4[4]=1;
        }
      }
    }
    if(dem==5){
      if (temp==UP)
      {
        if(i==1){
          mangAlarm5[1]++;
          if(mangAlarm5[1]>12) mangAlarm5[1]=1;
        }
        if(i==0){
          if(mangAlarm5[1]==1 || mangAlarm5[1]==3 || mangAlarm5[1]==5 || mangAlarm5[1]==7 || mangAlarm5[1]==8 || mangAlarm5[1]==10 || mangAlarm5[1]==12){
            mangAlarm5[0]++;
            if(mangAlarm5[0]>31)  mangAlarm5[0]=1;
          }
          if(mangAlarm5[1]==4 || mangAlarm5[1]==6 || mangAlarm5[1]==9 || mangAlarm5[1]==11){
            mangAlarm5[0]++;
            if(mangAlarm5[0]>30)  mangAlarm5[0]=1;
          }
          if(mangAlarm5[1]==2){
            mangAlarm5[0]++;
            if(mangAlarm5[0]>28)  mangAlarm5[0]=1;
          }
        }
        if(i==2){
          mangAlarm5[2]++;
          if(mangAlarm5[2]>23)  mangAlarm5[2]=0;
        }
        if(i==3){
          mangAlarm5[3]++;
          if(mangAlarm5[3]>59)  mangAlarm5[3]=0;
        }
        if(i==4){
          mangAlarm5[4]++;
          if(mangAlarm5[4]>1) mangAlarm5[4]=0;
        }
      }
      if (temp==DOWN)
      {
        if(i==1){
          mangAlarm5[1]--;
          if(mangAlarm5[1]<1) mangAlarm5[1]=12;
        }
        if(i==0){
          if(mangAlarm5[1]==1 || mangAlarm5[1]==3 || mangAlarm5[1]==5 || mangAlarm5[1]==7 || mangAlarm5[1]==8 || mangAlarm5[1]==10 || mangAlarm5[1]==12){
            mangAlarm5[0]--;
            if(mangAlarm5[0]<1)  mangAlarm5[0]=31;
          }
          if(mangAlarm5[1]==4 || mangAlarm5[1]==6 || mangAlarm5[1]==9 || mangAlarm5[1]==11){
            mangAlarm5[0]--;
            if(mangAlarm5[0]<1)  mangAlarm5[0]=30;
          }
          if(mangAlarm5[1]==2){
            mangAlarm5[0]--;
            if(mangAlarm5[0]<1)  mangAlarm5[0]=28;
          }
        }
        if(i==2){
          mangAlarm5[2]--;
          if(mangAlarm5[2]<0)  mangAlarm5[2]=23;
        }
        if(i==3){
          mangAlarm5[3]--;
          if(mangAlarm5[3]<0)  mangAlarm5[3]=59;
        }
        if(i==4){
          mangAlarm5[4]--;
          if(mangAlarm5[4]<0) mangAlarm5[4]=1;
        }
      }
    }
    if(dem==6){
      dem=0;
      man_hinh_chinh=true;
    }
    _delay200=endTime;
  }
  
}

void loop() {
  //pHvalue = get_pH();
  pHvalue = 7;
  //temValue = get_Tem();
  temValue = 23;
  //turValue = get_turbidity();
  turValue = 50;
  checkErr();
  alarm();
  _display();  
}
