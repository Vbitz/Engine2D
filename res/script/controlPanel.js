var controls = [];

function getSpecialChars(key) {
	switch (key) {
		case '[':       return '{';
		case ']':       return '}';
		case '-':       return '_';
		case '=':       return '+';
		case '\\':      return '|';
		case '`':       return '~';
		case '1':       return '!';
		case '2':       return '@';
		case '3':       return '#';
		case '4':       return '$';
		case '5':       return '%';
		case '6':       return '^';
		case '7':       return '&';
		case '8':       return '*';
		case '9':       return '(';
		case '0':       return ')';
		case ',':       return '<';
		case '.':       return '>';
		case ';':       return ':';
		case '\'':      return '"';
		case '/':       return '?';
		default:        return key;
	}
}

var currentText = "";

function onInput(raw, key) {
	if (raw) currentText += key.toLowerCase();
	else {
		if (key === "ESCAPE") {
			currentText = "";
		} else if (key === "ENTER") {
			controls.forEach(function (control) {
				control.submitInput(currentText);
			});
			currentText = "";
		} else {
			console.log(key);
			controls.forEach(function (control) {
				control.submitSpecialInput(key);
			});
		}
	}
}

function ButtonControl(label, x, y, callback) {
	this.label = ":" + label;
	this.x = x;
	this.y = y;
	this.callback = callback || function () { };
}

ButtonControl.prototype.draw = function () {
	var totalWidth = 100;
	if (currentText.length > 0 && this.label.indexOf(currentText) === 0) {
		draw.setColor(0x00ff00);
	} else {
		draw.setColor(0xffffff);
	}
	draw.rect(this.x, this.y, totalWidth + 2, 16 + 2);
	draw.setColor(0x101010);
	draw.rect(this.x + 1, this.y + 1, totalWidth, 16);
	draw.setColor(0xffffff);
	draw.print(this.x + 10, this.y + 4, this.label);
};

ButtonControl.prototype.submitInput = function () {
	if (this.label !== currentText) return;
	this.callback();
};

ButtonControl.prototype.submitSpecialInput = function () { };

function ProgressBar(width, step, x, y, callback) {
	this.active = false;
	this.currentWidth = 0;
	this.width = width;
	this.step = step;
	this.x = x;
	this.y = y;
	this.callback = callback || function () { };
}

ProgressBar.prototype.draw = function () {
	draw.setColor(0xffffff);
	draw.rect(this.x, this.y, this.width + 2, 16 + 2);
	if (this.active) {
		draw.setColor(0x10a010);
	} else {
		draw.setColor(0x101010);
	}
	draw.rect(this.x + 1, this.y + 1, this.currentWidth, 16);
	if (this.currentWidth > this.width) {
		this.callback();
	} else if (this.active) {
		this.currentWidth += this.step * sys.deltaTime;
	}
};

ProgressBar.prototype.submitInput = function () { };

ProgressBar.prototype.submitSpecialInput = function () { };

function addProgressBarPlusButton(label, x, y, callback) {
	var progressBar = new ProgressBar(250, 30, x + 110, y, function () {

	});
	controls.push(new ButtonControl(label, x, y, function () {
		progressBar.active = !progressBar.active;
		if (!progressBar.active) { progressBar.currentWidth = 0; }
	}));
	controls.push(progressBar);
}

addProgressBarPlusButton("a", 40, 20);
addProgressBarPlusButton("b", 40, 40);
addProgressBarPlusButton("c", 40, 60);
addProgressBarPlusButton("d", 40, 80);
addProgressBarPlusButton("e", 40, 100);
addProgressBarPlusButton("f", 40, 120);
addProgressBarPlusButton("g", 40, 140);

sys.on("draw", "controlPanel_draw", function (e) {
	controls.forEach(function (control) {
		control.draw();
	});
	draw.setColor(0x606060);
	draw.rect(20, sys.screenHeight - 40, sys.screenWidth - 40, 20);
	draw.setColor(0xffffff);
	draw.print(30, sys.screenHeight - 35, currentText.toLowerCase());
});

sys.on("input", "controlPanel_input", function (e) {
	if (e.state !== "press") return;
	if (e.key.charCodeAt(0) > 256) return;
	if (e.key.length === 1 && e.rawKey < 256) {
		var realKey = e.shift ? getSpecialChars(e.key) : e.key;
		onInput(true, realKey);
	} else {
		onInput(false, e.key);
	}
});