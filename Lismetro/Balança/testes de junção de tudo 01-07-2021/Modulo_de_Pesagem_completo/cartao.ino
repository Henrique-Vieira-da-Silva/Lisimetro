#pragma region //appendFile
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
#pragma endregion
#pragma region  //removeFile
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
#pragma endregion
#pragma region //createFile
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
#pragma endregion
#pragma region  //createDir
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
#pragma endregion
#pragma region  //readFile
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
#pragma endregion
#pragma region //writeFile
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
#pragma endregion
#pragma region //renameFile
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
#pragma endregion
#pragma region //delefile
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
#pragma endregion
#pragma region //testeIO
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
#pragma endregion
#pragma region //mkdirs - cria uma ou varias pastas uma dentro da outra
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

#pragma endregion
