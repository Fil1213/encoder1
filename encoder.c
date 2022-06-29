#include <Wire.h>

/********************************************************
**   Для управлением громкостью используется энкодер   **
**                    Arduino и pt2257  v4             **                  
** Вращением энкодера регулируется громкость, нажатием **                  
**       кнопки включается-отключается режим "MUTE"    **
**  Выключение "Mute" кнопкой или поворотом энкодера.  **   
**                        http://mynobook.blogspot.com **                
*********************************************************/
 
int Volume=40;       // Начальная громкость составляет половину (50%)
int Mute=0;          // Отключение звука
int fadeAmount = 3;        // шаг изменения громкости
unsigned long currentTime;
unsigned long loopTime;
const int pin_A = 12;       // Подключение вывода A (CLK) энкодера
const int pin_B = 11;       // Подключение вывода B (DT) энкодера
const int pin_SW = 9;       // Подключение вывода кнопки (SW) энкодера
unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_A_prev=0;

int i2c_write(int Az){ // функция записи данных в pt2257 121 - MUTE ON, 120 -MUTE OFF, 0 - Volume
    Wire.beginTransmission(0x44);  // i2c адрес pt2257
if (Az == 0)   {  Wire.write(Volume/10+224);   // Отправляем десятки громкости в pt2257 значение 224-231(00-70)
              Wire.write(Volume%10+208);}   // Отправляем единицы громкости в pt2257 значение 208-217(0-9)
else Wire.write(Az);
    Wire.endTransmission(); 
  }
  

void setup()  {
  Wire.begin();
  pinMode(pin_SW,INPUT);  // устанавливаем pin pin_SW как вход
  digitalWrite(pin_SW,HIGH); // Поддяжка вывода к 1         
  pinMode(pin_A, INPUT);
  pinMode(pin_B, INPUT);
  currentTime = millis();
  loopTime = currentTime;
  i2c_write(0); // Задаем громкость при включении
 } 
 
void loop()  {
  currentTime = millis();
  if(currentTime >= (loopTime + 5)){ // проверяем каждые 5мс (200 Гц)
    encoder_A = digitalRead(pin_A);     // считываем состояние выхода А энкодера 
    encoder_B = digitalRead(pin_B);     // считываем состояние выхода B энкодера    
    if((!encoder_A) && (encoder_A_prev)){    // если состояние изменилось с положительного к нулю
     if (Mute == 1) {Mute=0; i2c_write(120);} // если включен режим "Mute" то выключить его, иначе регулировать громкость
     else if(encoder_B) {
        // выход В в полож. сост., значит вращение по часовой стрелке
        // увеличиваем громкость, не более чем до 79 и записываем в pt2257
        if(Volume + fadeAmount <= 79) {Volume += fadeAmount;
                                      i2c_write(0);
                                      }               
      }   
      else {
        // выход В в 0 сост., значит вращение против часовой стрелки     
        // уменьшаем яркость, но не ниже 0 и записываем в pt2257
        if(Volume - fadeAmount >= 0) {Volume -= fadeAmount;               
                                     i2c_write(0); 
                                     }
      }   
     }   
    encoder_A_prev = encoder_A;     // сохраняем значение А для следующего цикла 
    if  (digitalRead(pin_SW) == 0) // Если нажата кнопка то выключить или включить звук 
    {
     if (Mute == 0) 
     {
      Mute = 1;
      while (digitalRead(pin_SW) == 0) delay(100);   //  Дождаться отпускания кнопки
      i2c_write(121);
 
     }
     else {
      while (digitalRead(pin_SW) == 0) delay(100);  //  Дождаться отпускания кнопки
      Mute = 0;
      i2c_write(120);
      
     }     
    }   
    loopTime = currentTime;
  }                       
}