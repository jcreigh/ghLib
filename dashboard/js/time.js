define(["prototype"], function() {

Time = Class.create({
	initialize: function() {
		this.connect();
		setInterval(this.handler.bind(this), 50);
		setInterval(this.connect.bind(this), 1000);
		this.epoch = 0;
	},
	connect: function() {
		if (this.ws && this.ws.readyState != 3) { // Connecting, connected, or closing
			return;
		}
		if (Ping.ready()) { // Ping has connected, so this should be good
			this.ws = new WebSocket("ws://localhost:8080/", ["time"]);
			this.ws.onmessage = this.onmessage.bind(this);
			this.outbuf = []
			this.inbuf = []
			this.sendRaw("G");
		}
	},
	ready: function() {
		return this.ws && this.ws.readyState == 1;
	},
	handler: function() {
		if (!this.ready()) {
			return;
		}
		if (this.outbuf.length > 0) {
			for (var i = 0; i < this.outbuf.length; i++) {
				this.ws.send(this.outbuf[i]);
			}
			this.outbuf = [];
		}
		if (this.inbuf.length > 0) {
			for (var i = 0; i < this.inbuf.length; i++) {
				var data = this.inbuf[i];
				if (data != "OK") {
					this.epoch = data;
					console.log("Epoch get: " + this.epoch);
				} else {
					console.log("Epoch set");
				}
			}
			this.inbuf = []
		}
	},
	sendRaw: function(data) {
		this.outbuf.push(data);
	},
	onmessage: function(event) {
		this.inbuf.push(event.data);
	},
	get: function() {
		return this.epoch;
	},
	set: function(newEpoch) {
		this.sendRaw("S" + newEpoch);
		this.sendRaw("G");
	},
	setNow: function() {
		this.set((new Date()).getTime());
	}
});

TimeSetter = Class.create({ // Keeps remote time aligned with real time
	initialize: function() {
		this.time = new Time();
		this.synced = false;
		setInterval(this.doTime.bind(this), 1000);
	},
	doTime: function() {
		if (this.time.ready()) {
			if (this.synced == false) {
				this.time.setNow();
				this.synced = true;
			}
		} else {
			this.synced = false;
		}
	}
});

});
