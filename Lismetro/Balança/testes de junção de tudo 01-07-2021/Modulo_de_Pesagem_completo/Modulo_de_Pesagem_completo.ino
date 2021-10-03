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
#include <DS3231.h>
#define DT 2         //d2
#define DT1 15       //d15
#define DT2 0        //d0
#define SCK 4        //d4
float tara = 254;    //tara
float massa = 1.000; //massa padrão para calibramento
HX711 balanca;       //objeto balança
HX711 balanca1;      //objeto balança1
HX711 balanca2;      //objeto balança2
RTClib Clock;        //criando objeto DS3231  (hardware)
DS3231 relogio;
const char *ssid = "Lisimetro";     //ssdi do modo AP
const char *password = "123456789"; //senha do modo AP
WiFiServer server(80);              //porta do servidor

double tara1=0, tara2=0, tara3=0;

/*
class medicao{
/*

- numero da balança byte
- tara atual double 
- escala/calibração double
- data string
- hora string
- massa double
- dados brutos da balança double
- data da tara string
- hora da tara string
- nome do intervalo (20 caracteres) string
- media aritimética intervalo atual double


    public:
        byte n;//numero da balança
        double  tara, escala, dbrutos, media;
        String data, hora, dataT, horaT, nome;

};

*/

class Configs
{

public:
    int httpPort;
    String host;
    int tempo;
};

class URL
{

public:
    String url;
    String rota;
    URL(String url)
    {
        this->url = url;
        GetRota();
    }

    String GetAtr(String atr)
    {
        String url = this->url;

        if (url.indexOf(atr) == -1)
            return (" ");
        url = url.substring(url.indexOf("?" + atr + "=") + 2 + atr.length(), url.length());

        if (url.lastIndexOf("?") != -1)
        {
            return (url.substring(0, url.indexOf("?")));
        }
        else
        {
            return (url.substring(0, url.indexOf("HTTP")));
        }
    }

    void GetRota()
    {
        this->url = this->url.substring(this->url.lastIndexOf("GET /") + 5, this->url.substring(this->url.lastIndexOf("GET /"), this->url.length()).lastIndexOf("HTTP") + 4);
        if (this->url.lastIndexOf("?") != -1)
        {
            this->rota = this->url.substring(0, this->url.indexOf("?"));
        }
        else
        {
            this->rota = this->url.substring(0, this->url.lastIndexOf("HTTP"));
        }
        this->rota.trim();
    }
};

