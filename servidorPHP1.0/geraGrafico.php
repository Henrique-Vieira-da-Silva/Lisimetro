<?php
require_once "bd/bd.php";
include "html/index.html";
if(isset($_GET['busca'])){
    //echo var_dump($db->query('SELECT * FROM medicoes WHERE name LIKE "%teste%"'));
    if($results = $db->query('SELECT * FROM medicoes WHERE name LIKE "'.$_GET['busca'].'" AND datetime between "'.$_GET['data1'].' " AND "'.$_GET['data2'].'"')){
        if($results->num_rows <= 0){
           
            echo '<div class="alert alert-danger" role="alert">
            Lisimetro Não Encontrado! <button type="button" onClick="history.go(-1)" class="btn btn-danger">Voltar</button>
          </div>';
          
            exit();
        }
        echo "<input id='dados' readonly type='text' style='visibility:hidden' value='[";
     
        $cont=0;$l1=0;$l2=0;$l3=0;
        $dataAntiga=0;
    foreach($results as $result){
            
            $result = json_encode($result);
            $result = json_decode($result);
            if($cont == 0){
                $dataA = substr($result->datetime, 0, 16);
                $dataAtiga = $result->datetime;
            }
            
   
           // echo var_dump($result);
           
            //echo var_dump($result);
            //echo $result->name;
           
           

 //echo '<br/>'.'teste'.$dataA.'fim'.substr($result->datetime, 0, 16).'n'.$result->balanca;
            if($dataA !=  substr($result->datetime, 0, 16)){  
                //echo 'imprimiu';
                echo '{"l1":'.$l1.',"l2":'.$l2.',"l3":'.$l3.',"data":"'.$dataAtiga.'"},';
                //if($cont != $results->num_rows){ echo ',';}
                $dataA =  substr(strval($result->datetime), 0, 16);
                $l1=0;$l2=0;$l3=0;
                
            }
            
           
               // echo '<br/>'.$result->balanca.'cont'.$cont;
           if($result->balanca == "1"){
            $l1 =  $result->massa;
           }
           

           if($result->balanca == "2"){
            $l2 =  $result->massa;
           }
           

           if($result->balanca == "3"){
               
            $l3 =  $result->massa;
           }
           
    
        
        $cont++;
        $dataAtiga = $result->datetime;
        

    }
    echo '{"l1":'.$l1.',"l2":'.$l2.',"l3":'.$l3.',"data":"'.$dataAtiga.'"}';
         
        echo "]' />";
        echo ' <canvas id="myCanvas" style="margin:center" width="1000px" height="1000px" style="border:1px solid #d3d3d3;">
        Your browser does not support the HTML canvas tag.</canvas>';
        echo "<script>";
        include "js/grafico.js";
        echo "</script>";
    }else{
       
        echo '<div class="alert alert-danger" role="alert">
        Lisimetro Não Encontrado! <button type="button" onClick="history.go(-1)" class="btn btn-danger">Voltar</button>
      </div>';
            exit();
    }

}

?>
