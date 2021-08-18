//Modulo de pesagem completo
//calibra e pesa.
//com suporte até 3 moddulos e pesagem
//V1.1 Esp32
//junção de todos os modulos em funcionamento
//PINAGENS
/*

  ESP32
4
  balanças

  clock d4 GPIO 04
  1 d2 GPIO 2
  2 d15 GPIO 15
  3 d0 GPIO 0

  Sd

  CS d5 GPIO 05
  SCK d18 GPIO 018
  MOSI d23 GPIO 23
  MISO d19 GPIO 19

  Relogio

  SDA d21 GPIO 21
  SCL d22 GPIO 22

*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <DS3231.h>
#include <Wire.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "HX711.h"
#define DT 2       //d2
#define DT1 15     //d15
#define DT2 0      //d0
#define SCK 4      //d4
int tara = 254;    //tara
int massa = 0.200; //massa padrão para calibramento
HX711 balanca;     //objeto balança
HX711 balanca1;    //objeto balança1
HX711 balanca2;    //objeto balança2
RTClib Clock;      //criando objeto DS3231  (hardware)
DS3231 relogio;

const char *ssid = "Lisimetro";
const char *password = "123456789";
WiFiServer server(80);
void setup()
{


    pinMode(DT, INPUT);
    pinMode(DT1, INPUT);
    pinMode(DT2, INPUT);
    pinMode(SCK, OUTPUT);

    // inicializar(&balanca,tara,DT,SCK);
    //calibrar(&balanca,massa);

    Serial.begin(115200);
    Wire.begin();
    Ap();
    DateTime dia = Clock.now();
    defineDataHora(relogio, 18, 8, 21, 4, 13, 43, 0);

    Serial.print("data:  ");
    Serial.println(dia.day(), DEC);
    Serial.print(" / ");
    Serial.println(dia.month(), DEC);
    Serial.print(" / ");
    Serial.println(dia.year(), DEC);
    Serial.print(" / ");
    Serial.println(dia.hour(), DEC);
    Serial.print(" / ");
    Serial.println(dia.minute(), DEC);
    Serial.print(" / ");
    Serial.println(dia.second(), DEC);
    Serial.println("Hello  World");
    if (!SD.begin())
    {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/foo.txt");
    renameFile(SD, "/hello.txt", "/foo.txt");
    readFile(SD, "/foo.txt");
    testFileIO(SD, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void loop()
{

      WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
    //Serial.print(medir(balanca));
    //Serial.println(" g");
}

void Ap()
{
    Serial.println("Configuring access point...");
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.begin();
}

void defineDataHora(DS3231 dispositivo, byte day, byte month, byte year, byte dow, byte hour, byte minute, byte second)
{

    dispositivo.setClockMode(false); //define como 24h
    dispositivo.setDate(day);
    dispositivo.setMonth(month);
    dispositivo.setYear(year);
    dispositivo.setDoW(dow);
    dispositivo.setHour(hour);
    dispositivo.setMinute(minute);
    dispositivo.setSecond(second);
}

double medir(HX711 Dispositivo)
{
    return (Dispositivo.get_units(10) * 1000);
}
double medirRapido(HX711 Dispositivo)
{
    return (Dispositivo.get_units(1) * 1000);
}

void calibrar(HX711 *Dispositivo, double MassaPadrao)
{
    Serial.println("Calibrando!");
    Serial.print("Coloque a massa conhecida! de: ");
    Serial.print(MassaPadrao * 1000);
    Serial.println(" g em até 3s sobre a balança");
    delay(3000);
    Serial.println("iniciando Calibração !");
    double medida = 0;
    for (int i = 0; i < 10; i++)
    {
        medida += Dispositivo->get_value(10), 0;
        Serial.println("Executando...");
        //Serial.println(Dispositivo.get_value(10),0);
    }
    Serial.println("Finalizado");
    Dispositivo->set_scale(((medida / 10) / MassaPadrao));
}

void inicializarBalanca(HX711 *Dispositivo, float Tara, int Data, int Clock)
{
    Serial.println("Inicializando! Agurdade...");
    Serial.println("Retire qualquer peso que esteja sobre a balança!");
    delay(3000);
    Dispositivo->begin(Data, Clock);
    Serial.printf("Inicializado!");
    Dispositivo->tare(Tara);
    Dispositivo->set_scale();
}

//################################## funções do cartão SD

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char *path)
{
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path))
    {
        Serial.println("Dir created");
    }
    else
    {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char *path)
{
    Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path))
    {
        Serial.println("Dir removed");
    }
    else
    {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("File written");
    }
    else
    {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("Message appended");
    }
    else
    {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2))
    {
        Serial.println("File renamed");
    }
    else
    {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char *path)
{
    Serial.printf("Deleting file: %s\n", path);
    if (fs.remove(path))
    {
        Serial.println("File deleted");
    }
    else
    {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char *path)
{
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file)
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len)
        {
            size_t toRead = len;
            if (toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    }
    else
    {
        Serial.println("Failed to open file for reading");
    }

    file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++)
    {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

//##################################
