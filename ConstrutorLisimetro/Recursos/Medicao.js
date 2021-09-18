module.exports = (sequelize, DataTypes) => {
    const User = sequelize.define('Medicao', {
    name: DataTypes.STRING,
    email: DataTypes.STRING,
    password: DataTypes.STRING,
    });
    return User;
    }