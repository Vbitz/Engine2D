// Config Variables

var startWidth =		1000 * 1.2,
	startHeight =		600 * 1.2;

var borderMin =			2,
	borderMax =			2;

var widthMin =			100,
	heightMin =			100;

var widthMax =			startWidth / 2.5,
	heightMax =			startHeight / 2.5;

var topMin =			borderMin,
	topMax =			borderMax,
	bottomMin =			borderMax,
	bottomMax =			borderMax * 2,
	leftMin =			borderMin,
	leftMax =			borderMax,
	rightMin =			borderMin,
	rightMax =			borderMax * 2;

var minFactor =			0.3,
	maxFactor =			0.7;

var itMax =				30;

var minRooms =			20;

var hideAmount =		2;

// DO not edit below this line if you don't know what your doing

var nodes =				null;

var lastMicrotime =		0;

function rand(min, max) {
	return Math.floor(Math.random() * (max - min)) + min;
}

function _genTree() {
	nodes = [
		{
			x: 0,
			y: 0,
			width: startWidth,
			height: startHeight,
			//color: Math.floor(Math.random() * 255 * 255 * 255),
			sub: true,
			show: true,
			top: 10,
			bottom: 10,
			left: 10,
			right: 10
		}
	];
	for (var it = 0; it < itMax; it++) {
		var newNodes = [];
		for (var i = 0; i < nodes.length; i++) {
			if (nodes[i].sub) {
				if (rand(0, 2) === 0) {
					var split = rand(nodes[i].width * minFactor, nodes[i].width * maxFactor);
					newNodes.push({
						x: nodes[i].x,
						y: nodes[i].y,
						width: nodes[i].width - split,
						height: nodes[i].height,
						//color: Math.floor(Math.random() * 255 * 255 * 255),
						sub: true,
						show: true,
						top: rand(topMin, topMax),
						bottom: rand(bottomMin, bottomMax),
						left: rand(leftMin, leftMax),
						right: rand(rightMin, rightMax)
					});
					newNodes.push({
						x: nodes[i].x + (nodes[i].width - split),
						y: nodes[i].y,
						width: split,
						height: nodes[i].height,
						color: Math.floor(Math.random() * 255 * 255 * 255),
						sub: true,
						show: true,
						top: rand(topMin, topMax),
						bottom: rand(bottomMin, bottomMax),
						left: rand(leftMin, leftMax),
						right: rand(rightMin, rightMax)
					});
				} else {
					var split = rand(nodes[i].height * minFactor, nodes[i].height * maxFactor);
					newNodes.push({
						x: nodes[i].x,
						y: nodes[i].y,
						width: nodes[i].width,
						height: nodes[i].height - split,
						//color: Math.floor(Math.random() * 255 * 255 * 255),
						sub: true,
						show: true,
						top: rand(topMin, topMax),
						bottom: rand(bottomMin, bottomMax),
						left: rand(leftMin, leftMax),
						right: rand(rightMin, rightMax)
					});
					newNodes.push({
						x: nodes[i].x,
						y: nodes[i].y + (nodes[i].height - split),
						width: nodes[i].width,
						height: split,
						color: Math.floor(Math.random() * 255 * 255 * 255),
						sub: true,
						show: true,
						top: rand(topMin, topMax),
						bottom: rand(bottomMin, bottomMax),
						left: rand(leftMin, leftMax),
						right: rand(rightMin, rightMax)
					});
				}
			} else {
				newNodes.push(nodes[i]);
			}
		}
		for (x = 0; x < newNodes.length; x++) {
			if (newNodes[x].width < widthMin || newNodes[x].height < heightMin) {
				newNodes[x].sub = false;
			}
		}
		nodes = newNodes;
	}
	for (var y = 0; y < nodes.length; y++) {
		if (rand(0, hideAmount) === 0) {
			nodes[y].show = false;
		}
		if (nodes[y].width >= widthMax || nodes[y].height >= heightMax) {
			return false;
		}
	}
	if (nodes.length < minRooms) {
		return false;
	}
	return true;
}

function genTree() {
	while (!_genTree()) { }
}

genTree();

console.log(sys.hasExtention("GL_ARB_fragment_program"));

var r = 0;

sys.drawFunc(function () {
	draw.cameraPan(sys.screenWidth / 2, sys.screenHeight / 2);
	draw.setCenter(sys.screenWidth / 2, sys.screenHeight / 2);
	draw.cameraZoom(1);
	draw.cameraRotate(r++);
	for (var i = 0; i < nodes.length; i++) {
		if (nodes[i].show) {
			if (nodes[i].color) {
				draw.setColor(nodes[i].color);
			} else {
				draw.setColor(0xffffff);
			}
			draw.rect(nodes[i].x + nodes[i].left, nodes[i].y + nodes[i].left,
				nodes[i].width - nodes[i].right, nodes[i].height - nodes[i].bottom);
		}
	}
	draw.cameraReset();
	draw.setCenter(0, 0);
	draw.cameraZoom(0.8);
	draw.setColor(0x000000);
	draw.rect(0, 0, 360, 110);
	draw.setColor(0xffffff);
	draw.print(10, 10, "Verts: " + draw.getVerts());
	var currentMicrotime = sys.microtime();
	draw.print(10, 40, "MicroTime: " + (currentMicrotime - lastMicrotime).toFixed(4));
	var heapStats = sys.heapStats();
	draw.print(10, 70, "Heap Usage: " + (heapStats.heapUsed / heapStats.heapTotalSize * 100).toFixed(2));
	lastMicrotime = currentMicrotime;
});

sys.keyboardFunc(function (type, chr, press) {
	if (press && chr === 'r') {
		genTree();
	}
});