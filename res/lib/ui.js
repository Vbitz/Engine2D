var ui = ui ? ui : {};

ui._insideRect = function (rectX, rectY, rectW, rectH, x, y) {
	return x > rectX && x < rectX + rectW && y > rectY && y < rectY + rectH;
};

ui.mouseDownLastFrame = false;

ui.currentControls = ui.currentControls ? ui.currentControls : [];

ui.createControl = function () {
	var control = {
		id: "",
		_draw: function () {
			this._checkInput();

			this.draw();

			for (var i = 0; i < this.children.length; i++) {
				this.children[i]._draw();
			}
		},
		_checkInput: function () {
			if (ui._insideRect(this.style.left, this.style.top, this.style.width, this.style.height,
				input.mouseX, input.mouseY)) {
				this.mouseHover(input.mouseX, input.mouseY);
				this.hovering = true;

				if (input.leftMouseButton && !ui.mouseDownLastFrame) {
					this.mouseDown("left", input.mouseX, input.mouseY);
				}
			} else {
				this.hovering = false;
			}
		},
		draw: function () { },
		mouseHover: function (x, y) { },
		mouseDown: function (button, x, y) { },
		keyDown: function (type, chr) { },
		style: {
			left: 0,
			top: 0,
			width: 0,
			height: 0
		},
		children: []
	};
	ui.currentControls.push(control);
	return control;
};

ui.initAll = function () {
	ui.currentControls = [];
	ui.base = ui.createControl();
	ui.base._checkInput = function () {
		if (input.leftMouseButton && !ui.mouseDownLastFrame) {
			ui.currentInput = null;
		}
	};
	ui.currentInput = null;
};

ui.appendChildToBase = function (child) {
	ui.base.children.push(child);
};

ui.createLabel = function (x, y, text) {
	var label = ui.createControl();
	label.style.color = 0xffffff;

	label.setText = function (text) {
		this.innerText = text;
		this.style.width = draw.getTextWidth(text);
		this.style.height = 16;
	};
	label.setText(text);

	label.getText = function () {
		return this.innerText;
	};

	label.addPosition = function (x, y) {
		this.style.left += x;
		this.style.top += y;
		for (var i = 0; i < this.children.length; i++) {
			this.children[i].addPosition(x, y);
		}
	};

	label.setPosition = function (x, y) {
		var diffX = x - this.style.left;
		var diffY = y - this.style.top;
		this.addPosition(diffX, diffY);
	};
	label.setPosition(x, y);

	label.draw = function () {
		draw.setColor(this.style.color);
		draw.print(this.style.left, this.style.top, this.innerText);
	};

	return label;
};

ui.createButton = function (x, y, text, callback) {
	var button = ui.createLabel(x, y, text);
	button.style.backgroundColor = 0xffffff;
	button.style.hoverColor = 0xeeeeee;
	button.style.clickColor = 0xcccccc;
	button.style.color = 0x000000;

	button.setText = function (text) {
		this.innerText = text;
		this.style.width = draw.getTextWidth(text + 10);
		this.style.height = 26;
	};
	button.setText(text);

	if (callback) {
		button.mouseDown = callback;
	}

	button.draw = function () {
		if (input.leftMouseButton && this.hovering) {
			draw.setColor(this.style.clickColor);
		} else if (this.hovering) {
			draw.setColor(this.style.hoverColor);
		} else {
			draw.setColor(this.style.backgroundColor);
		}

		draw.rect(this.style.left, this.style.top, this.style.width, this.style.height);
		draw.setColor(this.style.color);
		draw.print(this.style.left + 5, this.style.top + 5, this.innerText);
	};

	return button;
};

ui.createTextbox = function (x, y, width) {
	var textbox = ui.createButton(x, y, "");
	textbox.style.width = width + 10;
	textbox.style.maxLength = Math.floor(width / 10);
	textbox.style.height = 26;

	textbox.keyDown = function (type, chr) {
		if (type === "char") {
			if (this.getText().length < this.style.maxLength) {
				this.setText(this.getText() + chr);
				this.textChanged();
			}
		} else {
			if (chr === "BACKSPACE") {
				var currentText = this.getText();
				if (currentText.length > 0) {
					this.setText(currentText.substring(0, currentText.length - 1));
				}
			} else {
				sys.println(chr);
			}
		}
	};

	textbox.setText = function (text) {
		this.innerText = text;
	};

	textbox.textChanged = function () {};

	textbox.mouseDown = function () {
		ui.currentInput = this;
	};

	textbox.draw = function () {
		if (input.leftMouseButton && this.hovering) {
			draw.setColor(this.style.clickColor);
		} else if (this.hovering && (ui.currentInput === this)) {
			draw.setColor(this.style.hoverColor);
		} else {
			draw.setColor(this.style.backgroundColor);
		}

		draw.rect(this.style.left, this.style.top, this.style.width, this.style.height);
		draw.setColor(this.style.color);
		draw.print(this.style.left + 5, this.style.top + 5, this.innerText);
	};

	return textbox;
};

ui.createListbox = function (x, y, width, height) {
	var listbox = ui.createButton(x, y, "");
	listbox.style.width = width;
	listbox.style.height = height;
	listbox.style.itemBackground = listbox.style.hoverColor;
	listbox.items = [];

	listbox.addItem = function (item) {
		this.items.push(item.toString());
	};

	listbox.itemClick = function (item, value) {};

	listbox.mouseDown = function (button, x, y) {
		var index = Math.floor((y - this.style.top) / 24);
		this.itemClick(index, this.items[index]);
	};

	listbox.draw = function () {
		draw.setColor(this.style.backgroundColor);
		draw.rect(this.style.left, this.style.top, this.style.width, this.style.height);

		for (var i = 0; i < this.items.length; i++) {
			draw.setColor(this.style.itemBackground);
			draw.rect(this.style.left + 2, this.style.top + (i * 24), this.style.width - 4, 20);
			draw.setColor(this.style.color);
			draw.print(this.style.left + 5, this.style.top + (i * 24) + 4, this.items[i]);
		}
	};

	return listbox;
};

ui.draw = function () {
	ui.base._draw();
	ui.mouseDownLastFrame = input.leftMouseButton;
};

ui.keyboardEvent = function (type, chr, press) {
	if (ui.currentInput !== null && press) {
		ui.currentInput.keyDown(type, chr);
	}
};