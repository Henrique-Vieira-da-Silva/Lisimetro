
//              Modulo de pesagem completo
//                  calibra e pesa.
//          com suporte até 3 moddulos e pesagem
//                      V1.1 Esp32
//      junção de todos os modulos em funcionamento

#pragma region //pinagens

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
#pragma endregion
#pragma region //Bibliotecas
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
#pragma endregion
#pragma region                      //constantes
#define DT 2                        //d2
#define DT1 15                      //d15
#define DT2 0                       //d0
#define SCK 4                       //d4
const char *ssid = "Lisimetro";     //ssdi do modo AP
const char *password = "123456789"; //senha do modo AP
#pragma endregion
#pragma region  //variaveis globais
HX711 balanca;  //objeto balança
HX711 balanca1; //objeto balança1
HX711 balanca2; //objeto balança2
RTClib Clock;   //criando objeto DS3231  (hardware)
DS3231 relogio;
WiFiServer server(80); //porta do servidor
double tara1 = 0, tara2 = 0, tara3 = 0;
double escala1 = 0, escala2 = 0, escala3 = 0;
String periodo1, periodo2, periodo3;
uint32_t sProxLeitura = 0;
#pragma endregion
#pragma region //Classe Configs
class Configs
{
public:
    int httpPort;
    String host;
    int tempo;
};
#pragma endregion
#pragma region //classe URL
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
#pragma endregion
#pragma region //objeto config global
Configs dadosConfig;
#pragma endregion
#pragma region //SETUP - configurações inicias
void setup()

