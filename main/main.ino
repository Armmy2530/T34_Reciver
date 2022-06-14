#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>

int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
RF24 radio(4, 5); // CE, CSN

const uint64_t address = 0xF0F0F0F0E1LL;

struct Mydata
{
    float voltage;
    byte radioactive;
    byte connection;
};

Mydata adata;
int current_time = 0;
int pev_time = 0;
int timeout_time = 7000;

void setup()
{
    lcd.init();
    lcd.backlight();
 
    Serial.begin(115200);
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_250KBPS);
    radio.startListening();
    current_time = millis();

    lcd_print(0, 0, "Reciever module");
    lcd_print(4, 1, "Starto!!");
  
    delay(5000);
    lcd.clear();
}

int recvadata()
{
    if (radio.available())
    {
        radio.read(&adata, sizeof(Mydata));
        return 1;
    }
    return 0;
}

void loop()
{
    delay(50);
    current_time = millis();
    if (recvadata())
    {
        Serial.print("voltage:");
        Serial.print(adata.voltage);
        Serial.print(",");
        Serial.print("radioactive:");
        Serial.print(adata.radioactive);
        Serial.print(",");
        Serial.print("connection:");
        Serial.println(adata.connection);
        lcd.clear();
        lcd_print(0, 0, "RA sensor:" + String(adata.radioactive));
        lcd_print(0, 1, "Volatage:" + String(adata.voltage));
        pev_time = millis();
    }
    else
    {
        if (current_time - pev_time >= timeout_time)
        {
            Serial.print("voltage:-1");
            Serial.print(",");
            Serial.print("radioactive:-1");
            Serial.print(",");
            lcd.clear();
            lcd_print(0, 0, "RA sensor:-1");
            lcd_print(0, 1, "Volatage:-1");
            pev_time = millis();
        }
    }
}

void lcd_print(int colume, int row, String word_print)
{
    lcd.setCursor(colume, row);
    lcd.print(word_print);
}
