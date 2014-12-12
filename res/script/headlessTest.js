console.log("In Headless Mode");

var loopCounter = 0;

event.headlessExit_test = function headlessExit_test(e) {
	console.log("Looped for: " + loopCounter);
	event.exit();
};

sys.createTimer(1, "headlessExit_test");

event.headlessLoop = function headlessTest_loop(e) {
	loopCounter++;
};