Configs dadosConfig;
void setup()
{
    Ap();
    pinMode(DT, INPUT);
    pinMode(DT1, INPUT);
    pinMode(DT2, INPUT);
    pinMode(SCK, OUTPUT);
    Serial.begin(115200);
    Wire.begin();
    wifiAuto();
    configAuto();
    balancasAuto();
    Serial.println("Dados da configuração: ");
    Serial.println(dadosConfig.host);
    Serial.println(dadosConfig.httpPort);
    VerificaUpdates();
    int teste3 = 3000;
    const char *teste = "192.168.2.7";
    
    Serial.println("Hora: ");
    RTClib myRTC;

    DateTime now = myRTC.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    //Cliente(teste3, teste, "/configHorario");
    //mkdirs("x/b/c/d/ola");
    // inicializar(&balanca,tara,DT,SCK);
    //calibrar(&balanca,massa);
}
void loop()
{

    WiFiClient client = server.available(); // listen for incoming clients

    if (client)
    {                            // if you get a client,
        String currentLine = ""; // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.print(c);
                currentLine += c;

                if (currentLine.lastIndexOf("HTTP") != -1)
                {
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println("<meta charset='UTF-8'>");
                    client.println();

                    URL req(currentLine);
                    req.url.replace("&", "?");
                    if (req.rota == "teste")
                    {
                        client.println("<h1>Ola</h1>");
                        break;
                    }
                    else if (req.rota == "setwifi")
                    {
                        client.println("<h2>Conectado...</h2>");
                        if (WfConnection(tochar(req.GetAtr("ssid")), tochar(req.GetAtr("senha")), converte(req, "ip"), converte(req, "gateway"), converte(req, "mask"), converte(req, "dns")))
                        {
                            client.println("<h1>Conectado!</h1> <br>");
                            if (salvaWifi(req))
                            {
                                client.println("<h1>Salvo!</h1>");
                            }
                            else
                            {
                                client.println("<h1>Erro ao Salvar!</h1>");
                            }
                            delay(500);
                            break;
                        }
                        else
                        {
                            client.println("<h1>Erro, Verifique os dados!</h1>");
                            delay(500);
                            client.println("<script>setTimeout(()=>{window.location.href='wifi'}, 2000);</script>");

                            break;
                        }
                    }
                    else if (req.rota == "wifi")
                    {
                        client.println("<html lang='pt-br'> <meta charset='UTF-8'><title>Configurações de WiFi</title>");
                        client.println("<a href='/'>Menu</a> <br/>");
                        client.println("<form style='text-align: center;' action='/setwifi' method='get'> ");
                        client.println("<h1>insira os dados e envie para configurar em qual rede o Lisimetro deve se conectar!</h1><br/>");
                        client.println("<h2>preencha todos os campos corretamente</h2><br/><br>");
                        client.println("<input type='text' name='ssid' placeholder='SSID (nome da rede WIFI)'><br><br>");
                        client.println("<input type='password' name='senha' placeholder='senha'><br><br>");
                        client.println("<input type='text' name='ip' placeholder='ip(192168001050)'><br><br>");
                        client.println("<input type='text' name='gateway' placeholder='gateway(192168001001)'><br><br>");
                        client.println("<input type='text' name='mask' placeholder='mask(255255255000)'><br><br>");
                        client.println("<input type='text' name='dns' placeholder='dns(008008008008)'><br><br>");
                        client.println(" <input type='submit' value='Enviar'>");
                        client.println("</form>");
                        break;
                    }
                    else if (req.rota == "configBalancas") //##############################
                    {
                        client.println("<html lang='pt-br'> <meta charset='UTF-8'><title>Configurações das Células de Carga</title>");
                        client.println("<a href='/'>Menu</a> <br/>");
                        client.println("<form style='text-align: center;' action='/setBalancas' method='get'> ");
                        client.println("<h2>preencha todos os campos corretamente</h2><br/><br>");
                        client.println("<input type='text' name='tara1' placeholder='tara 1'><br><br>");
                        client.println("<input type='text' name='tara2' placeholder='tara 2'><br><br>");
                        client.println("<input type='text' name='tara3' placeholder='tara 3'><br><br>");
                        client.println(" <input type='submit' value='Salvar'>");
                        client.println("</form>");
                        break;
                    }
                    else if (req.rota == "config")
                    {
                        client.println("<html lang='pt-br'> <meta charset='UTF-8'><title>Configurações</title>");
                        client.println("<a href='/'>Menu</a> <br/>");
                        client.println("<form style='text-align: center;' action='/setConfig' method='get'> ");
                        client.println("<h1>insira os dados e envie para configurar o Lisimetro</h1><br/>");
                        client.println("<h2>preencha todos os campos corretamente</h2><br/><br>");
                        client.println("<input type='text' name='host' placeholder='ip Servidor: 192.168.2.9'><br><br>");
                        client.println("<input type='text' name='porta' placeholder='porta: 3000'><br><br>");
                        client.println("<input type='text' name='tempo' placeholder='tempo em minutos de leitura'><br><br>");
                        client.println(" <input type='submit' value='Enviar'>");
                        client.println("</form>");
                        break;
                    }
                    else if (req.rota == "inicio")
                    {
                        switch (appendFile(req.GetAtr("pasta"), "Hello World"))
                        {
                        case -1:
                            client.println("<h1>Cartão Falhado</h1>");

                            break;
                        case -2:
                            client.println("<h1>cheio</h1>");

                            break;
                        case 1:
                            client.println("<h1>já criado</h1>");

                            break;
                        case 0:
                            client.println("<h1>sucesso</h1>");

                            break;
                        default:
                            break;
                        }
                        client.println(readFile(req.GetAtr("pasta")));
                    }
                    else if (req.rota == "setConfig")
                    {
                        if (salvaConfig(req))
                        {
                            client.println("Configurado!");
                        }
                        else
                        {
                            client.println("Erro, tente novamente!");
                            delay(500);
                            client.println("<script>setTimeout(()=>{window.location.href='config'}, 2000);</script>");
                        }
                    }
                    else if (req.rota == "setBalancas") //################
                    {
                        if (salvaConfigBalancas(req))
                        {
                            client.println("Configurado!");
                        }
                        else
                        {
                            client.println("Erro, tente novamente!");
                            delay(500);
                            client.println("<script>setTimeout(()=>{window.location.href='configBalancas'}, 2000);</script>");
                        }
                    }
                    else if (req.rota == "file/list")
                    {
                        client.println("<html lang='pt-br'><tiyle>Listar Arquivos</title> <meta charset='UTF-8'>");
                        client.println("<a href='/'>Menu</a> <br/>");
                        if (!SD.begin())
                        {
                            client.println("Cartão não conectado ou falha");
                            return;
                        }

                        //listDir(SD, "/", 0);

                        String dirname = "/";

                        client.printf("Listing directory: %s\n", dirname);

                        File root = SD.open(dirname);
                        if (!root)
                        {
                            client.println(" <br/> Failed to open directory <br/> ");
                            return;
                        }
                        if (!root.isDirectory())
                        {
                            client.println(" <br/> Not a directory <br/> ");
                            return;
                        }

                        File file = root.openNextFile();
                        while (file)
                        {
                            if (file.isDirectory())
                            {
                                client.print("<br/>  DIR : ");
                                client.println(file.name());
                            }
                            else
                            {
                                client.print("<br/>   FILE: ");
                                client.print(file.name());
                                client.print(" | ");
                                client.print(file.size());
                                client.print(" B");
                            }
                            file = root.openNextFile();
                        }
                    }
                    else if (req.url == " HTTP")
                    {
                        client.println("<html lang='pt-br'><meta charset='UTF-8'><title>Menu</title>");

                        client.println("<h1>MENU</h1><br/><hr>");
                        client.println("<a href='wifi'>Configurações de WiFi</a> <br/>");
                        client.println("<a href='config'>Configurações de servidor e outros</a> <br/>");
                        client.println("<a href='file/list'>Listar Arquivos Do cartão</a> <br/>");
                        client.println("");
                    }
                    else
                    {
                        client.println("<html lang='pt-br'> <meta charset='UTF-8'>");
                        client.println("<a href='/'>Menu</a> <br/>");
                        String arquivo = "/";
                        arquivo += req.url.substring(req.url.indexOf("/file/") + 6, req.url.indexOf("HTTP"));
                        Serial.println("\n");
                        Serial.println(arquivo);

                        if (!SD.begin())
                        {
                            Serial.println("Cartão não conectado ou falha");
                            return;
                        }
                        File file = SD.open(arquivo);
                        if (!file)
                        {
                            client.print("404");
                        }
                        while (file.available())
                        {
                            client.print(char(file.read()));
                        }
                    }
                    break;
                }
            }
        }

        // close the connection:
        client.stop();
        Serial.println("Client Disconnected.");
    }
}

