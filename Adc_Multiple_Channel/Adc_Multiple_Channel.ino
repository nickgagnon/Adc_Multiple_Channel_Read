#define NbAdcValues 100

uint16_t ADCvalue[NbAdcValues];
uint16_t adcValueIndex = 0;
volatile uint32_t AnalogData = 0;
volatile uint16_t cmptr = 0;
uint8_t channel = 0;

uint16_t IntervalPrint = 1000;
uint32_t LastPrint = 0;

void setup() {
  memset(ADCvalue, 0, NbAdcValues);
  ADCSRA = 0b10101111;
  ADCSRB = 0b00000000;
  ADMUX = 0b01000000;
  ADMUX |= channel;
  Serial.begin(115200);

  ADCSRA |= 0b01000000; // Start the ADC
}

void loop() {
  uint32_t Now = millis();

  if ((Now - LastPrint) > IntervalPrint) {
    channel++;
    if (channel == 6) {
      Serial.println("----------------------");
      channel = 0;
    }

    cli();
    uint32_t copyAnalogData = AnalogData;
    ADMUX &= ~0b00001111;
    ADMUX |= channel;
    uint16_t copycmptr = cmptr;
    cmptr = 0;
    sei();

    copyAnalogData /= NbAdcValues;
    Serial.print(channel);
    Serial.print('\t');
    Serial.print(copyAnalogData);
    Serial.print('\t');
    Serial.println(copycmptr);

    LastPrint = Now;
  }

}

ISR(ADC_vect) {
  AnalogData -= ADCvalue[adcValueIndex];
  ADCvalue[adcValueIndex] = ADCL | (ADCH << 8);
  AnalogData += ADCvalue[adcValueIndex];
  adcValueIndex++;

  if (adcValueIndex == NbAdcValues) {
    adcValueIndex = 0;
  }
  cmptr++;
}
