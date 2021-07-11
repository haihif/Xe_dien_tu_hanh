#include <Servo.h>
#include <stdbool.h>
#include <stdio.h>
#define ENA 5
#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 4
#define ENB 3

#define ir1 A3
#define ir2 A4
#define ir3 A5

int trigPin = 11;
int echoPin = 12;
bool left, right, front;
Servo Head;

int s2;  //Left Sensor
int s3;  //Middle Sensor
int s4;  //Right Sensor

int Q[181];  //mảng giá trị xác định thời gian quay => góc quay
unsigned int D[10];

void setup() {
  
//  BUG
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(ir3, INPUT);

  Head.attach(9);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}




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
  motor_1_Tien ();
  motor_2_Tien () ;
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

void Qdegree () {
  Q[30] = 500;
  Q[45] = 750;
  Q[90] = 1500;  
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
    int duration = pulseIn(echoPin, HIGH);
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

void check_bl_sur () {
  front = false;
  left = false;
  right = false;
  //check ben trái
  ctrl_servo(180);
  left = check_bl();
  delay(2000);
  //check bên phải
  ctrl_servo(0);
  right = check_bl();
  delay(2000);
  //  check đằng trước
  ctrl_servo(90);
  front = check_bl();
}

void getdoline () {
  s2 = analogRead(ir1);  //Left Sensor
  s3 = analogRead(ir2);  //Middle Sensor
  s4 = analogRead(ir3);  //Right Sensor
}


void loop() {
  Qdegree();
  
//  cố định cảm biến siêu âm ở đằng trước
  ctrl_servo(90);
  getdoline();
  
//  Trường hợp không có vật cản
  if (check_bl() == false) {
    getdoline();
    
//  Led giữa nhận tín hiệu
    if (s3 < 40 && s2 >= 40 && s4 >= 40) {
      Tien(700);
    }
    
//  Led trái nhận tín hiệu
    else if (s2 < 40 && s3 >= 40 && s4 >= 40) {
      do { QuayTrai(30); 
          getdoline();
      } while (s3 < 40);
    } 
    
//    Led phải nhận tín hiệu
    else if (s4 < 40 && s3 >= 40 && s2 >= 40) {
      do { QuayPhai(30); 
          getdoline();
      } while (s3 < 40);
    } 
    
//    Led trái và giữa nhận tín hiệu
    else if (s2 < 40 && s3 < 40 && s4 >= 40) {
        do { QuayTrai(45); 
          getdoline();
      } while (s3 < 40 && s2 >= 40);
    }
    
//    Led phải và giữa nhận tín hiệu
    else if (s4 < 40 && s3 < 40 && s2 >= 40) {
        do { QuayPhai(45); 
          getdoline();
      } while (s3 < 40 && s4 >= 40);
    }

//    Cả 3 led nhận tín hiệu
    else if (s2 < 40 && s3 < 40 && s4 < 40) {
      check_bl_sur();
      
//    2 bên không có vật cản
      if (right == false && left == false) {
          do { QuayTrai(45); 
          getdoline();
      } while (s3 < 40 && s2 >= 40);
      }
      
//    2 bên có vật cản 
      else if (front == false && left == true && right == true) {
        Tien(700);  
      }
      
//    chỉ bên phải có vật cản
      else if (right == true && left == false) {
          do { QuayTrai(45); 
          getdoline();
      } while (s3 < 40 && s2 >= 40);
      }
      
//    chỉ bên trái có vật cản
      else if (right == false && left == true) {
          do { QuayPhai(45); 
          getdoline();
      } while (s3 < 40 && s4 >= 40);
      }
      
//    cả 3 hướng có vật cản 
      else if (front == true && right == true && left == true) {
        Dung();
          do { 
          delay(1000);
          check_bl_sur();
        } while (right == false || left == false || front == false); 
      }
    }




//  cả 3 led không nhận tín hiệu
    else if (s2 >= 40 && s3 >= 40 && s4 >= 40) {
      check_bl_sur();
      
//    2 bên không có vật cản
      if (right == false && left == false) {
          QuayTrai(90);
      }
      
//    2 bên có vật cản 
      else if (front == false && left == true && right == true) {
        Tien(700);  
      }
      
//    chỉ bên phải có vật cản
      else if (right == true && left == false) {
          QuayTrai(90); 
      }
      
//    chỉ bên trái có vật cản
      else if (right == false && left == true) {
          QuayPhai(90); 
      }
      
//    cả 3 hướng có vật cản
      else if (front == true && right == true && left == true) {
        Dung();
          do { 
          delay(1000);
          check_bl_sur();
        } while (right == false || left == false || front == false); 
      }
    }
   }else // Phia truoc co vat can
	   {
	   //    2 bên không có vật cản
		if (right == false && left == false) {
			  QuayTrai(90);
		}
		//    2 bên có vật cản 
      else if (front == false && left == true && right == true) {
        Tien(700);  
      }
      
//    chỉ bên phải có vật cản
      else if (right == true && left == false) {
          QuayTrai(90); 
      }
      
//    chỉ bên trái có vật cản
      else if (right == false && left == true) {
          QuayPhai(90); 
      }
      
//    cả 3 hướng có vật cản
      else if (front == true && right == true && left == true) {
        Dung();
          do { 
          delay(1000);
          check_bl_sur();
        } while (right == false || left == false || front == false); 
      }
   }
	   
}
