#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial esp(2, 3);
Servo myservo;

#define led 9

bool acionado_ligar = false;
bool acionado_desligar = false;
bool intervalo = false;
bool agendado = false;
unsigned long int segundos = millis();
unsigned long int distancia = 0;
unsigned long int proximo = 0;
unsigned long int d_real = 0;
bool executando = false;
int timeout = 10000;
String comando_anterior = "";

void setup(){
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  myservo.attach(6);
  myservo.write(180);
  
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
    if (comando == "LIGAR" && !acionado_ligar)
    {
      digitalWrite(led, HIGH);
      myservo.write(90);
      Serial.println("LED Ligado");
      acionado_ligar = true;
      acionado_desligar = false;
    } else if (comando == "DESLIGAR" && !acionado_desligar)
    {
      digitalWrite(led, LOW);
      myservo.write(180);
      Serial.println("LED Desligado");
      acionado_desligar = true;
      acionado_ligar = false;
    }
    //Tempo Real

    //Hora Programada
    if (comando.indexOf("horario") != -1) {
      if (comando_anterior != comando){
        intervalo = false;
        distancia = comando.substring(7, comando.length()).toInt()*1000;
        if (distancia != 0) d_real = distancia;
        Serial.print("Distancia: ");
        Serial.println(d_real);
        segundos = millis();
        proximo = segundos + d_real;
        comando_anterior = comando;
        agendado = true;
      }
    } else if (comando.indexOf("intervalo") != -1)
    {
      if (comando_anterior != comando){
        intervalo = true;
        distancia = comando.substring(7, comando.length()).toInt()*1000;
        if (distancia != 0) d_real = distancia;
        Serial.print("Distancia: ");
        Serial.println(d_real);
        segundos = millis();
        proximo = segundos + d_real;
        comando_anterior = comando;
        agendado = true;
      }
    }
    
  }
  if (millis() >= proximo && d_real != 0 && !intervalo && agendado)
  {
    digitalWrite(led, HIGH);
    myservo.write(90);
    executando = true;
    agendado = false;
  }

  if (executando) {
    if (millis() > proximo + timeout) {
      digitalWrite(led, LOW);
      myservo.write(180);
      executando = false;
    }
  }
  //Hora Programada
}
