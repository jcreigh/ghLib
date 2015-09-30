Ping = Class.create({
	initialize: function() {
		this.pings = {}
		this.doPing = 0;
		setInterval(this.handler.bind(this), 250);
		setInterval(this.connect.bind(this), 1000);
		this.connect();
	},
	connect: function() {
		if (this.ws && this.ws.readyState != 3) { // Connecting, connected, or closing
			return;
		}
		// Closed or doesn't exist
		this.ws = new WebSocket("ws://localhost:8080/", ["ping"]);
		this.ws.onmessage = this.onmessage.bind(this);
	},
	handler: function() {
		if (!this.ws || this.ws.readyState != 1) { // Not open yet
			return
		}
		for (var r in this.pings) {
			if (this.pings[r]["done"] == -2) { // Not sent yet
				this.pings[r]["done"] = -1;
				var curTime = (new Date()).getTime();
				if (curTime - this.pings[r]["ts"] < 2000) {
					this.pings[r]["ts"] = curTime;
					this.ws.send(r);
				}
			}
		}
	},
	onmessage: function(event) {
		var data = event.data;
		this.pings[data]["done"] = (new Date()).getTime() - this.pings[data]["ts"];
	},
	ping: function() {
		var rand = Math.floor(Math.random() * 100000000);
		this.pings[rand] = {"done": -2, "ts": (new Date()).getTime()};
		return rand;
	},
	getResult: function(r) {
		if (this.pings[r]["done"] < 0) {
			var diff = (new Date()).getTime() - this.pings[r]["ts"];
			if (this.pings["done"] == -1 && diff > 2000) {
				return -2; // Timeout, has taken more than 2 seconds
			}
			return -1; // Haven't heard back yet
		}
		return this.pings[r]["done"];
	}
});

Pinger = Class.create({
	initialize: function(elm) {
		this.ping = new Ping();
		this.element = elm;
		this.lastPingID = -1;
		setInterval(this.doPing.bind(this), 2000);
	},
	doPing: function() {
		if (this.lastPingID > 0 && this.ping.ws && this.ping.ws.readyState == 1) {
			var res = this.ping.getResult(this.lastPingID);
			if (res >= 0) {
				this.element.html(res);
			}
		} else {
			this.element.html("n/a");
		}
		this.lastPingID = this.ping.ping();
	}
});
