#pragma region //defineDataHora - define data e hora no relogio
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

#pragma endregion
#pragma region //ALARME - VERIFICA SE A HORA DE ENVIAR DADOS
void alarme()
{   
    RTClib myRTC;
    DateTime now = myRTC.now();
    if(now.unixtime() >= sProxLeitura){
        sProxLeitura = now.unixtime() + dadosConfig.tempo*60;
        if(balanca.is_ready()){
            lisimetro(medir(balanca),1);
        }
          if(balanca1.is_ready()){
            lisimetro(medir(balanca1),2);
        }
          if(balanca2.is_ready()){
            lisimetro(medir(balanca2),3);
        }
    }
}
#pragma endregion