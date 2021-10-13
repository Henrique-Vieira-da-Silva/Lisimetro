
cd servidor/node_modules/.bin
rd /s/q migrations
mkdir migrations 
cd..
cd..
cd..
robocopy Recursos servidor/node_modules/.bin/migrations 20210909120214-create-Medicoes.js
robocopy Recursos servidor/node_modules/.bin/models Medicao.js
cd servidor/node_modules/.bin
npm install sqlite3