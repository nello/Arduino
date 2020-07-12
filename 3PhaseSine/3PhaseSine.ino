int out1 = 11;
int out2 = 10;
int out3 = 9;
const float pi = 3.1415;
float x = 0;
int enable = 8;
int freqpin = A0;
unsigned char p1 = 0;
unsigned char pp1 = 0;
unsigned char pp2 = 0;
unsigned char pp3 = 0;
unsigned char p2 = 0;
unsigned char p3 = 0;
const float y = pi/30;
const float ph1 = 2*pi/3;
const float ph2 = 4*pi/3;
const float ph3 = 2*pi;

void setup() {
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(enable, INPUT);
  pinMode(freqpin, INPUT);
}

void loop()  {
 while(digitalRead(enable) == HIGH) {
  x = x + y;
  analogWrite(out1, p1);
  analogWrite(out2, p2);
  analogWrite(out3, p3);
  pp1 = 126*sin(x+ph1);
  p1 = pp1+128;
  pp2 = 126*sin(x+ph2);
  p2 = pp2+128;
  pp3 = 126*sin(x+ph3);
  p3 = pp3+128;
  if(x >= 2*pi) x = 0;
  delay(freqpin); 
}
analogWrite(out1, 0);
analogWrite(out2, 0);
analogWrite(out3, 0);
x = 0;
}
