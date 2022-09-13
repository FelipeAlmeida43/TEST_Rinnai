#include <Arduino.h>
#define BAUD_RATE 9600
#define CTRLA 2
#define CTRLB 4
#define CTRLC 22
#define CTRLD 23
#define CFG 15
unsigned char Rbuffer[20];
unsigned char Ebuffer[20];
unsigned int index_rinnai = 0;
long int countErroSerial = 0;
long int countErroCheckSum = 0;
char hex2asc[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
char recebe_Serial(char flagCountErros)
{
  char chLido = ' ';

  unsigned char checkSum = 0;
  unsigned int Count_Out = 0;
  do
  {
    if (Serial2.available() > 0)
    {
      chLido = Serial2.read();
      Rbuffer[index_rinnai] = chLido;
      if ((index_rinnai >= 1) && (index_rinnai <= 12))
        checkSum = checkSum + chLido;
      index_rinnai++;
      Count_Out = 0;
    }
    else
    {
      Count_Out++;
      if (Count_Out > 140000)
      {
        if (flagCountErros)
          countErroSerial++;
        return 0;
      }
    }

  } while (chLido != 0x0D);

  if ((index_rinnai > 14) && (Rbuffer[13] == hex2asc[checkSum / 16]) && (Rbuffer[14] == hex2asc[checkSum % 16]))
  {
    return 1;
  }
  else // erro de CheckSum
  {
    if (flagCountErros)
      countErroCheckSum++;
    return 0;
  }
  for (int i = 0; i < 20; i++)
  {
    Serial.print(Rbuffer[i]);
  }
}
void setup()
{
  Serial.begin(BAUD_RATE);
  Serial.printf("\r\n\r\n");
  Serial2.begin(BAUD_RATE);
  pinMode(CTRLA, OUTPUT);
  pinMode(CTRLB, OUTPUT);
  pinMode(CTRLC, OUTPUT);
  pinMode(CTRLD, OUTPUT);
  pinMode(CFG, INPUT_PULLUP);
  if (!digitalRead(CFG))
  {
    digitalWrite(CTRLA, HIGH);
    digitalWrite(CTRLB, HIGH);
    digitalWrite(CTRLA, LOW);
    digitalWrite(CTRLA, LOW);
  }
  else
  {
    digitalWrite(CTRLA, LOW);
    digitalWrite(CTRLB, LOW);
    digitalWrite(CTRLA, HIGH);
    digitalWrite(CTRLA, HIGH);
  }
}

void loop()
{
  for (int i = 0; i < 4; i++)
  {
    Serial.printf("CMD INC[%u]\r\n",i);
    Serial2.write(0x02);
    Serial2.write(0x55);
    Serial2.write(0x30);
    Serial2.write(0x03);
    Serial2.write(0x38);
    Serial2.write(0x38);
    Serial2.write(0x0D);
    recebe_Serial(0);
    delay(200);
  }
  for (int i = 0; i < 4; i++)
  {
    Serial.printf("CMD DEC[%u]\r\n",i);
    Serial2.write(0x02);
    Serial2.write(0x55);
    Serial2.write(0x31);
    Serial2.write(0x03);
    Serial2.write(0x38);
    Serial2.write(0x39);
    Serial2.write(0x0D);
    recebe_Serial(0);
    delay(200);
  }
}