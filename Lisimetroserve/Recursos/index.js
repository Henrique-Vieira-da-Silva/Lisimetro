
const express = require('express')//importa o express
const app = express()//inicia express, cria um novo objeto express
const port = 3000//definindo uma porta em uma variavel
const { Medicao } = require('../../.bin/models/');

const { config } = require('process');
const { takeCoverage } = require('v8');
app.get('/ConfigHorario', (req, res) => {

    res.sendFile(__dirname + "/ConfigHora.html");
    //res.send("Aqui é o GET");
    //Medicao.create({ name: 'Henrique', email: 'Henrique@hvh.com.br', password: 'SenhaMalucaForteTbm' });
});

app.get('/', (req, res) => {


    res.write("<html lang='pt-br'> <meta charset='UTF-8'><title>Menu</title>");
    res.write("<a href='/'>Menu</a> <br/>");
    res.write("<h1>Menu</h1> <br/> <hr> <br/>");
    res.write("<a href='/download'>Baixar Arquivo .CSV com todos os registros</a> <br/><br/>");
    res.write("<form method='get' action='/download2'><input name='name' placeholder='Periodo desejado' />  <button type='submit'>Baixar .CSV deste Periodo</button></form>");
    res.end();

});



app.get('/dataHora', (req, res) => {
    var data = new Date();
    console.log("Atualizando...");
    res.send("/dataHora?hora=" + data.getHours() + "&minuto=" + data.getMinutes() + "&segundo=" + data.getSeconds() + "&dia=" + data.getDate() + "&mes=" + data.getMonth() + "&ano=" + (parseInt(data.getYear()) - 100) + "&dow=" + data.getDay() + "HTPP \n");

})

app.get('/gravar', (req, res) => {
    Medicao.create({ name: req.query.nome.toString(), balanca: parseInt(req.query.balanca), tara: parseFloat(req.query.tara), escala: parseFloat(req.query.escala), massa: parseFloat(req.query.massa) });
    console.log("gravando...");
    res.send("true");

})

app.get('/download', (req, res) => {
    res.setHeader('Content-disposition', 'attachment; filename=dados.csv');
    res.setHeader('Content-type', 'text/csv');

    var a = Medicao.findAll({
        attributes: ['id', 'name', 'balanca', 'tara', 'massa', 'createdAt']
    });

    a.then(function (value) {
        //console.log(value.every(value => value instanceof Medicao)); // true
        var valores = JSON.stringify(value, null, 2);
        
        console.log("All users:", value.length);
        if(value.length == 0){
            req.end();
            return;
        }
        res.write("L1 ; L2 ; L3 ; Data ; Hora ; DateTime\n");
        var data1 = new Date(value[0].createdAt);
        var massa1 = 0.0, massa2 = 0.0, massa3 = 0.0;
        value.forEach(values => {
            var data = new Date(values.createdAt);
            //console.log(data1.getMinutes() + " // " + data.getMinutes());
            if (data1.getMinutes() != data.getMinutes()) {
                res.write((massa1 + " ; " + massa2 + " ; " + massa3 + " ; " + data1.getDate() + "/" + (data1.getMonth() + 1) + "/" + data1.getFullYear() + " ; " + data1.getHours() + ":" + data1.getMinutes() + ":" + data1.getSeconds() + " ; " + data1.toUTCString() + "\n").replace('.', ','));
                massa1 = 0.0, massa2 = 0.0, massa3 = 0.0;
                data1 = data;
            }
            switch (parseInt(values.balanca)) {
                case 1:
                    massa1 = values.massa;
                    break;
                case 2:
                    massa2 = values.massa;
                    break;
                case 3:
                    massa3 = values.massa;
                    break;
            }


        });
        //res.write("teste");
        res.end();
    });


});

app.get('/download2', (req, res) => {
    res.setHeader('Content-disposition', 'attachment; filename=dados.csv');
    res.setHeader('Content-type', 'text/csv');

    var a = Medicao.findAll({
        attributes: ['id', 'name', 'balanca', 'tara', 'massa', 'createdAt'],
         where:{
            name : req.query.name
        }
    });

    a.then(function (value) {
        //console.log(value.every(value => value instanceof Medicao)); // true
        var valores = JSON.stringify(value, null, 2);
        console.log("All users:", value.length);
        if(value.length == 0){
            req.end();
            return;
        }
        res.write("L1 ; L2 ; L3 ; Data ; Hora ; DateTime\n");
        var data1 = new Date(value[0].createdAt);
        var massa1 = 0.0, massa2 = 0.0, massa3 = 0.0;
        
        value.forEach(values => {
            var data = new Date(values.createdAt);
            //console.log(data1.getMinutes() + " // " + data.getMinutes());
            if (data1.getMinutes() != data.getMinutes()) {
                res.write((massa1 + " ; " + massa2 + " ; " + massa3 + " ; " + data1.getDate() + "/" + (data1.getMonth() + 1) + "/" + data1.getFullYear() + " ; " + data1.getHours() + ":" + data1.getMinutes() + ":" + data1.getSeconds() + " ; " + data1.toUTCString() + "\n").replace('.', ','));
                massa1 = 0.0, massa2 = 0.0, massa3 = 0.0;
                data1 = data;
            }
            switch (parseInt(values.balanca)) {
                case 1:
                    massa1 = values.massa;
                    break;
                case 2:
                    massa2 = values.massa;
                    break;
                case 3:
                    massa3 = values.massa;
                    break;
            }


        });
        //res.write("teste");
        res.end();
    });
});


app.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)//só avisa que tá rodando
})