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
		this.ws = new WebSocket("ws://localhost:8080/", ["time"]);
		this.ws.onmessage = this.onmessage.bind(this);
		this.outbuf = []
		this.inbuf = []
		this.sendRaw("G");
	},
	handler: function() {
		if (!this.ws || this.ws.readyState != 1) {
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
})
