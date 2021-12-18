<?php
	require_once "bd/bd.php";
	include "html/index.html";
	include "html/busca.html";
	//echo "Hello World!";
	//date_default_timezone_set('America/Sao_Paulo');
	//echo  $dataLocal = date('d/m/Y H:i:s', time());
	if(!$db){
		echo "erro ao conectar ao banco de dados!";
		exit();
	}


?>

