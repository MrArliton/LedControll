# LedControll
Контроллер микроконтроллера, управляющего светодиодами
// !! Важно 
После компилирования програмы на ПК обязательно указать в конфиге количество светодиодов в C:\\ConfigLed\\config.txt
Пример:
\\.\COM13
9 9 32 50,50
// Успешно работает с конфигурацией для МК приведённой ниже
// !!



// Код для МК
// Указываем, какое количество пикселей у нашей ленты.
#define LED_COUNT 32

// Указываем, к какому порту подключен вход ленты DIN.
#define LED_PIN D6

// Создаем переменную strip для управления нашей лентой.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  // Инициализируем ленту.
  strip.begin();
  Serial.begin(115200);
  delay(500);
  Serial.write("Start");
}
int* value = new int[3];
int elem;
void loop()
{
  if(Serial.available()>0){
    elem = Serial.parseInt();
    if(elem == 255){
      elem = Serial.parseInt();
    if(elem!=-1){
     value[0] = Serial.parseInt();
     if(value[0]==-1){
        return;
      }
           value[1] = Serial.parseInt();
     if(value[1]==-1){
        return;
      }
           value[2] = Serial.parseInt();
     if(value[2]==-1){
        return;
      }
      Serial.println("Yes");
    strip.setPixelColor(elem, strip.Color(value[0], value[1], value[2])); 
  strip.show();
    }
    }else if(elem == 254){
      value[0] = Serial.parseInt();
      if(value[0]==-1){
        return;
        }
              value[1] = Serial.parseInt();
      if(value[1]==-1){
        return;
        }
              value[2] = Serial.parseInt();
      if(value[2]==-1){
        return;
        }
        for(int i = 0;i<LED_COUNT;i++){
            strip.setPixelColor(i, strip.Color(value[0], value[1], value[2])); 
            strip.show();
          }
      }else if(elem == 253){
          for(int i = 0;i<LED_COUNT;i++){
            strip.setPixelColor(i, strip.Color(0,0,0)); 
            strip.show();
          }
        }
        Serial.flush();
  }

  
}
