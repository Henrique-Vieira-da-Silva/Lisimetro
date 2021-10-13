robocopy Recursos servidor/node_modules/.bin/config config.json
robocopy Recursos servidor/node_modules/src/app index.js
cd servidor/node_modules/.bin
npx sequelize db:migrate