// Chip 8 interpreter based on http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

var argv = sys.argv();

var running = true;
var autoCycle = true;

var program = [];

if (argv.length === 1 && argv[0].substring(argv[0].lastIndexOf(".")) !== ".ch8") {
	console.log("loading: " + argv[0]);
	program = fs.readFile(argv[0], true);
} else if (argv.length === 2) {
	if (argv[1].substring(argv[1].lastIndexOf(".")) !== ".ch8") {
		console.error("usage: script/chip-8 [.ch8 file]");
	} else {
		console.log("loading: " + argv[1]);
		program = fs.readFile(argv[1], true);
		console.log("loaded: " + argv[1] + " length:" + program.length);
	}
}

var screenWidth = 64,
	screenHeight = 32,
	screenOffset = 64,
	tileSize = 8; // 512 / 256

var drawFlag = true;

var keyCodes = ["1", "2", "3", "4",
				"5", "6", "7", "8",
				"9", "0", "Q", "W",
				"E", "R", "T", "Y"];

var mem = new Array(0xfff); // unsigned char

for (var i = 0; i < mem.length; i++) {
	mem[i] = 0;
}

var regs = new Array(16); // unsigned char

for (var i = 0; i < 16; i++) {
	regs[i] = 0;
}

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
	} else if (opcode == 0x00EE) {
		--stackPointer;
		PC = stack[stackPointer];
		PC += 2;
	} else if ((opcode & 0xF000) == 0x1000) { // 1NNN Jumps to address NNN.
		PC = opcode & 0x0FFF;
	} else if ((opcode & 0xF000) == 0x2000) { // 1NNN Jumps to address NNN.
		stack[stackPointer] = PC;
		++stackPointer;
		PC = opcode & 0x0FFF;
	} else if ((opcode & 0xF000) == 0x3000) { // (3XNN) Skips the next instruction if VX equals NN.
		var reg = (opcode & 0x0F00) >> 8;
		var value = (opcode & 0x00FF);
		PC += regs[reg] == value ? 4 : 2;
	} else if ((opcode & 0xF000) == 0x4000) { // (4XNN) Skips the next instruction if VX doesn't equal NN.
		var reg = (opcode & 0x0F00) >> 8;
		var value = (opcode & 0x00FF);
		PC += regs[reg] == value ? 2 : 4;
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
	} else if ((opcode & 0xF00F) == 0x8000) { // 8XY0 Sets VX to the value of VY.
		var reg1 = (opcode & 0x0F00) >> 8;
		var reg2 = (opcode & 0x00F0) >> 4;
		regs[reg1] = regs[reg2];
		PC += 2;
	} else if ((opcode & 0xF00F) == 0x8001) {
		var reg1 = (opcode & 0x0F00) >> 8;
		var reg2 = (opcode & 0x00F0) >> 4;
		regs[reg1] = regs[reg1] | regs[reg2];
		PC += 2;
	} else if ((opcode & 0xF00F) == 0x8002) {
		var reg1 = (opcode & 0x0F00) >> 8;
		var reg2 = (opcode & 0x00F0) >> 4;
		regs[reg1] = regs[reg1] & regs[reg2];
		PC += 2;
	} else if ((opcode & 0xF00F) == 0x8004) {
		if(regs[(opcode & 0x00F0) >> 4] > (0xFF - regs[(opcode & 0x0F00) >> 8]))
			regs[0xF] = 1; //carry
		else
			regs[0xF] = 0;
		regs[(opcode & 0x0F00) >> 8] += regs[(opcode & 0x00F0) >> 4];
		PC += 2;
	} else if ((opcode & 0xF000) == 0xA000) { // Sets I to the address NNN.
		I = (opcode & 0x0FFF);
		PC += 2;
	} else if ((opcode & 0xF000) == 0xC000) { // (4XNN) Skips the next instruction if VX doesn't equal NN.
		var reg = (opcode & 0x0F00) >> 8;
		var value = (opcode & 0x00FF);
		regs[reg] = Math.floor(Math.random() * 255) & value;
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
	} else if ((opcode & 0xF0FF) == 0xE09E) { // EX9E Skips the next instruction if the key stored in VX is pressed.
		var reg = (opcode & 0x0F00) >> 8;
		PC += key[reg] ? 4 : 2;
	} else if ((opcode & 0xF0FF) == 0xE0A1) { // EXA1 Skips the next instruction if the key stored in VX isn't pressed.
		var reg = (opcode & 0x0F00) >> 8;
		PC += key[reg] ? 2 : 4;
	} else if ((opcode & 0xF0FF) == 0xF007) { // FX07 Sets VX to the value of the delay timer.
		var reg = regs[(opcode & 0x0F00) >> 8];
		regs[reg] = delayTimer;
		console.log("delay: " + delayTimer + " : reg: " + reg);
		PC += 2;
	} else if ((opcode & 0xF0FF) == 0xF015) { // FX15 Sets the delay timer to VX.
		var reg = regs[(opcode & 0x0F00) >> 8];
		delayTimer = regs[reg];
		PC += 2;
	} else if ((opcode & 0xF0FF) == 0xF018) { // FX18 Sets the sound timer to VX.
		// no sound support, ignored
		PC += 2;
	} else if ((opcode & 0xF0FF) == 0xF01E) { // Adds VX to I.
		var reg = regs[(opcode & 0x0F00) >> 8];
		I += regs[reg];
		PC += 2;
	} else if ((opcode & 0xF0FF) == 0xF029) {
		// no text support yet, ignored
		PC += 2;
	} else if ((opcode & 0xF0FF) == 0xF033) {
		mem[I]     = regs[(opcode & 0x0F00) >> 8] / 100;
		mem[I + 1] = (regs[(opcode & 0x0F00) >> 8] / 10) % 10;
		mem[I + 2] = (regs[(opcode & 0x0F00) >> 8] % 100) % 10;
		PC += 2;
	} else if ((opcode & 0xF0FF) == 0xF055) {
		var endCode = regs[(opcode & 0x0F00) >> 8];
		var memStart = I;
		for (var i = 0; i < endCode; i++) {
			mem[memStart++] = regs[i];
		}
		PC += 2;
	} else if ((opcode & 0xF0FF) == 0xF065) {
		var endCode = regs[(opcode & 0x0F00) >> 8];
		var memStart = I;
		for (var i = 0; i < endCode; i++) {
			regs[i] = mem[memStart++];
		}
		PC += 2;
	} else {
		console.error("Invalid Opcode: " + (opcode).toString(16) + " at " + PC.toString(16));
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

	try {
		var debuggingWidthOffset = (screenWidth * tileSize) + (screenOffset * 2);
		var debuggingHeight = screenOffset;

		draw.print(debuggingWidthOffset, debuggingHeight, "PC: 0x" + PC.toString(16));
		debuggingHeight += 12;
		draw.print(debuggingWidthOffset, debuggingHeight, "I: 0x" + I.toString(16));
		debuggingHeight += 12;
		//draw.print(debuggingWidthOffset, debuggingHeight, "Delay: " + delayTimer.toString(10));
		//debuggingHeight += 12;

		for (var i = 0; i < 16; i++) {
			draw.print(debuggingWidthOffset, debuggingHeight, "V" + i + ": 0x" + regs[i].toString(16));
			debuggingHeight += 12;
		}
	} catch (e) {
		console.log(e);
		sys.clearEvent("chip-8.draw");
	}

	if (autoCycle) {
		runCycle();
	}

	for (var x = 0; x < screenWidth; x++) {
		for (var y = 0; y < screenHeight; y++) {
			if (gfx[x + ((y) * screenWidth)]) {
				draw.rect(screenOffset + (x * tileSize), screenOffset + (y * tileSize), tileSize, tileSize);
			}
		}
	}

	updateKeys();
});