//Modulo de calibração


#include "HX711.h"
 
#define DT 14
#define SCK 12
 
HX711 escala;    // Relaciona a variável escala
 
void setup() {
  Serial.println("Aguarde!!");
  escala.begin (DT, SCK);
  Serial.begin(9600);
  Serial.print("Leitura da Tara:  ");
  Serial.println(escala.read());  // Aguada o termino de verificação do peso
  Serial.println("Aguarde!");
  Serial.println("Iniciando ...");
  escala.set_scale();             // Utiliza uma escala padrão de verificação
  escala.tare(293);                // Fixa o peso como tara
  Serial.println("Medindo valor de calibração, isso pode levar 10s ou mais...");
 
  Serial.print("Média: " );
  Serial.println(calibrar(escala,0.2));
  
}
 
void loop() {
 
}

double calibrar(HX711 Dispositivo, double MassaPadrao){
  Serial.println("Insira a massa conhecida!"); 
  delay(3000);
  Serial.println("iniciando Calibração !"); 
  double medida =0;
  for(int i=0; i<10; i++){
  medida += Dispositivo.get_value(10),0;
  Serial.println("Executando..."); 
  //Serial.println(Dispositivo.get_value(10),0);
  }
  Serial.println("Finalizado"); 
  return(medida/10/MassaPadrao);
}
