<?php
require_once "bd/bd.php";
date_default_timezone_set('America/Sao_Paulo');

if(isset($_GET['nome']) && isset($_GET['balanca']) && isset($_GET['tara']) && isset($_GET['massa']) ){
    //echo ('INSERT INTO medicoes(name,balanca,tara,massa,data) VALUES("'.$_GET['name'].'","'.$_GET['balanca'].'" ,"'.$_GET['tara'].'","'.$_GET['massa'].'","'.$dataLocal = date('Y-m-d H:i:s', time()).'")');
    if($results = $db->query('INSERT INTO medicoes(name,balanca,tara,massa,datetime) VALUES("'.$_GET['nome'].'","'.$_GET['balanca'].'" ,"'.$_GET['tara'].'","'.$_GET['massa'].'","'.$dataLocal = date('Y/m/d H:i:s', time()).'")')){

        echo 'sucesso ao inserir';
        //echo var_dump($results);
            exit();
        }else{
            echo 'falha ao inserir';
            exit();
        }
    }else{

        echo 'falha ao inserir2';
            exit();
    }



?>
