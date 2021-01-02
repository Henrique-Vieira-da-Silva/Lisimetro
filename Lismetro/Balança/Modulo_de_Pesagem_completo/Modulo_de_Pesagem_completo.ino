//Modulo de pesagem completo
//calibra e pesa.
//V1.0

#include "HX711.h"
#define DT 14 //d5
#define SCK 12 //d6
#define tara 254
#define massa 0.200
HX711 balanca;
 
void setup() {
  Serial.begin(9600);
  pinMode(DT, INPUT);
  pinMode(SCK, OUTPUT);
  inicializar(&balanca,tara,DT,SCK);
  calibrar(&balanca,massa);
}
 
void loop() {
  Serial.print(medir(balanca));
  Serial.println(" g");
}

double medir(HX711 Dispositivo){
  return(Dispositivo.get_units(10)*1000);
}
double medirRapido(HX711 Dispositivo){
  return(Dispositivo.get_units(1)*1000);
}

void calibrar(HX711 *Dispositivo, double MassaPadrao){
  Serial.println("Calibrando!"); 
  Serial.print("Coloque a massa conhecida! de: ");
  Serial.print(MassaPadrao*1000);
  Serial.println(" g em até 3s sobre a balança");   
  delay(3000);
  Serial.println("iniciando Calibração !"); 
  double medida =0;
  for(int i=0; i<10; i++){
  medida += Dispositivo->get_value(10),0;
  Serial.println("Executando..."); 
  //Serial.println(Dispositivo.get_value(10),0);
  }
  Serial.println("Finalizado"); 
  Dispositivo->set_scale(((medida/10)/MassaPadrao));
 
}

void inicializar(HX711 *Dispositivo, float Tara, int Data, int Clock){
  Serial.println("Inicializando! Agurdade...");
  Serial.println("Retire qualquer peso que esteja sobre a balança!");
  delay(3000);
  Dispositivo->begin(Data,Clock);
  Serial.printf("Inicializado!");
  Dispositivo->tare(Tara);
  Dispositivo->set_scale();
}
