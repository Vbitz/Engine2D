var TILE_SIZE = 8;

var renderers = [
	function empty_block (x, y) {
		//draw.setColor(0x101010);
		//draw.rect(x, y, TILE_SIZE, TILE_SIZE);
	},
	function wall_block (x, y) {
		draw.setColor(0xa0a0a0);
		draw.rect(x, y, TILE_SIZE, TILE_SIZE);	
	},
	function floor_block (x, y) {
		draw.setColor(0xe0e0e0);
		draw.rect(x, y, TILE_SIZE, TILE_SIZE);	
	}
];

function Block(w, h, doors, gen) {
	this.width = w;
	this.height = h;
	this.doors = doors;
	this.build = gen;
}

var blocks = {
	"basicHallX": new Block(8, 8, {"left": true, "right": true}, function (x, y) {
		for (var i = 0; i < 8; i++) {
			this.placeBlock(1, i, 2);
			for (var i2 = 3; i2 < 6; i2++) {
				this.placeBlock(2, i, i2);
			}
			this.placeBlock(1, i, 6);
		}
	}),

	"basicHallY": new Block(8, 8, {"foward": true, "backward": true}, function (x, y) {
		for (var i = 0; i < 8; i++) {
			this.placeBlock(1, 2, i);
			for (var i2 = 3; i2 < 6; i2++) {
				this.placeBlock(2, i2, i);
			}
			this.placeBlock(1, 6, i);
		}
	}),

	"basicRoom": new Block(8, 8, {"foward": true, })
};

function Map(w, h) {
	this.width = w;
	this.height = h;
	this._map = [];
	this.clear();
}

Map.prototype.clear = function () {
	this._map = [];
	for (var x = 0; x < this.width; x++) {
		this._map[x] = [];
		for (var y = 0; y < this.height; y++) {
			this._map[x][y] = 0;
		}
	}
};

Map.prototype.draw = function (offX, offY) {
	draw.cameraPan(offX, offY);
	for (var x = 0; x < this.width; x++) {
		for (var y = 0; y < this.height; y++) {
			var value = this._map[x][y];
			renderers[value](x * TILE_SIZE, y * TILE_SIZE);
		}
	}
};

var map = map || new Map(64, 64);

function generate() {
	map.clear();
}

generate();

event.draw = function mapGenerator_draw(e) {
	map.draw(40, 40);
};

event.input = function mapGenerator_input(e) {

};