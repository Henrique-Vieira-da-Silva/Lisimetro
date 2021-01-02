#include "HX711.h"

 
#define DT 14 //d5
#define SCK 12 //d6
 
HX711 escala;
 HX711 escala2;
float a;
 
void setup() {
  Serial.println("Inicializando! Agurdade...");
  Serial.begin(9600);
  escala.begin (DT, SCK);
  escala.read();
   escala.tare(293);
  escala.set_scale();
   a = calibrar(escala,0.200);
   escala.set_scale(a);
  Serial.println(a);

  
  
}
 
void loop() {
 
  Serial.print(medir(escala)*1000);
  Serial.println(" g");
}

double medir(HX711 Dispositivo){
  return(Dispositivo.get_units(20));
}

float calibrar(HX711 Dispositivo, double MassaPadrao){
  Serial.println("Calibrando!"); 
  Serial.print("Coloque a massa conhecida! de: ");
  Serial.print(MassaPadrao);
  Serial.println(" g em até 3s sobre a balança");   
  delay(3000);
  Serial.println("iniciando Calibração !"); 
  double medida =0;
  for(int i=0; i<10; i++){
  medida += Dispositivo.get_value(10),0;
  Serial.println("Executando..."); 
  //Serial.println(Dispositivo.get_value(10),0);
  }
  
  Serial.println("Finalizado"); 
  return((medida/10)/MassaPadrao);
}
