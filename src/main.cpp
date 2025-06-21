#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial esp(2, 3);
Servo myservo;

#define led 9

String response = "";
bool intervalo = false;
unsigned long int segundos = millis();
int distancia = 0;
int proximo = 0;
bool executando = false;
int timeout = 1500;

void setup(){
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  myservo.attach(6);
  myservo.write(0);

  Serial.begin(9600);
  esp.begin(9600);

  delay(100);
}

void loop(){
  if(esp.available()){
    String comando = esp.readStringUntil('\n');
    comando.trim();

    Serial.print("Recebido: ");
    Serial.println(comando);

    //Tempo Real
    if (comando == "LIGAR")
    {
      digitalWrite(led, HIGH);
      myservo.write(180);
      Serial.println("LED Ligado");
    } else if (comando == "DESLIGAR" && myservo.read() > 15)
    {
      digitalWrite(led, LOW);
      myservo.write(0);
      Serial.println("LED Desligado");
    }
    //Tempo Real

    //Hora Programada
    if (comando.indexOf("horario") != -1) {
      intervalo = false;
      distancia = comando.substring(7, comando.length()).toInt()*1000;
      segundos = millis();
      proximo = segundos + distancia;
    }
  }

  if (millis() == proximo)
  {
    digitalWrite(led, HIGH);
    myservo.write(180);
    executando = true;
  }

  if (executando) {
    if (millis() > proximo + timeout) {
      digitalWrite(led, LOW);
      myservo.write(0);
      executando = false;
    }
  }
  
}
