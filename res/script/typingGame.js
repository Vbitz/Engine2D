// Typing Game, started at Fri 10 Jan 11:44:00pm
// Game works at 11:59:16pm (wow 15 minutes)
// Bugs fixed and ready to commit at 12:09:50pm (It was a issue with the way I was handling input)

function randFloat(min, max) {
	if (max === undefined) {
		max = min;
		min = 0;
	}
	return (Math.random() * (max - min)) + min;
}

function rand(min, max) {
	return Math.floor(randFloat(min, max));
}

function randArray(arr) {
	return arr[rand(arr.length)];
}

var score = 0;

function lose() {
	console.log("lost at: " + score);
	sys.forceReload("script/typingGame.js");
}

var wordBank = [
	"Engine2D"
	// You can add your own wordbank here or load it from a file
];

wordBank = wordBank.map(function (i) {
	return i.toLowerCase();
});

var words = [];

var pendingKeys = [];

var keyLastFrame = false;

function addWord() {
	words.push({
		word: randArray(wordBank),
		x: rand(100, sys.screenWidth - 100),
		y: rand(100, sys.screenHeight - 100)
	});
}

var lastKey = "";

addWord();

sys.clearEvent("typingGame.draw");
sys.on("draw", "typingGame.draw", function () {
	draw.setColor("white");
	draw.setFont("basic", 28);

	var removeWords = [];

	for (var i = 0; i < words.length; i++) {
		word = words[i];

		// update word
		if (word.word.length > 0 && word.word[0].toUpperCase() === lastKey) {
			lastKey = "";
			score += 10;
			word.word = word.word.substring(1);
			if (word.word.length < 1) {
				score += 1000;
				removeWords.push(i);
				addWord();
				break;
			}
		}

		draw.print(word.x, word.y, word.word);
	}

	removeWords.forEach(function (i) {
		words.splice(i, 1); // remove at index
	});

	draw.setColor("blue");
	draw.setFont("basic", 16);
	// draw score info
	draw.print(32, 32, "Score: " + score);
});

sys.clearEvent("typingGame.input");
sys.on("input", "typingGame.input", function (e) {
	var key = e["key"];
	if (e["state"] == "press") {
		lastKey = key;
	}
});