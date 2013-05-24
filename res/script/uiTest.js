ui.initAll();

var currentItem = ui.createLabel(10, 10, "Current Item: ");

ui.appendChildToBase(currentItem);

var textBox = ui.createTextbox(10, 40, 300);

ui.appendChildToBase(textBox);

var listBox = ui.createListbox(10, 80, 370, 500);

listBox.itemClick = function (index, value) {
	currentItem.setText("Current Item: " + value);
};

ui.appendChildToBase(listBox);

var addButton = ui.createButton(330, 40, "Add", function (x, y) {
	listBox.addItem(textBox.getText());
	textBox.setText("");
});

ui.appendChildToBase(addButton);

sys.drawFunc(function () {
	ui.draw();
});

sys.keyboardFunc(function (a, b, c) {
	ui.keyboardEvent(a, b, c);
});