robocopy Recursos servidor/node_modules/.bin/config config.json
cd servidor/node_modules/.bin
npx sequelize migration:create --name=Medicoes