{
    Serial.begin(115200);
    Serial.println("###############################");
    Serial.println("       Inicializando... ");
    Serial.println("    Inicializando em modo AP ");
    Ap();
    pinMode(DT, INPUT);
    pinMode(DT1, INPUT);
    pinMode(DT2, INPUT);
    pinMode(SCK, OUTPUT);
    Wire.begin();
    Serial.println("    Conectando a rede... ");
    wifiAuto();
    Serial.println("   Recuperando dados de configuração... ");
    configAuto();
    Serial.println("   Recuperando dados das balanças... ");
    balancasAuto();
    Serial.println("###############################");
    Serial.println("           Taras               ");
    Serial.println("Balança 1 = " + String(tara1));
    Serial.println("Balança 2 = " + String(tara2));
    Serial.println("Balança 3 = " + String(tara3));
    Serial.println("###############################");
    Serial.println("           Escalas               ");
    Serial.println("Balança 1 = " + String(escala1));
    Serial.println("Balança 2 = " + String(escala2));
    Serial.println("Balança 3 = " + String(escala3));
    Serial.println("###############################");
    Serial.println("    Dados da configuração: ");
    Serial.println("Host: " + dadosConfig.host);
    Serial.println("Porta: " + dadosConfig.httpPort);
    Serial.println("tempo: " + dadosConfig.tempo);
    Serial.println("###############################");
    Serial.println("    Atualizando: ");
    VerificaUpdates();
    Serial.println("###############################");
    Serial.println("           Data e Hora ");
    RTClib myRTC;
    DateTime now = myRTC.now();
    sProxLeitura = now.unixtime();
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
    inicializarBalanca(&balanca, escala1, tara1, DT, SCK);
    //recuperaBalanca(&balanca, escala1, tara1);
    //Cliente(teste3, teste, "/configHorario");
    //mkdirs("x/b/c/d/ola");
    // inicializar(&balanca,tara,DT,SCK);
    //calibrar(&balanca,massa);

    Serial.println("###############################");
    Serial.println("       Setup Finalizado ");
}
#pragma endregion
#pragma region //LOOP #####################################
void loop()
{
#pragma region //FUNÇÕES INICIAIS DE ABERTURA E CONFIGURAÇÕES
    alarme();
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
#pragma endregion
#pragma region //SETWIFI - FUNCIONAL
                    if (req.rota == "setwifi")
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
#pragma endregion
#pragma region //WIFI - VISUAL
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
#pragma endregion
#pragma region                                             //CONFIGBALANCAS - VISUAL
                    else if (req.rota == "configBalancas") //##############################
                    {
                        client.println("<html lang='pt-br'> <meta charset='UTF-8'><title>Configurações das taras</title>");
                        client.println("<a href='/'>Menu</a> <br/>");
                        client.println("<form style='text-align: center;' action='/setBalancas' method='get'> ");
                        client.println("<h1>Configurações das taras em gramas, insira -1 para balanças desativadas</h1><br/><br>");
                        client.println("<h2>preencha todos os campos corretamente</h2><br/><br>");
                        client.println("<input type='text' name='tara1' placeholder='tara 1'><br><br>");
                        client.println("<input type='text' name='tara2' placeholder='tara 2'><br><br>");
                        client.println("<input type='text' name='tara3' placeholder='tara 3'><br><br>");
                        client.println("<input type='text' name='periodo1' placeholder='periodo 1'><br><br>");
                        client.println("<input type='text' name='periodo2' placeholder='periodo 2'><br><br>");
                        client.println("<input type='text' name='periodo3' placeholder='periodo 3'><br><br>");
                        client.println(" <input type='submit' value='Salvar'>");
                        client.println("</form>");
                        break;
                    }
#pragma endregion
#pragma region //CONFIG - VISUAL
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
#pragma endregion
#pragma region //INICIO - FUNCIONAL
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
#pragma endregion
#pragma region //SETCONFIG - FUNCIONAL
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
#pragma endregion
#pragma region                                          //SETBALANCAS - FUNCIONAL
                    else if (req.rota == "setBalancas") //################
                    {
                        if (salvaConfigBalancas(req, true))
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
#pragma endregion
#pragma region                                       //CALIBRAR - VISUAL
                    else if (req.rota == "calibrar") //################
                    {
                        client.println("<html lang='pt-br'> <meta charset='UTF-8'><title>Calibrar Balanças</title>");
                        client.println("<a href='/'>Menu</a> <br/>");
                        client.println("<form style='text-align: center;' action='/calibrarBalanca' method='get'> ");
                        client.println("<h1>Calibrar Balanças</h1><br/>");
                        client.println("<h2>preencha todos os campos corretamente</h2><br/><br>");
                        client.println("<input type='number' name='balanca' placeholder='numero da balança de 1 a 3'><br>");
                        client.println("<input type='number' name='massa' placeholder='massa de calibração em g'><br><br>");
                        client.println(" <input type='submit' value='Enviar'>");
                        client.println("</form>");
                    }
#pragma endregion
#pragma region                                              //CALIBRA BALANCAS - FUNCIONAL
                    else if (req.rota == "calibrarBalanca") //################
                    {
                        //calibrar(HX711 *Dispositivo, double MassaPadrao, double *escala, WiFiClient client)
                        switch (req.GetAtr("balanca").toInt())
                        {
                        case 1:
                            if (balanca.is_ready())
                                calibrar(&balanca, req.GetAtr("massa").toDouble() / 1000, &escala1, client);
                            break;
                        case 2:
                            if (balanca1.is_ready())
                                calibrar(&balanca1, req.GetAtr("massa").toDouble() / 1000, &escala2, client);
                            break;
                        case 3:
                            if (balanca2.is_ready())
                                calibrar(&balanca2, req.GetAtr("massa").toDouble() / 1000, &escala3, client);
                            break;
                        }
                    }
#pragma endregion
#pragma region                                           //MEDIRBALANCA - VISUAL / FUNCIONAL
                    else if (req.rota == "medirBalanca") //################
                    {
                        String massajs = "0";
                        client.println("<html lang='pt-br'><meta charset='UTF-8'><title>Medindo</title>");
                        client.println("<a href='/'>Menu</a> <br/>");
                        client.println("Massa: <input type='text' id='massa'/>g");

                        //calibrar(HX711 *Dispositivo, double MassaPadrao, double *escala, WiFiClient client)
                        while (client.available())
                        {
                           

                            switch (req.GetAtr("balanca").toInt())
                            {
                            case 1:

                                if (balanca.is_ready())
                                    massajs = String(medirRapido(balanca));
                                break;
                            case 2:
                                if (balanca1.is_ready())
                                    massajs = String(medirRapido(balanca1));
                                break;
                            case 3:
                                if (balanca2.is_ready())
                                    massajs = String(medirRapido(balanca2));
                                break;
                            }
                             client.print("<div id='script'><script> document.getElementById('massa').value ='"+massajs);
                            client.println("'; document.getElementById('script').remove();</script></div>");
                        }
                    }
#pragma endregion
#pragma region //FILE/LIST - FUNCIONAL / VISUAL
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
#pragma endregion
#pragma region //MENU - VISUAL
                    else if (req.url == " HTTP")
                    {
                        client.println("<html lang='pt-br'><meta charset='UTF-8'><title>Menu</title>");

                        client.println("<h1>MENU</h1><br/><hr>");
                        client.println("<a href='wifi'>Configurações de WiFi</a> <br/>");
                        client.println("<a href='config'>Configurações de servidor e outros</a> <br/>");
                        client.println("<a href='file/list'>Listar Arquivos Do cartão</a> <br/>");
                        client.println("<a href='configBalancas'>Configurar Taras</a> <br/>");
                        client.println("<a href='calibrar'>Calibrar Balanças</a> <br/>");
                        client.println("<a href='medirBalanca?balanca=1'>Medir Balança 01</a> <br/>");
                        client.println("<a href='medirBalanca?balanca=2'>Medir Balança 02</a> <br/>");
                        client.println("<a href='medirBalanca?balanca=3'>Medir Balança 03</a> <br/>");

                        client.println("");
                    }
#pragma endregion
#pragma region //BUSCAARQUIVOS & 404 - FUNCIONAL / VISUAL
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
#pragma endregion
}
#pragma endregion //#####################################
#pragma region    //comentarios de testes
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
#pragma endregion
#pragma region //LISIMETRO - EXECUTA AS FUNÇÕES PARA COLETAR DADOS, PREPRAR E ENVIAR PARA O SERVIDOR
bool lisimetro(double massa, int balan)
{
    //const int httpPort, const char *host, String url

    switch (balan)
    {
    case 1:
        //    Medicao.create({ name: req.query.nome.toString(), balanca: parseInt(req.query.balanca), tara: parseFloat(req.query.tara) , escala: parseFloat(req.query.escala), massa: parseFloat(req.query.massa) });
        Serial.println("resposta:" + Cliente(dadosConfig.httpPort, tochar(dadosConfig.host), String("/gravar?balanca=" + String(balan) + "&massa=" + String(massa) + "&tara=" + String(tara1) + "&escala=" + String(escala1) + "&nome=" + periodo1)));
        Serial.println("/gravar?balanca=" + String(balan) + "&massa=" + String(massa) + "&tara=" + String(tara1) + "&escala=" + String(escala1) + "&nome=" + periodo1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}
#pragma endregion