const char *tochar(String string)
{
    const char *a = string.c_str();
    return (a);
}
bool wifiAuto()
{
    if (SD.begin())
    {
        if (SD.exists("/lisimetro/config/rede/config.txt"))
        {
            URL req(" ");

            File file = SD.open("/lisimetro/config/rede/config.txt");
            if (!file)
            {
                Serial.print("Arquivo não encontrado");
            }
            while (file.available())
            {
                req.url += char(file.read());
            }

            if (WfConnection(tochar(req.GetAtr("ssid")), tochar(req.GetAtr("senha")), converte(req, "ip"), converte(req, "gateway"), converte(req, "mask"), converte(req, "dns")))
                return true;
        }
    }
    return false;
}
bool configAuto()
{
    if (SD.begin())
    {
        if (SD.exists("/lisimetro/config/gerais/config.txt"))
        {
            URL req(" ");

            File file = SD.open("/lisimetro/config/gerais/config.txt");
            if (!file)
            {
                Serial.print("Arquivo não encontrado");
            }
            while (file.available())
            {
                req.url += char(file.read());
            }
            dadosConfig.httpPort = req.GetAtr("porta").toInt();
            dadosConfig.host = req.GetAtr("host");
            dadosConfig.tempo = req.GetAtr("tempo").toInt();
        }
    }
    return false;
}

bool balancasAuto()
{
    if (SD.begin())
    {
        if (SD.exists("/lisimetro/config/gerais/configBalanca.txt"))
        {
            URL req(" ");

            File file = SD.open("/lisimetro/config/gerais/configBalanca.txt");
            if (!file)
            {
                Serial.print("Arquivo não encontrado");
            }
            while (file.available())
            {
                req.url += char(file.read());
            }
            tara1 = req.GetAtr("tara1").toDouble();
            tara2 = req.GetAtr("tara2").toDouble();
            tara3 = req.GetAtr("tara3").toDouble();
        }
    }
    return false;
}

