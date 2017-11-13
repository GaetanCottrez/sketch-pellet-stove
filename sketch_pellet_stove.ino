/*
  
Management of the ignition / extinction of a pellet stove.
Communication used RF433Mhz and use of a relay to turn on or off
  
*/

#include <RCSwitch.h>

RCSwitch Receive433 = RCSwitch();
RCSwitch Transmit433 = RCSwitch();

int Pin_relay = 8;
const int pellet_stove_off = 117;
const int pellet_stove_on = 476;
const int give_me_status = 29;
int current_status = pellet_stove_off;
void setup()
{
  Serial.begin(9600);
  
  Transmit433.enableTransmit(10); // Transmitter is connected to Arduino Pin #10  
  Receive433.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
  
  pinMode(Pin_relay, OUTPUT); 
}
 
void loop() {
  if (Receive433.available()) {
    
    switch(Receive433.getReceivedValue()){
      case pellet_stove_off:
        TurnOffPelletStove();
        Serial.println("Extinction du poele");
        current_status = Receive433.getReceivedValue();
      break;
      
      case pellet_stove_on:
        TurnOnPelletStove();
        Serial.println("Allumage du poele");
        current_status = Receive433.getReceivedValue();
      break;

      case give_me_status:
        Serial.println("donne statut poele");
        SendRF433Mhz(current_status);
      break;
    }
    output(Receive433.getReceivedValue(), Receive433.getReceivedBitlength(), Receive433.getReceivedDelay(), Receive433.getReceivedRawdata(),Receive433.getReceivedProtocol());
    Receive433.resetAvailable();
  }
}

void TurnOnPelletStove(){
  digitalWrite(Pin_relay, HIGH); // turn on relay
}

void TurnOffPelletStove(){
  digitalWrite(Pin_relay, LOW); // turn off relay
}

void SendRF433Mhz(int send_status){
  Transmit433.send(send_status, 24);
}

static const char* bin2tristate(const char* bin);
static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength);

void output(unsigned long decimal, unsigned int length, unsigned int delay, unsigned int* raw, unsigned int protocol) {

  if (decimal == 0) {
    Serial.print("Unknown encoding.");
  } else {
    const char* b = dec2binWzerofill(decimal, length);
    Serial.print("Decimal: ");
    Serial.print(decimal);
    Serial.print(" (");
    Serial.print( length );
    Serial.print("Bit) Binary: ");
    Serial.print( b );
    Serial.print(" Tri-State: ");
    Serial.print( bin2tristate( b) );
    Serial.print(" PulseLength: ");
    Serial.print(delay);
    Serial.print(" microseconds");
    Serial.print(" Protocol: ");
    Serial.println(protocol);
  }
  
  Serial.print("Raw data: ");
  for (unsigned int i=0; i<= length*2; i++) {
    Serial.print(raw[i]);
    Serial.print(",");
  }
  Serial.println();
  Serial.println();
}

static const char* bin2tristate(const char* bin) {
  static char returnValue[50];
  int pos = 0;
  int pos2 = 0;
  while (bin[pos]!='\0' && bin[pos+1]!='\0') {
    if (bin[pos]=='0' && bin[pos+1]=='0') {
      returnValue[pos2] = '0';
    } else if (bin[pos]=='1' && bin[pos+1]=='1') {
      returnValue[pos2] = '1';
    } else if (bin[pos]=='0' && bin[pos+1]=='1') {
      returnValue[pos2] = 'F';
    } else {
      return "not applicable";
    }
    pos = pos+2;
    pos2++;
  }
  returnValue[pos2] = '\0';
  return returnValue;
}

static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength) {
  static char bin[64]; 
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    } else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}
