// Typing Game, started at Fri 10 Jan 11:44:00pm
// Game works at 11:59:16pm (wow 15 minutes)
// Bugs fixed and ready to commit at 12:09:50pm (It was a issue with the way I was handling input)

sys.config("core.debug.profiler", true);

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
var lives = 10;

function lose() {
	console.log("lost at: " + score);
	sys.forceReload("script/typingGame.js");
}

var maxWordLength = 50;

// WordList derived from "Moby Word Lists by Grady Ward" released under Public Domain
var wordBank = fs.readFile("content/wordList.txt")
	.split('\n')
	.splice(1)
	.map(function (i) {
		return i.trim();
	})
	.filter(function (i) {
		return i.length <= maxWordLength && i.length > 0;
	});

console.log("Loaded " + wordBank.length + " words");

wordBank = wordBank.map(function (i) {
	return i.toLowerCase();
});

var words = [];

var pendingKeys = [];

var keyLastFrame = false;

function addWord() {
	var word = randArray(wordBank);
	console.log("adding: [" + word + "]");
	words.push({
		word: word,
		x: rand(100, sys.screenWidth - 100),
		y: 50
	});
}

var lastKey = "";

addWord();

sys.clearEvent("typingGame.draw");
sys.on("draw", "typingGame.draw", function () {
	draw.setColor("white");
	draw.setFont("basic", 28);

	var removeWords = [];

	var realWordSpeed = 100 * sys.deltaTime;

	for (var i = 0; i < words.length; i++) {
		word = words[i];

		if (word.word.length <= 0) {
			continue;
		}

		// update word
		if (word.word[0].toUpperCase() === lastKey) {
			lastKey = "";
			score += 10;
			word.word = word.word.substring(1);
			if (word.word.length < 1) {
				score += 100;
				removeWords.push(i);
				addWord();
				break;
			}
		}

		word.y += realWordSpeed;

		if (word.y > sys.screenHeight) {
			lives--;
			if (lives < 0) {
				lose();
			}
			removeWords.push(i);
			addWord();
			break;
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