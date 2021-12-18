#pragma region //salva config -salva configurações gerais no cartão de memoria
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


#pragma endregion
#pragma region //configAuto - carrega di cartão configurações gerais
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

#pragma endregion