bool salvaWifi(URL req)
{

    if (!SD.begin())
    {
        Serial.println("Card Mount Failed");
        return (false);
    }

    removeFile("lisimetro/config/rede/config.txt");

    if (SD.begin())
    {

        String conteudo = "GET /setwifi?ip=" + req.GetAtr("ip");
        conteudo += "?gateway=" + req.GetAtr("gateway");
        conteudo += "?mask=" + req.GetAtr("mask");
        conteudo += "?dns=" + req.GetAtr("dns");
        conteudo += "?ssid=" + req.GetAtr("ssid");
        conteudo += "?senha=" + req.GetAtr("senha");
        conteudo += "HTTP";

        if (!mkdirs("lisimetro/config/rede"))
            return (false);

        if (!createFile("/lisimetro/config/rede/config.txt", conteudo))
        {
            Serial.println("certo");
            return (true);
        }
        else
        {
            Serial.println("falhou");
            return (false);
        }
    }
    else
    {
        return (false);
    }
}
IPAddress converte(URL teste, String atr)
{
    IPAddress a(teste.GetAtr(atr).substring(0, 3).toInt(), teste.GetAtr(atr).substring(3, 6).toInt(), teste.GetAtr(atr).substring(6, 9).toInt(), teste.GetAtr(atr).substring(9, 12).toInt());
    return (a);
}
//salvaConfigBalancas
bool salvaConfigBalancas(URL req)
{

    if (!SD.begin())
    {
        Serial.println("Card Mount Failed");
        return (false);
    }

    removeFile("lisimetro/config/gerais/configBalanca.txt");

    if (SD.begin())
    {

        String conteudo = "GET /configBalancas";
        conteudo += "?tara1=" + req.GetAtr("tara1");
        conteudo += "?tara2=" + req.GetAtr("tara2");
        conteudo += "?tara3=" + req.GetAtr("tara3"); //em minutos
        conteudo += "HTTP";

        if (!mkdirs("lisimetro/config/gerais"))
            return (false);

        if (!createFile("/lisimetro/config/gerais/configBalanca.txt", conteudo))
        {
            // Serial.println("certo");
            return (true);
        }
        else
        {
            // Serial.println("falhou");
            return (false);
        }
    }
    else
    {
        return (false);
    }
}

bool salvaConfig(URL req)
{

    if (!SD.begin())
    {
        Serial.println("Card Mount Failed");
        return (false);
    }

    removeFile("lisimetro/config/gerais/config.txt");

    if (SD.begin())
    {

        String conteudo = "GET /config";
        conteudo += "?host=" + req.GetAtr("host");
        conteudo += "?porta=" + req.GetAtr("porta");
        conteudo += "?tempo=" + req.GetAtr("tempo"); //em minutos
        conteudo += "HTTP";

        if (!mkdirs("lisimetro/config/gerais"))
            return (false);

        if (!createFile("/lisimetro/config/gerais/config.txt", conteudo))
        {
            // Serial.println("certo");
            return (true);
        }
        else
        {
            // Serial.println("falhou");
            return (false);
        }
    }
    else
    {
        return (false);
    }
}

bool mkdirs(String pastas)
{

    //crias varias pastas, sua sintaxe é asism: mkdirs("a/b/c/d");
    String pasta, ultima = "";

    do
    {
        if (pastas.lastIndexOf("/") >= 0)
        {
            pasta = pastas.substring(0, pastas.indexOf("/"));
            pastas = pastas.substring(pastas.indexOf("/") + 1);
        }
        else
        {
            pasta = pastas;
        }

        if (!SD.begin())
            return (false);
        // Serial.println("pasta: " + pasta);
        if (ultima == "")
        {
            if (!SD.exists("/" + pasta))
            {
                if (!SD.mkdir("/" + pasta))
                    return (false);
            }
        }
        else
        {
            if (!SD.exists(ultima + "/" + pasta))
            {
                if (!SD.mkdir(ultima + "/" + pasta))
                    return (false);
            }
        }
        ultima += "/" + pasta;
    } while (pastas != pasta);
    return (true);
}

