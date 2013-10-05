// wow this is really slow if you don't use the fast way

var IMAGE_WIDTH = sys.screenWidth === undefined ? 512 : sys.screenWidth;
var IMAGE_HEIGHT = sys.screenHeight === undefined ? 512 : sys.screenHeight;
var SCALE = 4;
IMAGE_WIDTH = IMAGE_WIDTH / SCALE;
IMAGE_HEIGHT = IMAGE_HEIGHT / SCALE;

function setXY(array, x, y, z, value) {
	var index = 0;
	index += x * IMAGE_HEIGHT * 4;
	index += y * 4;
	index += z;
	array[index] = value;
}

// from here is basicly just https://code.google.com/p/simplexnoise/source/browse/trunk/SimplexNoise/Noise.cs
// converted to JS
// Thanks to heikki.to4

var F2 = 0.366025403; // F2 = 0.5*(sqrt(3.0)-1.0)
var G2 = 0.211324865; // G2 = (3.0-Math.sqrt(3.0))/6.0

var perm = (function () {
	var arr = [];
	for (var i = 0; i < 512; i++) {
		arr[i] = Math.floor(Math.random() * 256);
	}
	return arr;
})();

function grad(hash, x, y) {
	var h = hash & 7;      // Convert low 3 bits of hash code
	var u = h<4 ? x : y;  // into 8 simple gradient directions,
	var v = h<4 ? y : x;  // and compute the dot product with (x,y).
	return ((h&1) != 0 ? -u : u) + ((h&2) != 0 ? -2.0*v : 2.0*v);
}

function perlin(x, y, scale) {
	var n0, n1, n2; // Noise contributions from the three corners

	// Skew the input space to determine which simplex cell we're in
	var s = (x+y)*F2; // Hairy factor for 2D
	var xs = x + s;
	var ys = y + s;
	var i = Math.floor(xs);
	var j = Math.floor(ys);

	var t = (i+j)*G2;
	var X0 = i-t; // Unskew the cell origin back to (x,y) space
	var Y0 = j-t;
	var x0 = x-X0; // The x,y distances from the cell origin
	var y0 = y-Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	var i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0>y0) {i1=1; j1=0;} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else {i1=0; j1=1;}      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	var x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	var y1 = y0 - j1 + G2;
	var x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
	var y2 = y0 - 1.0 + 2.0 * G2;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	var ii = i % 256;
	var jj = j % 256;

	// Calculate the contribution from the three corners
	var t0 = 0.5 - x0 * x0 - y0 * y0;
	if(t0 < 0.0) n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad(perm[ii + perm[jj]], x0, y0); 
	}

	var t1 = 0.5 - x1*x1-y1*y1;
	if(t1 < 0.0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad(perm[ii + i1 +perm[jj + j1]], x1, y1);
	}

	var t2 = 0.5 - x2*x2-y2*y2;
	if(t2 < 0.0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad(perm[ii+  1 + perm[jj + 1]], x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return scale * (n0 + n1 + n2); // TODO: The scale factor is preliminary!
}

function noise(arr, level, width, height) {
	sys.perf("noiseGen", function () {
		for (var x = 0; x < width; x++) {
			for (var y = 0; y < height; y++) {
				val = perlin(x, y, SCALE * 10);
				setXY(arr, x, y, 0, perlin(x, y, Math.floor(Math.random() * 255)));
				setXY(arr, x, y, 1, perlin(x, y, Math.floor(Math.random() * 255)));
				setXY(arr, x, y, 2, perlin(x, y, Math.floor(Math.random() * 255)));
				setXY(arr, x, y, 3, 1.0);
			}
		}
	});
	return arr;
}

var noiseField = draw.createImageArray(IMAGE_WIDTH, IMAGE_HEIGHT);
noiseField = noise(noiseField, 512, IMAGE_WIDTH, IMAGE_HEIGHT);

var img = draw.createImage(noiseField, IMAGE_WIDTH, IMAGE_HEIGHT, img === undefined ? -5 : img);

sys.drawFunc(function () {
	draw.setColor("white");
	draw.draw(img, 0, 0, IMAGE_WIDTH * SCALE, IMAGE_HEIGHT * SCALE);
});

sys.keyboardFunc(function (a, k, p) {
	if (!p) return;
	if (k === "R") sys.forceReload("script/noise.js");
});