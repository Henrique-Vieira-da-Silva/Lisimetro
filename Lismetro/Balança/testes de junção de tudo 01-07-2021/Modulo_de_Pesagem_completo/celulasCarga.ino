#pragma region //balancaAuto - carrega dados do cartão para configurar as balanças
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
            escala1 = req.GetAtr("escala1").toDouble();
            escala2 = req.GetAtr("escala2").toDouble();
            escala3 = req.GetAtr("escala3").toDouble();
            periodo1 = req.GetAtr("periodo1");
            periodo2 = req.GetAtr("periodo2");
            periodo3 = req.GetAtr("periodo3");
        }
    }
    return false;
}

#pragma endregion
#pragma region //salvaConfigBalancas - Salva no cartão de memoria as configurações da balanca
bool salvaConfigBalancas(URL req, bool n)
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
        if (n)
        { //veradeiro grava a partir da url
            conteudo += "?tara1=" + req.GetAtr("tara1");
            conteudo += "?tara2=" + req.GetAtr("tara2");
            conteudo += "?tara3=" + req.GetAtr("tara3");
            conteudo += "?escala1=" + String(escala1);
            conteudo += "?escala2=" + String(escala2);
            conteudo += "?escala3=" + String(escala3); 
            conteudo += "?periodo1=" + req.GetAtr("periodo1");
            conteudo += "?periodo2=" + req.GetAtr("periodo2");
            conteudo += "?periodo3=" + req.GetAtr("periodo3");

        }
        else
        {
            conteudo += "?tara1=" + String(tara1);
            conteudo += "?tara2=" + String(tara2);
            conteudo += "?tara3=" + String(tara3);
            conteudo += "?escala1=" + String(escala1);
            conteudo += "?escala2=" + String(escala2);
            conteudo += "?escala3=" + String(escala3);
             conteudo += "?periodo1=" + periodo1; 
            conteudo += "?periodo2=" + periodo2; 
            conteudo += "?periodo3=" + periodo3; 
        }
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
#pragma endregion
#pragma region //medir - realiza mediçõs com medias x 20
double medir(HX711 Dispositivo)
{  return (Dispositivo.get_units(20) * 1000);
}
#pragma endregion
#pragma region // inicializarBalanca -  inicializa a celula de carga
void inicializarBalanca(HX711 *Dispositivo,double scale, float Tara, int Data, int Clock)
{
    Serial.println("Inicializando! Agurdade...");
    Serial.println("Retire qualquer peso que esteja sobre a balança!");
    delay(3000);
    Dispositivo->begin(Data, Clock);
    Serial.printf("Inicializado!");
    Dispositivo->tare(Tara);
    Dispositivo->set_scale(scale);
}
#pragma endregion
#pragma region //recuperaBalanca -  reinicializa uma balanca com novos parametros.
void recuperaBalanca(HX711 *Dispositivo,double scale,double tara)
{
    Dispositivo->set_scale(scale);
    Dispositivo->tare(tara);
}

#pragma endregion
#pragma region //calibrar - calibra uma baalanca
void calibrar(HX711 *Dispositivo, double MassaPadrao, double *escala, WiFiClient client)
{
    client.println("<html lang='pt-br'><meta charset='UTF-8'><title>Calibração</title>");
    client.println("<h1>Calibração</h1><br/><hr><br/>");
    client.println("Calibrando!<br/>");
    client.print("Coloque a massa conhecida! de: ");
    client.print(MassaPadrao * 1000);
    client.println(" g em até 3s sobre a balança <br/>");
    delay(3000);
    client.println("iniciando Calibração ! <br/>");
    double medida = 0;
    for (int i = 0; i < 10; i++)
    {
        medida += Dispositivo->get_value(10), 0;
        client.println("Executando... <br/>");
        //Serial.println(Dispositivo.get_value(10),0);
    }
    client.println("Finalizado <br/>");
    *escala = (((medida / 10) / MassaPadrao));
    Dispositivo->set_scale(((medida / 10) / MassaPadrao));

    URL a("currentLine");
    salvaConfigBalancas(a, false);
    client.println("<a href='/'>Voltar ao Menu</a> <br/>");
}

#pragma endregion
#pragma region //medirRapido - realiza medição com medias x1
double medirRapido(HX711 Dispositivo)
{
    double medicao = (Dispositivo.get_units(1) * 1000);
    return  (medicao >= 0.0) ?medicao : -0.00 ;
}
#pragma endregion