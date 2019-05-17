/*
  Arduino sketch that approximates LDR lux equation.
  Created by Vektor Sepesi, 2019
  MIT license
*/

#include <math.h>
#include <Wire.h>
#include <BH1750FVI.h>

#define LDR_PIN A0
#define N_ACQUISITIONS 10 // At least 10 to make a good approxiamtion.

BH1750FVI luxMeter(BH1750FVI::k_DevModeContHighRes);

uint8_t iAcquisition = 0;
uint16_t x[N_ACQUISITIONS];
uint32_t y[N_ACQUISITIONS];
float a = 0;
float b = 0;
float r2 = 0;

void setup() {
  luxMeter.begin();
  Serial.begin(9600);
  printHelp();
}

void loop() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    if (incomingByte == '\r') {
      acquireData();
      iAcquisition++;
      if (iAcquisition >= N_ACQUISITIONS) {
        iAcquisition = 0;
        calculateFunction();
        calculateR2();
        printFunction();
        printR2();
        printLine(50);
        printHelp();
      }
    }
  }
}

void acquireData() {
  x[iAcquisition] = analogRead(LDR_PIN);
  y[iAcquisition] = luxMeter.GetLightIntensity();

  Serial.print(F("Acquisition no. "));
  Serial.print(iAcquisition+1);
  Serial.print(F(", analogRead = "));
  Serial.print(x[iAcquisition]);
  Serial.print(F(", lux = "));
  Serial.println(y[iAcquisition]);  
}

void calculateFunction() {
  float multXiYi = 0;
  float lnYi = 0;
  float sum1 = 0;
  float sum2 = 0;
  float sum3 = 0;
  float sum4 = 0;
  float sum5 = 0;

  for (uint8_t i = 0; i < N_ACQUISITIONS; i++) {
    multXiYi = x[i] * y[i];
    lnYi = log(y[i]);
    sum1 += x[i] * multXiYi;
    sum2 += y[i] * lnYi;
    sum3 += multXiYi;
    sum4 += multXiYi * lnYi;
    sum5 += y[i];
  }

  float denom = sum5 * sum1 - sum3 * sum3;
  a = exp((sum1 * sum2 - sum3 * sum4) / denom);
  b = (sum5 * sum4 - sum3 * sum2) / denom;
}

void calculateR2() {
  float sumYi = 0;
  float mean = 0;
  float sum6 = 0;
  float sum7 = 0;
  
  for (uint8_t i = 0; i < N_ACQUISITIONS; i++) {
    sumYi += y[i];
  }
  
  mean = sumYi / N_ACQUISITIONS;
  
  for (uint8_t i = 0; i < N_ACQUISITIONS; i++) {
    sum6 += sq(y[i] - (a * exp(x[i] * b)));
    sum7 += sq(y[i] - mean);
  }
  
  r2 = 1 - (sum6 / sum7);

  if (r2 > 1.0 || r2 < 0) {
    r2 = 0;
  }
}

void printFunction() {
  Serial.println(F("Result:"));
  Serial.print(F("lux = "));
  printCoeff(a);
  Serial.print(F(" * exp("));
  printCoeff(b);
  Serial.println(F(" * analogRead(ldrPin));"));
}

void printR2() {
  Serial.print(F("R2 = "));
  Serial.println(r2);
  if (!(r2 > 0.9 && r2 <= 1.0)) {
    Serial.println(F("Not an accurate approximation, repeat the process."));
  }
}

void printLine(uint8_t len) {
  for (uint8_t i = 0; i <= len; i++) {
    Serial.print(F("-"));
  }
  Serial.println("");
}

void printCoeff(float &var) {
  char buffer[15];
  uint8_t len;

  if (var < 10) {
    len = 1;
  } else if (var < 100) {
    len = 2;
  } else {
    len = 3;
  }
  
  dtostrf(var, len, 9, buffer);
  Serial.print(buffer);
}

void printHelp() {
  Serial.print(F("Illuminate the sensors with "));
  Serial.print(N_ACQUISITIONS);
  Serial.println(F(" different light intensities."));
  Serial.println(F("At each light intensity press 'Enter' to acquire data."));
  Serial.println(F("Result will be shown after all acquisitions are made."));
}
