#include <Servo.h>
#include <stdbool.h>
#include <stdio.h>
#define ENA 5
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 9
#define ENB 10

int trigPin = 11;
int echoPin = 12;
int servo = 3;
bool bl_sur[2];
Servo Head;

int Q[181];  //mảng giá trị xác định thời gian quay => góc quay
unsigned int D[10];
unsigned int duration; // khoảng thời gian sóng đi và về

void setup()
{
  Serial.begin(9600);
  
  Head.attach(servo);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}
// motor1 : động cơ bên phải
// motor2 : động cơ bên trái
 
void motor_1_Dung () {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}
 
void motor_2_Dung () {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
 
void motor_1_Lui () {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 90);
}
 
void motor_1_Tien () {
  digitalWrite(IN2, HIGH);
  digitalWrite(IN1, LOW);
  analogWrite(ENA, 90);
}
 
void motor_2_Lui () {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 90);
}
 
void motor_2_Tien () {
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  analogWrite(ENB, 90);
}

// time : 10^-3 giây
// degre: {45, 90, 135, 180}

void Lui (int time) {
  motor_1_Lui (); motor_2_Lui () ;
  delay(time);
  Dung();
}
  
void Tien (int time) {
  motor_1_Tien (); motor_2_Tien () ;
  delay(time);
  Dung();
}
 
void Dung () {
  motor_1_Dung();
  motor_2_Dung();  
}

void QuayPhai (int degree) {
  motor_1_Dung (); motor_2_Tien () ;
  delay(Q[degree]);
  Dung();
}

void QuayTrai (int degree) {
  motor_1_Tien (); motor_2_Dung () ;
  delay(Q[degree]);
  Dung();
}

bool check_bl() {
	
	bool blocked;
	long D_avg, D_sum;
	
	for (int i=0; i<10; i++) 
	  { 
	  //tạo sóng
		digitalWrite(trigPin, LOW);delayMicroseconds(2);
		digitalWrite(trigPin, HIGH);delayMicroseconds(10);
		digitalWrite(trigPin, LOW);

		//nhận giá trị thời gian
		duration = pulseIn(echoPin, HIGH);
		//tính khoảng cách
		int distance = (duration / 2 / 29.412) ;
		//trả về 
		D[i] = distance;
		delay(0);
	  } 
	
	D_avg = 0; D_sum = 0;
	
	for (int i=0; i<10; i++) 
	{
		D_sum = D_sum + D[i] ;
	}
	
	D_avg = D_sum / 10;

	if (D_avg < 25) 
		blocked = true;
	else 
		blocked = false;
	
	return blocked;
}

void ctrl_servo (int deg) { 
	switch (deg) {
		case 0: 
		Head.write(0); //phai
		break;
		case 90: 
		Head.write(90); //truoc
		break;
		case 180:
		Head.write(180); //trai
		break;
	}
}

void check_bl_sur () {
	bl_sur[0] = false;
	bl_sur[1] = false;
	//check ben trái
	ctrl_servo(180);
	bl_sur[0] = check_bl();
	//check bên phải
	ctrl_servo(0);
	bl_sur[1] = check_bl();
	
	ctrl_servo(90);
}

void define_degree() { // cần qua thực nghiệm để hoàn thiện mảng này
  Q[45] = 0;
  Q[90] = 1000;
  Q[135]= 0;
  Q[180]= 0;
}

void start() {
	
	ctrl_servo(90);
	
	while (!check_bl())
	{
	Tien(2000);
	}
	
	Dung();
	
	check_bl_sur();
	
	/*if ((bl_sur[0] == true) && (bl_sur[1] == false)) 
	{
		QuayPhai(90);
	}
	if ((bl_sur[0] == false) && (bl_sur[1] == true)) 
	{
		QuayTrai(90);
	}*/
  
}
void loop() {
//define_degree();

// Head.write(180);delay(1000);
// Head.write(0);delay(1000);
    
// start(); 
}
