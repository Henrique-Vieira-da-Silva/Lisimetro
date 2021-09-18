
const express = require('express')//importa o express
const app = express()//inicia express, cria um novo objeto express
const port = 3000//definindo uma porta em uma variavel
const { Medicao } = require('../../.bin/models/');
app.get('/hvh', (req, res) => {
    res.send("Aqui é o GET");
    Medicao.create({ name: 'Henrique', email: 'Henrique@hvh.com.br', password: 'SenhaMalucaForteTbm' });
});
app.listen(port, () => {
    console.log(`Example app listening at http://localhost:${port}`)//só avisa que tá rodando
})