cd servidor/node_modules
mkdir src\app
cd..
cd..
robocopy Recursos servidor/node_modules/src/app index.js
cd servidor/node_modules/.bin
npx sequelize init