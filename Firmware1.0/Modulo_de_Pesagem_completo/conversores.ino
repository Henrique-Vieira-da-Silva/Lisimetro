#pragma region //converte - converte uma url em um ip
IPAddress converte(URL teste, String atr)
{
    IPAddress a(teste.GetAtr(atr).substring(0, 3).toInt(), teste.GetAtr(atr).substring(3, 6).toInt(), teste.GetAtr(atr).substring(6, 9).toInt(), teste.GetAtr(atr).substring(9, 12).toInt());
    return (a);
} 
#pragma endregion
#pragma region //tochar - converte uma string em um ponteiro de char
const char *tochar(String string)
{
    const char *a = string.c_str();
    return (a);
}

#pragma endregion
#pragma region tochars - converte uma string em um ponteiro de unsigned char
const unsigned char *tochars(String string)
{
    const unsigned char *a = reinterpret_cast<const unsigned char *>(tochar(string));
    return (a);
}

#pragma endregion