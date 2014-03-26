var comics = fs.lsdir("comics");
var comicCount = 1000;

var currentComic = -1,
	currentComicIndex = 0,
	currentComicWidth = -1,
	currentComicHeight = -1;

function loadComic(index) {
	var imgArray = draw.getImageArray("comics/" + comics[currentComicIndex]);

	currentComicWidth = imgArray.width;
	currentComicHeight = imgArray.height;

	currentComic = draw.createImage(imgArray, currentComicWidth, currentComicHeight);
}

sys.on("draw", "comicViewer.draw", function (e) {

	draw.setColor("white");
	var y = 0;
	for (var i = currentComicIndex; i < comics.length && i < currentComicIndex + 20; i++) {
		draw.print(20, (y++ * 16) + 40, i.toString(10) + " : " + comics[i]);
		draw.setColor("grey");
	}

	draw.setColor("white");

	if (draw.isTexture(currentComic)) {
		draw.draw(currentComic, 280, 40, currentComicWidth, currentComicHeight);
	} else {
		loadComic(currentComicIndex);
	}
});

sys.on("input", "comicViewer.input", function (e) {
	console.log(e.state, "\"", e.key, "\"");
	if (e.state != "press") return;

	if (e.key == "S") {
		if (currentComicIndex < comics.length) {
			draw.freeImage(currentComic);
			currentComicIndex++;
		}
	} else if (e.key == "W") {
		if (currentComicIndex > 0) {
			draw.freeImage(currentComic);
			currentComicIndex--;
		}
	}
});