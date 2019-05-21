#include <Wire.h>
#include <INA219.h>
#include <SimpleTimer.h>

//Outputs
#define MAIN_LIGHTS 26
#define BATH_LIGHTS 27
#define KITCHEN_LIGHTS 28
#define BED_LIGHTS 29
#define DIESEL_HEATER 30
#define HOUSE_HEAT 31
#define HOUSE_COOL 32
#define WATER_PUMP 33
#define BED_ON 39
#define BED_DOWN 40

//Inputs
#define THERMO_HEATON 25
#define THERMO_COOLON 24
#define MAIN_LIGHTS_SW 23
#define BED_DN_SW 22
#define BED_UP_SW 21
#define WATER_PUMP_SW 20
#define KITCHEN_LIGHTS_SW 16
#define BATH_LIGHTS_SW 18
#define WATER_HEATER_SW 17
#define BED_LIGHTS_SW 19

INA219 ina219_Inverter2House(INA219::t_i2caddr::I2C_ADDR_41);
float houseVoltage = 0;
float inverterShuntVoltage = 0;
float inverterCurrent = 0;
float inverterPower = 0;
float avghouseVoltage = 0;
float avginverterShuntVoltage = 0;
float avginverterCurrent = 0;
float avginverterPower = 0;
int readingCount = 0;

#define currentReadInterval 3000
SimpleTimer readCurrentTimer;

void setup() {
  // Bridge startup
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(115200);
  digitalWrite(13, HIGH);
  
  Serial.println("Initializing Inverter to House INA219 now");
  ina219_Inverter2House.begin();
  ina219_Inverter2House.configure(INA219::RANGE_16V, INA219::GAIN_2_80MV, INA219::ADC_128SAMP, INA219::ADC_128SAMP, INA219::CONT_SH_BUS);
  ina219_Inverter2House.calibrate(0.0003, 0.075, 16.0, 250.0); 
  Serial.println("Initialized Inverter to House INA219");
  readCurrentTimer.setInterval(currentReadInterval,readCurrent);
  
}

void loop() {
  readCurrentTimer.run();
  doTheIO();
  delay(50);
}

void initOutput(int thepin) {
  digitalWrite(thepin, HIGH);
  pinMode(thepin, OUTPUT);
}

void initIO() {
  initOutput(MAIN_LIGHTS);
  initOutput(BATH_LIGHTS);
  initOutput(KITCHEN_LIGHTS);
  initOutput(BED_LIGHTS);
  initOutput(DIESEL_HEATER);
  initOutput(HOUSE_HEAT);
  initOutput(HOUSE_COOL);
  initOutput(WATER_PUMP);
  initOutput(BED_DOWN);
  initOutput(BED_ON);
  initInput(THERMO_HEATON);
  initInput(THERMO_COOLON);
  initInput(MAIN_LIGHTS_SW);
  initInput(BED_UP_SW);
  initInput(BED_DN_SW);
  initInput(WATER_PUMP_SW);
  initInput(KITCHEN_LIGHTS_SW);
  initInput(WATER_HEATER_SW);
  initInput(BATH_LIGHTS_SW);
  initInput(BED_LIGHTS_SW);
}

void doTheIO() {
  digitalWrite(KITCHEN_LIGHTS, digitalRead(KITCHEN_LIGHTS_SW));
  digitalWrite(MAIN_LIGHTS, digitalRead(MAIN_LIGHTS_SW));
  digitalWrite(BED_LIGHTS, digitalRead(BED_LIGHTS_SW));
  digitalWrite(BATH_LIGHTS, digitalRead(BATH_LIGHTS_SW));
  digitalWrite(WATER_PUMP, digitalRead(WATER_PUMP_SW));
  digitalWrite(HOUSE_HEAT, digitalRead(THERMO_HEATON));
  if (digitalRead(THERMO_HEATON)==LOW || digitalRead(WATER_HEATER_SW)==LOW) {
    digitalWrite(DIESEL_HEATER, LOW);
  } else {
    digitalWrite(DIESEL_HEATER, HIGH);
  }
  digitalWrite(HOUSE_COOL, digitalRead(THERMO_COOLON));
  digitalWrite(BED_DOWN, digitalRead(BED_DN_SW));
  if (digitalRead(BED_DN_SW)==LOW || digitalRead(BED_UP_SW)==LOW ) {
    digitalWrite(BED_ON, LOW);
  } else {
     digitalWrite(BED_ON, HIGH);
  }
}

void readCurrent() 
{
  Serial.println("read current");
  readingCount++;
  houseVoltage = ina219_Inverter2House.busVoltage();
  inverterShuntVoltage = ina219_Inverter2House.shuntVoltage();
  inverterCurrent = ina219_Inverter2House.shuntCurrent();
  inverterPower = ina219_Inverter2House.busPower();
  avghouseVoltage =  ((avghouseVoltage * (readingCount-1)) + houseVoltage)/readingCount;
  avginverterShuntVoltage = ((avginverterShuntVoltage * (readingCount-1)) + inverterShuntVoltage)/readingCount;
  avginverterCurrent = ((avginverterCurrent * (readingCount-1)) + inverterCurrent)/readingCount;
  avginverterPower = ((avginverterPower * (readingCount-1)) + inverterPower)/readingCount;
  printCurrent();
}

void printCurrent()
{
  Serial.print("Now:");
  Serial.print("houseV:"); Serial.print(houseVoltage); Serial.print("/");
  Serial.print("InvSV:"); Serial.print(inverterShuntVoltage*1000); Serial.print("/");
  Serial.print("InvA:"); Serial.print(inverterCurrent); Serial.print("/");
  Serial.print("InvW:"); Serial.print(inverterPower); Serial.println("");
  Serial.print("Avg:");
  Serial.print("houseV:"); Serial.print(avghouseVoltage); Serial.print("/");
  Serial.print("InvSV:"); Serial.print(avginverterShuntVoltage*1000); Serial.print("/");
  Serial.print("InvA:"); Serial.print(avginverterCurrent); Serial.print("/");
  Serial.print("InvW:"); Serial.print(avginverterPower); Serial.println("");
}
void initInput(int thepin) {
  pinMode(thepin, INPUT_PULLUP);
}

void turnOnHeater() {
  digitalWrite(DIESEL_HEATER, LOW);
  digitalWrite(HOUSE_HEAT, LOW);
}

void turnOffHeater() {
  digitalWrite(DIESEL_HEATER, HIGH);
  digitalWrite(HOUSE_HEAT, HIGH);
}
