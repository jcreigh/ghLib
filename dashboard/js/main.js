console.log("Moo");
requirejs.config({
	appDir: ".",
	baseUrl: "js",
	paths: {
		'jquery': ['https://ajax.googleapis.com/ajax/libs/jquery/2.1.4/jquery.min'],
		'bootstrap': ['https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min'],
		'prototype': ['https://cdnjs.cloudflare.com/ajax/libs/prototype/1.7.2/prototype.min'],
		'nwt': ['nwt'],
		'ping': ['ping'],
		'time': ['time']
	},
	shim: {
		'bootstrap': ['jquery'],
		'nwt': ['prototype'],
		'ping': ['prototype'],
		'time': ['prototype']
	}
});

var nwt;

require(['jquery', 'bootstrap', 'nwt', 'ping', 'time', []], function($) {
	console.log("Loaded :)");
	nwt = new NWT();
	//p = new Ping();
	//t = new Time();
	updateStatus = function(sel, cl) {
		var ws = cl.ws;
		var s = "Uninitialized";
		if (ws) {
			s = ["Connecting", "Open", "Closing", "Connecting"][ws.readyState];
		}
		$(sel).html(s)

	}
	setInterval(function() { updateStatus("#status", nwt) }, 250);
	p = new Pinger($("#ping_result"));
	return {};
});

