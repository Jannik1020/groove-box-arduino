int analogPin = A1;
int raw = 0;

const float V_REF = 5.0;     // Analog reference voltage (e.g., 5V or 3.3V)
const float R_BITS = 10.0;   // ADC resolution (bits)
const float ADC_STEPS = (1 << int(R_BITS)) - 1; // Number of steps (2^R_BITS - 1)

const int max1 = 1023;
const int max2 = 919;
const int max3 = 746;
const int max4 =  628;
const int max5 =  545;
const int max6 =  483;
const int max7 =  431;
const int max8 =  389;
const int max9 =  355;
const int max10 =  328;
const int max11 =  303;
const int max12 =  282;
const int max13 =  264;
const int max14 =  248;
const int max15 =  234;
const int max16 =  221;

char last = ' ';
char new1 = ' ';

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(ADC_STEPS);
}

void loop() {

  raw = analogRead(analogPin);

  //Serial.println(raw);

  switch(raw) {
    case max2 + 1 ... max1:
      new1 = '1';
      break;
    case max3 + 1 ... max2:
      new1 = '2';
      break;
    case max4 + 1 ... max3:
      new1 = '3';
      break;
    case max5 + 1 ... max4:
      new1 = 'A';
      break;
    case max6 + 1 ... max5:
      new1 = '4';
      break;
    case max7 + 1 ... max6:
      new1 = '5';
      break;
    case max8 + 1 ... max7:
      new1 = '6';
      break;
    case max9 + 1 ... max8:
      new1 = 'B';
      break;
    case max10 + 1 ... max9:
      new1 = '7';
      break;
    case max11 + 1 ... max10:
      new1 = '8';
      break;
    case max12 + 1 ... max11:
      new1 = '9';
      break;
    case max13 + 1 ... max12:
      new1 = 'C';
      break;
    case max14 + 1 ... max13:
      new1 = '*';
      break;
    case max15 + 1 ... max14:
      new1 = '0';
      break;
    case max16 + 1 ... max15:
      new1 = '#';
      break;
    case 10 ... max16:
      new1 = 'D';
      break;
    default:
      new1 = 0;
  }

  if(new1 != last) {
    last = new1;
    Serial.println(new1);
  }

/*
  float voltage = (raw / ADC_STEPS) * V_REF;

  Serial.print("Voltage: ");
  Serial.print(voltage, 3); // Print voltage with 3 decimal places
  Serial.println(" V");

  float resistance = 0.0;
  resistance = (ADC_STEPS / raw)  - 1;     // (1023/ADC - 1) 
  resistance = SERIESRESISTOR / resistance;  // 10K / (1023/ADC - 1)

  Serial.print("Resistance: ");
  Serial.print(resistance, 3); // Print voltage with 3 decimal places
  Serial.println(" Ohm");
*/

}
