#include "FastLED.h"
#include <NewPing.h>

#define PIN_TRIG 12
#define PIN_ECHO 11
#define PIN 13        // пин для сведодиодной ленты
#define NUM_LEDS 57   // число диодов
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(PIN_TRIG, PIN_ECHO, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
CRGB leds[NUM_LEDS];

unsigned long next_time; // время очередного переключения первого светодиода
unsigned long timeout = 60000; // половина периода мигания
unsigned long now_time = 0;
int led_state = 0; // начальное состояние светодиода - выключен

long duration, cm, count_work, count_not_work, all;
int work_time = 0;


byte counter;


void setup() {

  // Инициализируем взаимодействие по последовательному порту

  Serial.begin (9600);
  //Определяем вводы и выводы
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(255);
  pinMode(PIN, OUTPUT);


  now_time = millis(); // текущее время
  next_time = now_time + timeout;


  for (int i = 0; i < NUM_LEDS; i++ ) {
    leds[i] = CHSV(200, 100, 255); // залить белый
  }
  FastLED.show(); // отправить на ленту

}

void loop() {

 // Каждую секунду получаем расстояние до объекта
  cm = sonar.ping_cm();
  delay(1000);

  Serial.print(", ");
  Serial.print(cm);
  if (cm < 90) {
    // Сижу за ПК
    count_work++;
    all++;
  } else {
    // У ПК нет никого
    count_not_work++;
    all++;
  }


  if (now_time > next_time) { // если текущее время превысило намеченное время, то
    next_time = now_time + timeout; // вычисляем время следующего переключения

    if (count_not_work < 25) {
      count_work = count_work + count_not_work;
      count_not_work = 0;
    }

    work_time += (count_work - (count_not_work * 3));

    Serial.println("");
    Serial.print("count_work: ");
    Serial.println(count_work);

    Serial.println("");
    Serial.print("count_not_work: ");
    Serial.println(count_not_work);

    count_work = count_not_work = 0;

    if (work_time < -100) {
      work_time = -100;
    }
    
    if (work_time > 3700) {
      work_time = 3700;
    }

    Serial.println("");
    Serial.print("work_time: ");
    Serial.println(work_time);

    all = 0;
    // В зависимости сколько времени за ПК, заливаем разный цвет
    for (int i = 0; i < NUM_LEDS; i++ ) {
      if (work_time > 2700 && work_time <= 9999999999) {
        leds[i] = CHSV(255, 255, 255);     // залить красный
        //Serial.println("красный");
      } else if (work_time > 2400 && work_time <= 2700) {
        leds[i] = CHSV(220, 255, 255);     // залить желтый
        //Serial.println("желтый");
      } else if (work_time > 1200 && work_time <= 1800) {
        leds[i] = CHSV(160, 255, 255);     // залить зеленый
        //Serial.println("зеленый");
      } else if (work_time > 300 && work_time <= 1200) {
        leds[i] = CHSV(100, 255, 255);     // залить синий
        //Serial.println("синий");
      } else if (work_time < 300) {
        leds[i] = CHSV(200, 100, 255);     // залить белый
        //Serial.println("белый");
      }
    }
    FastLED.show(); // отправить на ленту

    // Если слишком долго сидим, мигаем красно белым цветом
    if (work_time > 3600) {
      delay(500);
      for (int i = 0; i < NUM_LEDS; i++ ) {
        leds[i] = CHSV(200, 100, 255);     // залить белый
      }
      FastLED.show(); // отправить на ленту
      delay(500);
      for (int i = 0; i < NUM_LEDS; i++ ) {
        leds[i] = CHSV(255, 255, 255);     // залить красный
      }
      FastLED.show(); // отправить на ленту
      delay(500);
      for (int i = 0; i < NUM_LEDS; i++ ) {
        leds[i] = CHSV(200, 100, 255);     // залить белый
      }
      FastLED.show(); // отправить на ленту
      delay(500);
      for (int i = 0; i < NUM_LEDS; i++ ) {
        leds[i] = CHSV(255, 255, 255);     // залить красный
      }
      FastLED.show(); // отправить на ленту

    }

  }

  now_time = millis(); // текущее время
}
