#include <OneWire.h>
#include <DallasTemperature.h>

const int pH = A0;
const int turbidity = A1;
float pHvalue,temValue,turValue;

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;  //gpio4   

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  sensors.begin();
}

float get_pH(){
  unsigned long delay10 = 0;
  unsigned long delay20 = 0;
  int pH_Value = 0;
  unsigned long int avgValue;
  int buf[10], temp_pH;
  float pHVol,phValue;

  for (int i = 0; i < 10; i++){
    if (millis() - delay10 > 10){       //delay 10ms
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
  if (millis() - delay20 > 20){         //delay 20ms
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
  if (millis() - delay5000 > 5000){       //delay 5s
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
    voltage = sensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    Serial.println(voltage);                      // print out the value you read:
    delay500 = millis();
  }
  return voltage;
}

void loop() {
  pHvalue = get_pH();
  temValue = get_Tem();
  turValue = get_turbidity();

}
