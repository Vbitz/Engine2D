fs.configDir("Basic");

fs.writeFile("testing.txt", "Hello world");

db.open("hello.db");

db.exec("DROP TABLE IF EXISTS testing");
db.exec("CREATE TABLE IF NOT EXISTS testing(t1 TEXT, t2 NUM)");
db.exec("INSERT INTO testing VALUES('hello world', 2)");

console.log(JSON.stringify(db.execPrepare("SELECT * FROM testing"), 0, '\t'));

sys.drawFunc(function () {
	draw.rect(100, 100, 200, 200);
});

sys.keyboardFunc(function (type, key, press) {
	if (key === " ") {
		sys.toggleFullscreen();
	} else if (key === "ESC") {
		sys.exit();
	}
});