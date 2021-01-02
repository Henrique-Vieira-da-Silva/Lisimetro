//Modulo de pesagem completo
//calibra e pesa.


#include "HX711.h"
#define DT 14 //d5
#define SCK 12 //d6
#define tara 254
#define massa 0.200
 
HX711 escala;

 
void setup() {
  Serial.begin(9600);
  while(!Serial);
  escala = inicializar(escala,tara,DT,SCK);
  escala = (calibrar(escala,massa));
 

  
  
}
 
void loop() {
 
  Serial.print(medir(escala));
  Serial.println(" g");
}

double medir(HX711 Dispositivo){
  return(Dispositivo.get_units(10)*1000);
}
double medirRapido(HX711 Dispositivo){
  return(Dispositivo.get_units(1)*1000);
}

HX711 calibrar(HX711 Dispositivo, double MassaPadrao){
  Serial.println("Calibrando!"); 
  Serial.print("Coloque a massa conhecida! de: ");
  Serial.print(MassaPadrao*1000);
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
  Dispositivo.set_scale(((medida/10)/MassaPadrao));
  return Dispositivo ;
}

HX711 inicializar(HX711 Dispositivo, float Tara, int Data, int Clock){
  Serial.println("Inicializando! Agurdade...");
  Serial.println("Retire qualquer peso que esteja sobre a balança!");
  delay(3000);
  Dispositivo.begin(Data,Clock);
  Dispositivo.read();
  Serial.printf("Inicializado!");
  Dispositivo.tare(Tara);
  Dispositivo.set_scale();
  return Dispositivo;
}
