
var preConfigDirTests = {
// Before fs.configDir is set

// JSDatabase
	"db.open Arg0 invalid type": function () {
		// Should fail with "Arg0 is the filename of the database"
		try {
			db.open(0);
			db.close();
			return false;
		} catch (e) {
			db.close();
			return true;
		}
	},
	"db.open no fs.configDir": function () {
		// Should fail with "fs.configDir needs to be called before db.open can be called"
		try {
			db.open("hello.db");
			db.close();
			return false;
		} catch (e) {
			db.close();
			return true
		}
	},
	"db.close no database": function () {
		// Should fail with "A database needs to be loaded with fs.open before it can be closed"
		try {
			db.close()
			return false;
		} catch (e) {
			return true;
		}
	},
	"db.exec no database": function () {
		// Should fail with "A database needs to be loaded with fs.open before queries can be executed"
		db.open("hello.db");
		try {
			db.exec("SELECT * FROM Testing_Database;");
			db.close();
			return false;
		} catch (e) {
			db.close();
			return true;
		}
	}

};

var postConfigDirTests = {

};

var drawTests = {

};

for (var i in preConfigDirTests) {

}