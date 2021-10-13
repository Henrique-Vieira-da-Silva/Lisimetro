'use strict';
module.exports = { 
  up: (queryInterface, DataTypes) => {
    return queryInterface.createTable('Medicaos', {
      id: {
        allowNull: false,
        autoIncrement: true,
        primaryKey: true,
        type: DataTypes.INTEGER,
      },
      balanca: {
        allowNull: false,
        type: DataTypes.INTEGER,
      },
      name: {
        allowNull: false,
        type: DataTypes.STRING,
      },
      tara: {
        allowNull: false,
        type: DataTypes.REAL,
      
      },
      escala: {
        allowNull: false,
        type: DataTypes.REAL,
        
      },
      massa: {
        allowNull: false,
        type: DataTypes.REAL,
      },
      createdAt: {
        allowNull: false,
        type: DataTypes.DATE,
      },
      updatedAt: {
        allowNull: false,
        type: DataTypes.DATE,
      },
    });
  },

  down: (queryInterface) => {
    return queryInterface.dropTable('Medicaos');
  }
};