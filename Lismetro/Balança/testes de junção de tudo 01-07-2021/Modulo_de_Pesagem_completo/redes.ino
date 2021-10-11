#pragma region //salva Wifi - salva os dados do wifi no cartão.

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
#pragma endregion
#pragma region //Wifi auto - recupera os dados de wifi do cartão.
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
#pragma endregion
#pragma region //AP - inicializa arduino no modo roteador.
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
#pragma endregion
#pragma region //WFConnection - conecta o arduino a uma rede Wirelless
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
#pragma endregion
#pragma region //cliente - fazer requisições HTTP
String Cliente(const int httpPort, const char *host, String url)
{
    Serial.print("connecting to");
    Serial.println(host);
    Serial.print(" At:"+ String(httpPort));
    Serial.print("|end|");
    WiFiClient client;
    if (!client.connect(host, httpPort))
    {
        Serial.println("connection failed");
        return "-1";
    }
    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print(String("GET ") + url + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n");
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
#pragma endregion
#pragma region //verificaUpdates - procura por atualizações e baixa 
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
#pragma endregion