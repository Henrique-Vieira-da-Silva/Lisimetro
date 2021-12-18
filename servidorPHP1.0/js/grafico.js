

    var obj = JSON.parse(document.getElementById("dados").value); 
    //alert(obj[1].data);
    var universo=[];

    var avanco = 20;
    var menor = Number.MAX_VALUE, maior = Number.MIN_VALUE;
    var count = 0;
    do{
        universo.push(obj[count]);
        if(obj[count].l1 < menor)menor = obj[count].l1;
        if(obj[count].l2 < menor)menor = obj[count].l2;
        if(obj[count].l3 < menor)menor = obj[count].l3;

        if(obj[count].l1 > maior)maior = obj[count].l1;
        if(obj[count].l2 > maior)maior = obj[count].l2;
        if(obj[count].l3 > maior)maior = obj[count].l3;
        count++; 
    }while(count < obj.length || count >= 100);
    //universo = JSON.parse(universo);

    var c = document.getElementById("myCanvas");
            var ctx = c.getContext("2d");
            var amplitude = maior-menor;
            if(amplitude<0){
                amplitude*=-1;
            }
            var marcasY = parseInt(1000/amplitude);
            
            if(marcasY <= 50){
                marcasY =50;
                taxa = 1000/amplitude;
            }
            marcasY2 = marcasY;
            //desenhando escala
            ctx.moveTo(10,0);
            ctx.lineTo(10,1000);
            ctx.stroke();
            
        

      

            count=0;
            
            marcasY = 1000-marcasY;
            while(count < amplitude){
                ctx.font = '15px serif';
                ctx.fillText((maior-marcasY/taxa), 11, marcasY+15);
                ctx.moveTo(5,marcasY);
                ctx.lineTo(15,marcasY);
                ctx.stroke();
                marcasY-=marcasY2;
               //console.log(marcasY);
                count++;
            }
            ctx.moveTo(10,999);
            ctx.lineTo(1000,999);
            ctx.stroke();

    

            
            //desenhando grafico
            count=10, ul1 = 990,ul2 = 990,ul3 = 990;
            universo.forEach(element => {
                ctx.beginPath();
                ctx.moveTo(count,ul1);
                ctx.lineTo(count+avanco,1000-taxa*element.l1);
                ctx.lineWidth=3;
                ctx.strokeStyle = '#ff0000';
                ul1 =1000-taxa*element.l1;
                console.log(element.l1);
                ctx.stroke();

                ctx.beginPath();
                ctx.moveTo(count,ul2);
                ctx.lineTo(count+avanco,1000-taxa*element.l2);
                ctx.lineWidth=3;
                ctx.strokeStyle = '#00ff00';
                ul2 =1000-taxa*element.l2;
                console.log(element.l2);
                ctx.stroke();

                ctx.beginPath();
                ctx.moveTo(count,ul3);
                count+=avanco;
                ctx.lineTo(count,1000-taxa*element.l3);
                ctx.lineWidth=3;
                ctx.strokeStyle = '#0000ff';
                ul3 =1000-taxa*element.l3;
                console.log(element.l3);
                ctx.stroke();
                ctx.save();
                ctx.translate(0,0);
                ctx.rotate( Math.PI / 4 );
                ctx.font = '10px serif';
                ctx.fillText(element.data,count ,500);
                ctx.restore();
              
            });
            //ctx.moveTo(0,0);
           // ctx.lineTo(200,100);
            ctx.stroke();
