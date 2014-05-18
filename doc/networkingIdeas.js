// The networking system for "Engine2D" will be based around 2 types of communications, database sync and remote procedure call.
// Since I haven't yet given enough thought to the DBMS to have a idea of how the API will work I'll just go over how I intend RPC to work.

// Server
var net = new NetChanealHost();

events["networkTest"] = function networkTest(e) {
	if (e.isRemote)
		e.setReturnValue({msg: "Hello " + e.target});
};

events["networkTest"].setRemote(net);

net.listen();

// Client
var net = new NetChanealClient();

net.setEventNamespace("netTest");
net.connect("localhost");

events["netTest:networkTest"]({
	"target": "world"
}, function (e) {
	console.log(e.msg);
});