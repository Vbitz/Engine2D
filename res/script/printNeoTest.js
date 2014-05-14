event.draw = function printNeoTest_draw(e) {
	if (!draw.isFontLoaded("sourceSansPro")) {
		//console.log("Loading Font");
		draw.loadFont("sourceSansPro", "fonts/SourceSansPro-Regular.ttf");
	}

	draw.printNeo(60, 60, 32, "Hello World");
	draw.printNeo(60, 100, 16, "Hello World");
	draw.printNeo(60, 140, 8, "Hello World");
	draw.printNeo(60, 180, 12, "Hello World");

	draw.setFont("sourceSansPro", 32);
	draw.print(220, 60, "Hello World");
	draw.setFont("sourceSansPro", 16);
	draw.print(220, 100, "Hello World");
	draw.setFont("sourceSansPro", 8);
	draw.print(220, 140, "Hello World");
	draw.setFont("sourceSansPro", 12);
	draw.print(220, 180, "Hello World");
};