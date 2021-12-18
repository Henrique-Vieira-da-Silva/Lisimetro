<?php
header('Content-type: text');
	//date_default_timezone_set('America/Sao_Paulo');
	//echo  $dataLocal = date('d/m/Y H:i:s', time());
//res.send("/dataHora?hora=" + data.getHours() + "&minuto=" + data.getMinutes() + "&segundo=" + data.getSeconds() + "&dia=" + data.getDate() + "&mes=" + data.getMonth() + "&ano=" + (parseInt(data.getYear()) - 100) + "&dow=" + data.getDay() + "HTPP \n");
	date_default_timezone_set('America/Sao_Paulo');
	echo  "/dataHora?hora=".date('H', time())."&minuto=".date('i', time())."&segundo=".date('s', time())."&dia=".date('d', time())."&mes=".date('m', time())."&ano=".date('y', time())."&dow=".date('w', time());

?>