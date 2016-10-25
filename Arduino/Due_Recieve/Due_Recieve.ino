int SPI_SS = 31;//53
int SPI_MOSI = 35;//51
int SPI_MISO = 29;//50
int SPI_SCK = 33;//52
int nIRQ = 27;
int TXEN = 23; //??
int RXEN = 25; //47
void setup() {
  Serial.begin(115200);
  pinMode(nIRQ, INPUT);
  // Pins für TXEN und RXEN
  pinMode(TXEN, OUTPUT);
  pinMode(RXEN, OUTPUT);
  // und gleich einmal definieren
  digitalWrite(TXEN, LOW);
  digitalWrite(RXEN, LOW);
  delay(3000);
  spi_init();
  // Funkmodul initialisieren
  rfm12_init();
  digitalWrite(RXEN, HIGH);
  pinMode(6,OUTPUT);
  rfm12_write(0xCA81);
  empfang_akt();
  Serial.println("Start");
  pinMode(2,INPUT);
  pinMode(13,OUTPUT);
  attachInterrupt(2,nix,RISING);
  attachInterrupt(nIRQ,empfang,FALLING);
}
void empfang()
{
  unsigned int r = rfm12_recieve();
  Serial.println(r,HEX);
  rfm12_write(0xCA81);
  digitalWrite(RXEN, LOW);
  empfang_akt();
}
void nix()
{
  digitalWrite(13,!digitalRead(13));
}
unsigned int FIFO_data;
unsigned int deltaMillis;
void loop() {
  /*empfang_akt();
  unsigned int r = rfm12_recieve();
  Serial.println(r,HEX);
  rfm12_write(0xCA81);
  digitalWrite(RXEN, LOW);*/
  /*unsigned int reg;
    //TXEN auf 1 und gleichzeitig RXEN auf 0 ... damit Übertragung"genehmigt"
    digitalWrite(TXEN, HIGH);
    digitalWrite(RXEN, LOW);
    reg = rfm12_write(0x0000); //Status Register lesen
    //Serial.println(reg, HEX);
    rfm12_write(0x823D); // Power Management Register neu senden
    // Preamble senden:
    rfm12_send(0xAA);
    rfm12_send(0xAA);
    rfm12_send(0xAA);
    // Synchronisieren:
    rfm12_send(0x2D); // SYNC HIGH Byte
    rfm12_send(0xD4); // SYNC LOW Byte
    // Daten senden:
    rfm12_send(0x66);
    //Dummy Bytes:
    rfm12_send(0xAA);
    rfm12_send(0xAA);
    rfm12_send(0xAA);
    //Übertragung fretig -> also TXEN wieder auf 0
    digitalWrite(TXEN, LOW);
    Serial.println("Sent");
    Serial.println(reg,HEX);
    delay(1000);*/
}
void empfang_akt() {
  digitalWrite(RXEN, HIGH);
  //Serial.println(FIFO_data & 0x00FF,HEX);
  //FIFO enablen
  rfm12_write(0xCAF3); // F wegen höherem Interrupt-Level
}

unsigned int rfm12_write(unsigned int val) {

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
    //delayMicroseconds(1);
    digitalWrite(SPI_SCK, HIGH); //"senden"
    val = val << 1; //val-bits um 1 nach links verschieben (damit des vergleichen mit 8000 funzt)
    //delayMicroseconds(1);
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
  rfm12_write(0x80E7); // Configuration Setting Command: EL, EF, 868MHz,12pF
  rfm12_write(0x82D8); // Power Management Command: ER, EBB, !ET, ES, EX,EB, EB, !EW, DC
  rfm12_write(0xA640); // Frequency Setting Command: 868.0 MHz
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
  rfm12_write(0xC049); // Low Battery
}

unsigned int rfm12_recieve() {
  unsigned int FIFO_data;
  //while (digitalRead(nIRQ) == 1) {
    //Serial.println("while");
 // }
  rfm12_write(0x0000); // Status-Read
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
