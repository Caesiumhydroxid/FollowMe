#define SPI_SS 10
#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCK 13
#define nIRQ 2
#define TXEN 9
#define RXEN 8
#include<SPI.h>
void setup() {
  pinMode(nIRQ, INPUT);
  // Pins für TXEN und RXEN
  pinMode(TXEN, OUTPUT);
  pinMode(RXEN, OUTPUT);
  pinMode(7,INPUT);
  // und gleich einmal definieren
  digitalWrite(TXEN, LOW);
  digitalWrite(RXEN, LOW);
  delay(3000);
  spi_init();
   Serial.begin(115200);
  Serial.println("Start");
  // Funkmodul initialisieren
  rfm12_init();
 
}
int deltaMicros;
byte sen;
void loop() {
  unsigned int reg;
    //TXEN auf 1 und gleichzeitig RXEN auf 0 ... damit Übertragung"genehmigt"
    digitalWrite(TXEN, HIGH);
    digitalWrite(RXEN, LOW);
    reg = rfm12_write(0x0000); //Status Register lesen
    //Serial.println(reg, HEX);
    rfm12_write(0x823D); // Power Management Register neu senden
    // Preamble senden:
    deltaMicros = micros();
    rfm12_send(0xAA);
    rfm12_send(0xAA);
    rfm12_send(0xAA);
    // Synchronisieren:
    rfm12_send(0x2D); // SYNC HIGH Byte
    rfm12_send(0xD4); // SYNC LOW Byte
    // Daten senden:
    deltaMicros = micros();
    rfm12_send(sen);
    //while(digitalRead(7)==0);
    int x = micros()-deltaMicros;
    Serial.println(x); 
    //Dummy Bytes:
    rfm12_send(0xAA);
    rfm12_send(0xAA);
    rfm12_send(0xAA);
    //Übertragung fretig -> also TXEN wieder auf 0
    digitalWrite(TXEN, LOW);
    Serial.println("Sent");
    Serial.println(reg,HEX);
    delay(500);
    sen++;
}

uint16_t rfm12_write(uint16_t val){
  Serial.println(val);
    SPI.beginTransaction(SPISettings(500000,MSBFIRST,SPI_MODE0));
    digitalWrite(SPI_SS,LOW);
    uint16_t rec = SPI.transfer16(val);
    digitalWrite(SPI_SS,HIGH);
    SPI.endTransaction();
    return rec;
}
unsigned int rfm12_writee(unsigned int val) {

  unsigned int i;
  unsigned int temp = 0; // zwecks checkbit ... dann aber methodemit rückgabewert int!!!
  digitalWrite(SPI_SCK, LOW); //SCK auf 0 setzten --> nix übertragen
  digitalWrite(SPI_SS, LOW); //SS auf 0 setzten --> "Chip Select" ...Funkmodul ansprechen/selecten ...
  // 1 byte übertragen
  for (i = 0; i < 16; i++) {
    digitalWrite(SPI_SCK, LOW); //SCK auf 0 setzten --> nix übertragen
    if (val & 0x8000) { // wenn das MSB eine 1 ist ...
      digitalWrite(SPI_MOSI, HIGH); // ... 1 raus schreiben
    }
    else { // wenn MSB keine 1 ist ...
      digitalWrite(SPI_MOSI, LOW); // ... 0 raus schreiben
    }
   // delayMicroseconds(8);
    digitalWrite(SPI_SCK, HIGH); //"senden"
    val = val << 1; //val-bits um 1 nach links verschieben(damit des vergleichen mit 8000 funzt)
   // delayMicroseconds(8);
    temp = temp << 1;
    if (digitalRead(SPI_MISO) == HIGH) {
      temp |= 0x0001;
    }
  }
  digitalWrite(SPI_SCK, LOW); // nix übertragen
  digitalWrite(SPI_MOSI, LOW); // MOSI in "Ruhezustand"
  digitalWrite(SPI_SS, HIGH); // Funkmodul "disselecten"
  return (temp); //zwecks checksumme
}

void rfm12_init()
{
  rfm12_write(0x80E7); // Configuration Setting Command: EL, EF, 868MHz, 12pF
  rfm12_write(0x82D8); // Power Management Command: ER, EBB, !ET, ES, EX,EB, EB, !EW, DC
  rfm12_write(0xA640); // Frequency Setting Command: 868.0 MHz Follow Me DA 2010/11 //Wichtig! Aänder
  rfm12_write(0xC6D8); // Data Rate Command: 49,3 kbit/s
  rfm12_write(0x90A1); // Receiver Control Command: VDI output, 134kHz,LNA gain = 0dBm, RSSI = -97dBm
  rfm12_write(0xC2AC); // Data Filter Command: AL, !ML, 1, Analog RCfilter, 1, DQD = 4
  rfm12_write(0xCAF1); // 8 statt F // FIFO and Reset Mode Command: FIFOInterruptlevel,SYNC, !FF, DR
  rfm12_write(0xCED4); // Synchron pattern Command: SYNC = 2DD4
  rfm12_write(0xC483); // AFC Command: @PWR, NO RESTRICTION, !ST, !FI, OE,EN
  rfm12_write(0x9850); // TX Configuration Control Command: !MP, 90kHz, 0,0 dBm
  rfm12_write(0xCC77); // PLL Setting Command: 0, 2,5 MHz or less, 1,!DDY, DDIT, 1, BW0
  rfm12_write(0xE000); // Wake-Up Timer Command: NOT USE
  rfm12_write(0xC800); // Low Duty-Cycle Command: NOT USE
  rfm12_write(0xC049); // Low Battery Detector an Microcontroller ClockDivider Command: 1,66 MHz, 0, 3,1V
}

unsigned int rfm12_recieve() {
  unsigned int FIFO_data;
  Serial.println(rfm12_write(0x0000), HEX); // Status-Read
  FIFO_data = rfm12_write(0xB000); // FIFO Read
  return (FIFO_data & 0x00FF);
}


void rfm12_send(unsigned char aByte) {
  while (digitalRead(nIRQ) == 1); // wait for previously TX over
  rfm12_write(0xB800 + aByte);
}
static void spi_init () {
  digitalWrite(SPI_SS, 1);
  pinMode(SPI_SS, OUTPUT); //SPI_SS = Pin 10
  pinMode(SPI_MOSI, OUTPUT); //SPI_MOSI = Pin 11
  pinMode(SPI_MISO, INPUT); //SPI_MISO = Pin 12
  pinMode(SPI_SCK, OUTPUT); //SPI_SCK = Pin 13
}