void Ap()
{
    Serial.println("Configuring access point...");
    WiFi.softAP(ssid, password);
    IPAddress Ip(192, 168, 4, 50);
    IPAddress gatway(192, 168, 4, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(Ip, gatway, NMask);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.begin();
}

bool WfConnection(const char *ss, const char *s, IPAddress Ip, IPAddress gatway, IPAddress NMask, IPAddress dns)
{

    Serial.println(Ip);
    Serial.println(gatway);
    Serial.println(NMask);
    Serial.println(dns);
    Serial.println(ss);
    Serial.println(s);
    WiFi.config(Ip, gatway, NMask, dns, dns);
    WiFi.begin(ss, s);
    int count = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        count++;
        if (count > 25)
            return (false);
    }
    return (true);
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
    Serial.println(dispositivo.getDate());
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

int appendFile(String arquivo, String texto)
{
    Serial.println("olha so: " + arquivo);

    if (!SD.begin())
    {
        return -1; //cartão não disponivel
    }

    if (SD.exists(arquivo))
    {
        File file = SD.open(arquivo, FILE_APPEND);
        if (file.println(texto))
        {
            file.close();
            return 0; //aquivoCriado
        }
        else
        {
            file.close();
            return -2; //falha ao criar (memoria cheia ou caminha errado)
        }
    }
    else
    {

        return -2; //já criada
    }
}

int removeFile(const char *arquivo)
{
    if (SD.remove(arquivo))
    {
        return (0);
    }
    else
    {
        return (-1);
    }
}

int createFile(const char *arquivo, String texto)
{
    File file = SD.open(arquivo, FILE_WRITE);
    if (file.println(texto))
    {
        file.close();
        return 0; //aquivoCriado
    }
    else
    {
        return -2; //falha ao criar (memoria cheia ou caminha errado)
    }
}

int createDir(const char *pasta)
{
    if (SD.mkdir(pasta))
    {
        return 0; //pastaCriada
    }
    else
    {
        return -2; //falha ao criar (memoria cheia ou caminha errado)
    }
}

String readFile(String arquivo)
{

    File file = SD.open(arquivo);
    if (!file)
    {
        return "-1";
    }

    arquivo = "";
    while (file.available())
    {
        arquivo += char(file.read());
    }

    file.close();
    return arquivo;
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
String Cliente(const int httpPort, const char *host, String url)
{
    Serial.print("connecting to");
    Serial.println(host);
    WiFiClient client;
    if (!client.connect(host, httpPort))
    {
        Serial.println("connection failed");
        return "-1";
    }
    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print("GET " + url + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0)
    {
        if (millis() - timeout > 5000)
        {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return "-1";
        }
    }
    String retorno = "";
    while (client.available())
    {
        String line = client.readStringUntil('\r');
        retorno += line;
    }
    return retorno;
}
/*
void ClienteUpdate(String url, String caminho)
{
    WiFiClient client;
     Serial.print("connecting to");
    Serial.println(dadosConfig.host);

    if (!client.connect(dadosConfig.host, dadosConfig.httpPort))
    {
        Serial.println("connection failed");
        return;
    }
    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print("GET " + url + " HTTP/1.1\r\nHost: " + dadosConfig.host + "\r\nConnection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0)
    {
        if (millis() - timeout > 5000)
        {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    removeFile(tochar(caminho));
    File file = SD.open(caminho, FILE_WRITE);
    bool b = false;
    String letra = "";
    while (client.available())
    {

        if (!SD.begin())
        {

            return; //cartão não disponivel
        }
        letra = client.readStringUntil('\r');

        if (letra.indexOf("!") >= 0 && b == false)
        {
            b = true;
            file.print("<!");
        }
        if (b)
        {
            file.print(letra);
        }

        ///###########################################
    }

    Serial.println();
    Serial.println("closing connection");
}
*/
void VerificaUpdates()
{
    //defineDataHora(DS3231 dispositivo, byte day, byte month, byte year, byte dow, byte hour, byte minute, byte second)
    String dados = Cliente(dadosConfig.httpPort, tochar(dadosConfig.host), "/dataHora");
    dados.trim();
    if (dados != "-1")
    {
        URL req(dados);
        req.url = dados;
        req.url.replace("&", "?");
        /*
    Serial.println(req.GetAtr("hora"));
    Serial.println(req.GetAtr("minuto"));
    Serial.println(req.GetAtr("segundo"));
    Serial.println(req.GetAtr("dia"));
    Serial.println(req.GetAtr("mes"));
    Serial.println(req.GetAtr("ano"));
*/
        defineDataHora(relogio, req.GetAtr("dia").toInt(), req.GetAtr("mes").toInt(), req.GetAtr("ano").toInt(), req.GetAtr("dow").toInt(), req.GetAtr("hora").toInt(), req.GetAtr("minuto").toInt(), req.GetAtr("segundo").toInt());
    }

    if (!SD.begin())
    {
        return; //cartão não disponivel
    }
    //ClienteUpdate("/configHorario","/configHorario.html");
    // Cliente(dadosConfig.httpPort,tochar(dadosConfig.host) , "/configHorario");
}