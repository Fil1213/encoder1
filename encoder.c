#include <Wire.h>

/********************************************************
**   ��� ����������� ���������� ������������ �������   **
**                    Arduino � pt2257  v4             **                  
** ��������� �������� ������������ ���������, �������� **                  
**       ������ ����������-����������� ����� "MUTE"    **
**  ���������� "Mute" ������� ��� ��������� ��������.  **   
**                        http://mynobook.blogspot.com **                
*********************************************************/
 
int Volume=40;       // ��������� ��������� ���������� �������� (50%)
int Mute=0;          // ���������� �����
int fadeAmount = 3;        // ��� ��������� ���������
unsigned long currentTime;
unsigned long loopTime;
const int pin_A = 12;       // ����������� ������ A (CLK) ��������
const int pin_B = 11;       // ����������� ������ B (DT) ��������
const int pin_SW = 9;       // ����������� ������ ������ (SW) ��������
unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_A_prev=0;

int i2c_write(int Az){ // ������� ������ ������ � pt2257 121 - MUTE ON, 120 -MUTE OFF, 0 - Volume
    Wire.beginTransmission(0x44);  // i2c ����� pt2257
if (Az == 0)   {  Wire.write(Volume/10+224);   // ���������� ������� ��������� � pt2257 �������� 224-231(00-70)
              Wire.write(Volume%10+208);}   // ���������� ������� ��������� � pt2257 �������� 208-217(0-9)
else Wire.write(Az);
    Wire.endTransmission(); 
  }
  

void setup()  {
  Wire.begin();
  pinMode(pin_SW,INPUT);  // ������������� pin pin_SW ��� ����
  digitalWrite(pin_SW,HIGH); // �������� ������ � 1         
  pinMode(pin_A, INPUT);
  pinMode(pin_B, INPUT);
  currentTime = millis();
  loopTime = currentTime;
  i2c_write(0); // ������ ��������� ��� ���������
 } 
 
void loop()  {
  currentTime = millis();
  if(currentTime >= (loopTime + 5)){ // ��������� ������ 5�� (200 ��)
    encoder_A = digitalRead(pin_A);     // ��������� ��������� ������ � �������� 
    encoder_B = digitalRead(pin_B);     // ��������� ��������� ������ B ��������    
    if((!encoder_A) && (encoder_A_prev)){    // ���� ��������� ���������� � �������������� � ����
     if (Mute == 1) {Mute=0; i2c_write(120);} // ���� ������� ����� "Mute" �� ��������� ���, ����� ������������ ���������
     else if(encoder_B) {
        // ����� � � �����. ����., ������ �������� �� ������� �������
        // ����������� ���������, �� ����� ��� �� 79 � ���������� � pt2257
        if(Volume + fadeAmount <= 79) {Volume += fadeAmount;
                                      i2c_write(0);
                                      }               
      }   
      else {
        // ����� � � 0 ����., ������ �������� ������ ������� �������     
        // ��������� �������, �� �� ���� 0 � ���������� � pt2257
        if(Volume - fadeAmount >= 0) {Volume -= fadeAmount;               
                                     i2c_write(0); 
                                     }
      }   
     }   
    encoder_A_prev = encoder_A;     // ��������� �������� � ��� ���������� ����� 
    if  (digitalRead(pin_SW) == 0) // ���� ������ ������ �� ��������� ��� �������� ���� 
    {
     if (Mute == 0) 
     {
      Mute = 1;
      while (digitalRead(pin_SW) == 0) delay(100);   //  ��������� ���������� ������
      i2c_write(121);
 
     }
     else {
      while (digitalRead(pin_SW) == 0) delay(100);  //  ��������� ���������� ������
      Mute = 0;
      i2c_write(120);
      
     }     
    }   
    loopTime = currentTime;
  }                       
}