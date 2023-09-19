//Bibliotecas e tarefas
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <FastLED.h>
TaskHandle_t tarefa3;
void loop() {}

//

// Pinos

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
int TrigPin = 26;
int EchoPin = 27;
#define DHTPIN 4
#define DHTTYPE DHT11
#define LIGHT_SENSOR_PIN 36
float _moisture, sensor_analog;
const int sensor_pin = 32;
DHT dht(DHTPIN, DHTTYPE);
#define NUM_LEDS_FITA 12
#define PINO_FITA 14
CRGB fita[NUM_LEDS_FITA];

// Variáveis de led
uint8_t hue = 10;
uint8_t palleteIndex = 0;
uint16_t breatheLevel = 0;  
byte cor = 0;
uint16_t beatA = 0;
uint16_t beatB = 0;
uint8_t sinBeat = 0;
uint8_t sinBeat2 = 0;
uint8_t sinBeat3 = 0;

// variaveis LCD

// config byte lcd
byte zero[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte one[] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};

byte two[] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};

byte three[] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};

byte four[] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};

byte five[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn)
{
  double factor = totalCount / 80.0;
  int percent = (count + 1) / factor;
  int number = percent / 5;
  int remainder = percent % 5;
  if (number > 0)
  {
    for (int j = 0; j < number; j++)
    {
      lcd.setCursor(j, lineToPrintOn);
      lcd.write(5);
    }
  }
  lcd.setCursor(number, lineToPrintOn);
  lcd.write(remainder);
  if (number < 16)
  {
    for (int j = number + 1; j <= 16; j++)
    {
      lcd.setCursor(j, lineToPrintOn);
      lcd.write(0);
    }
  }
}

//Config Perifericos
void setup() {
  Serial.begin (115200);

  pinMode (TrigPin, OUTPUT);
  pinMode (EchoPin, INPUT);
  dht.begin();
  FastLED.addLeds<WS2812B, PINO_FITA, GRB>(fita, NUM_LEDS_FITA);
  FastLED.setBrightness(200);
  xTaskCreatePinnedToCore(leitura, "tarefa3", 10000, NULL, 2, &tarefa3, 0);
  lcd.begin (16, 2);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);

}
void leitura(void *pvParameters) {
  for (;;) {
    int i = 0;
    int k = 0;
    int j = 0;//contador para iniciar a rega em café
    float distancia;
    digitalWrite (TrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite (TrigPin, LOW);
    unsigned long tempoSom = pulseIn(EchoPin, HIGH);
    distancia = tempoSom / 58;
    float t = dht.readTemperature();
    //mede luz
    int analogValue = analogRead(LIGHT_SENSOR_PIN);
    //mede umidade do solo
    sensor_analog = analogRead(sensor_pin);
    _moisture = ( 100 - ( (sensor_analog / 4095.00) * 100 ) );
    //mede volume do café
    float M , Vt, nivel, volume, raioVariavel, fator1;
    nivel = 8 - distancia;
    raioVariavel = (4.5 - (0.36 / nivel));
    fator1 = ((raioVariavel * raioVariavel) + (raioVariavel * 0.7) + 0.49);
    Vt = ((nivel * 3.1415) / 3) * fator1;
    M = (0.31 * Vt);


    /////////Testes de sensor//////////////

    Serial.print("Distancia: ");
    Serial.print(distancia);
    Serial.println(" CM");
    //Serial.print(t);
    //Serial.println("C°");
    Serial.print(analogValue);
    Serial.println("LUX");
    Serial.print("Amostra : ");
    Serial.print(sensor_pin);
    Serial.println("%");

    ////// Logica do café //////////////

    if ((analogValue <= 90) and (distancia <=3) and (i==0) and (j==0) and (k==0) ) {
      fill_solid(fita, NUM_LEDS_FITA , CRGB ::Red);
      FastLED.show();
      delay(500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Puxe");
      lcd.setCursor(0, 1);
      lcd.print("medir cafe");
      fill_solid(fita, NUM_LEDS_FITA , CRGB ::White);
      FastLED.show();
      delay(500);
    }
    if ((distancia > 4) and (analogValue <= 60)) {
      fill_solid(fita, NUM_LEDS_FITA , CRGB ::Green);
      FastLED.show();
      lcd.setCursor(0, 0);
      lcd.print("Cafe   Mg:");
      lcd.setCursor(10, 0);
      delay(2000);
      lcd.print(M);
      lcd.print("   ");
      updateProgressBar(M, 6.6, 1);
      delay(7000);
      if ((M >= 6.6) and (M <= 7.9) and (i <= 5)) {
        lcd.setCursor(0, 1);
        lcd.print("Cafe na medida");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("insira agua em:");
        lcd.setCursor(0, 16);
        lcd.print(i);
        delay(1000);
        i = i + 1;
        lcd.print(i);
        delay(1000);
        i = i + 1;
        lcd.print(i);
        delay(1000);
        i = i + 1;
        lcd.print(i);
        delay(1000);
        i = i + 1 ;
        lcd.print(i);
        delay(1000);
        i = i + 1;
        lcd.print(i);
      }
    }

    if (analogValue >= 100) {
      fill_solid(fita, NUM_LEDS_FITA , CRGB ::White);
      FastLED.show();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Insira Caneco");
      lcd.setCursor(0, 1);
      lcd.print("para iniciar");
      delay(500);
    }
    if (i == 5) {
      fill_solid(fita, NUM_LEDS_FITA , CRGB ::Blue);
      FastLED.show();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Recolha");
      lcd.setCursor(0, 1);
      lcd.print("Medir Café");
      delay(5000);
      k = 6;

    }
    if((k==6) and (distancia <=3)){
      fill_solid(fita, NUM_LEDS_FITA , CRGB ::Red);
      FastLED.show();
      delay(500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Insira 100ml");
      lcd.setCursor(0, 1);
      lcd.print("De agua");
      fill_solid(fita, NUM_LEDS_FITA , CRGB ::White);
      FastLED.show();
      delay(10000);
      j=7;
      }
    if((j == 7)) {
      fill_solid(fita, NUM_LEDS_FITA , CRGB ::Blue);
      FastLED.show();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Seu Café");
      lcd.setCursor(0, 0);
      lcd.print("Esta Pronto");
      delay(12000);
    }
    



    ////// Logica do café //////////////


  }
}
