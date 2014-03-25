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
	draw.print(40, 40, "currentComic = " + comics[currentComicIndex]);

	if (draw.isTexture(currentComic)) {
		draw.draw(currentComic, 40, 60, currentComicWidth, currentComicHeight);
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