// Chip 8 interpreter based on http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

// Now for something more complex
var program = [
	// program encoded as a C style array goes here xxd -i works great for this
];

var screenWidth = 64,
	screenHeight = 32,
	screenOffset = 64,
	tileSize = 8; // 512 / 256

var running = true;

var drawFlag = true;

var keyCodes = ["1", "2", "3", "4",
				"5", "6", "7", "8",
				"9", "0", "Q", "W",
				"E", "R", "T", "Y"];

var mem = new Array(0xfff); // unsigned char

var regs = new Array(16); // unsigned char

var I = 0; // unsigned short
var PC = 0x200; // unsigned short

var gfx = new Array(screenWidth * screenHeight); // bool

var delayTimer = 0, // unsigned char
	soundTimer = 0; // unsigned char

var stack = new Array(16); // unsigned short
var stackPointer = 0;

var key = new Array(16); // bool

// clear screen
function clearScreen() {
	for (var x = 0; x < screenWidth; x++) {
		for (var y = 0; y < screenHeight; y++) {
			gfx[x * screenHeight + y] = false;
		}
	}
}
clearScreen();

// load program into memory
for (var i = 0; i < program.length; i++) {
	mem[i + 0x200] = program[i]; 
}

function runCycle() {
	if (!running) return;

	var opcode = mem[PC] << 8 | mem[PC + 1];

	if (opcode == 0x00E0) { // clear screen
		clearScreen();
		PC += 2;
	} else if ((opcode & 0xF000) == 0x1000) { // 1NNN Jumps to address NNN.
		PC = opcode & 0x0FFF;
	} else if ((opcode & 0xF000) == 0x3000) { // (3XNN) Skips the next instruction if VX equals NN.
		var reg = (opcode & 0x0F00) >> 8;
		var value = (opcode & 0x00FF);
		PC += regs[reg] == value ? 4 : 2;
	} else if ((opcode & 0xF000) == 0x6000) { // (6XNN) Sets VX to NN
		var reg = (opcode & 0x0F00) >> 8;
		var value = (opcode & 0x00FF);
		regs[reg] = value;
		PC += 2;
	} else if ((opcode & 0xF000) == 0x7000) { // (7XNN) Adds NN to VX.
		var reg = (opcode & 0x0F00) >> 8;
		var value = (opcode & 0x00FF);
		regs[reg] += value;
		PC += 2;
	} else if ((opcode & 0xF000) == 0xA000) { // Sets I to the address NNN.
		I = (opcode & 0x0FFF);
		PC += 2;
	} else if ((opcode & 0xF000) == 0xD000) { // (DXYN) IT DRAWS
		var x = regs[(opcode & 0x0F00) >> 8];
		var y = regs[(opcode & 0x00F0) >> 4];
		var height = opcode & 0x000F;
		var pixel;

		regs[0xF] = 0;
		for (var yline = 0; yline < height; yline++)
		{
			pixel = mem[I + yline];
			for(var xline = 0; xline < 8; xline++)
			{
				if((pixel & (0x80 >> xline)) != 0)
				{
					if(gfx[(x + xline + ((y + yline) * screenWidth))] === true)
						regs[0xF] = 1;
					gfx[x + xline + ((y + yline) * screenWidth)] ^= 1;
				}
			}
		}
		PC += 2;
	} else {
		console.error("Invalid Opcode: " + (opcode).toString(16));
		running = false;
	}

	if (delayTimer > 0) {
		--delayTimer;
	}

	if (soundTimer > 0) {
		if (soundTimer == 1) {
			console.log("BEEP!!!");
		}
		--soundTimer;
	}
}

function updateKeys() {
	for (var i = 0; i < keyCodes.length; i++) {
		key[i] = input.keyDown(keyCodes[i]);
	}
}

sys.clearEvent("chip-8.draw");
sys.on("draw", "chip-8.draw", function () {
	// TODO: draw debugging info

	runCycle();

	for (var x = 0; x < screenWidth; x++) {
		for (var y = 0; y < screenHeight; y++) {
			if (gfx[x + ((y) * screenWidth)]) {
				draw.rect(screenOffset + (x * tileSize), screenOffset + (y * tileSize), tileSize, tileSize);
			}
		}
	}

	updateKeys();
});