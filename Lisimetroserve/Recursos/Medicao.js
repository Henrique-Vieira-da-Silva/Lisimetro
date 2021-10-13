module.exports = (sequelize, DataTypes) => {
    const User = sequelize.define('Medicao', {
    name: DataTypes.STRING,
    balanca: DataTypes.INTEGER,
    tara: DataTypes.FLOAT,
    escala: DataTypes.FLOAT,
    massa: DataTypes.FLOAT,
    });
    return User;
    } 