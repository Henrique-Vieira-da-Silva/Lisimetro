
const express = require('express')//importa o express
const app = express()//inicia express, cria um novo objeto express
const port = 3000//definindo uma porta em uma variavel
const { Medicao } = require('../../.bin/models/');

const { config } = require('process');
const { takeCoverage } = require('v8');
app.get('/ConfigHorario', (req, res) => {
    
    res.sendFile(__dirname+"/ConfigHora.html");
    //res.send("Aqui é o GET");
    //Medicao.create({ name: 'Henrique', email: 'Henrique@hvh.com.br', password: 'SenhaMalucaForteTbm' });
});
 

app.get('/dataHora', (req,res)=>{
    var data = new Date();
    console.log("Atualizando...");
    res.send("/dataHora?hora="+data.getHours()+"&minuto="+data.getMinutes()+"&segundo="+data.getSeconds()+"&dia="+data.getDate()+"&mes="+data.getMonth()+"&ano="+(parseInt(data.getYear())-100)+"&dow="+data.getDay()+"HTPP \n");

})

app.get('/gravar', (req,res)=>{
    Medicao.create({ name: req.query.nome.toString(), balanca: parseInt(req.query.balanca), tara: parseFloat(req.query.tara) , escala: parseFloat(req.query.escala), massa: parseFloat(req.query.massa) });
    console.log("gravando...");
    res.send("true");

})

app.get('/download', (req, res)=>{
    res.setHeader('Content-disposition', 'attachment; filename=dados.csv');
    res.setHeader('Content-type', 'video/quicktime');

    var a = Medicao.findAll({
        attributes: ['id','name', 'balanca','tara','massa','createdAt']
    });

    a.then(function(value){
        console.log(value.every(value => value instanceof Medicao)); // true
        var valores = JSON.stringify(value, null, 2);
        console.log("All users:", value.length);
        res.write("nome ; balanca ; massa \n");
        value.forEach(values => {
            res.write(values.name+" ; "+values.balanca+" ; "+values.massa+"\n");
        });
        res.write("teste");
        res.end();
    });


})


app.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)//só avisa que tá rodando
})