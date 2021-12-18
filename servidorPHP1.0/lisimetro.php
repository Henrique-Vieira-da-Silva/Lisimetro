<?php
require_once "bd/bd.php";
include "html/index.html";

if(isset($_GET['busca'])){
    //echo var_dump($db->query('SELECT * FROM medicoes WHERE name LIKE "%teste%"'));
    if($results = $db->query('SELECT * FROM medicoes WHERE name LIKE "'.$_GET['busca'].'"')){
        if($results->num_rows <= 0){
            echo '<div class="alert alert-danger" role="alert">
            Lisimetro Não Encontrado! <button type="button" onClick="history.go(-1)" class="btn btn-danger">Voltar</button>
          </div>';
            exit();
        }
                echo '<br/><div class="col-4"><input id="cod" type="text" readonly value="'.$_GET['busca'].'"  name="busca" class="form-control busca" placeholder="Código" aria-label="Username" aria-describedby="basic-addon1"></div> <p style="margin-left: 2%">Numero Total de Registros: '.$results->num_rows.'</p>';
        include "html/downloads.html";
        include "js/codigos.js";
       
    }else{

        echo '<div class="alert alert-danger" role="alert">
        Lisimetro Não Encontrado! <button type="button" onClick="history.go(-1)" class="btn btn-danger">Voltar</button>
      </div>';
      
            exit();
    }

}

?>
