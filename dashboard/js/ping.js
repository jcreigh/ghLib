define(["jquery", "prototype", "chart"], function(jq) {

loadCSS("css/logger.css");

PingClass = Class.create({
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
	ready: function() {
		return this.ws && this.ws.readyState == 1;
	},
	handler: function() {
		if (!this.ready()) { // Not open yet
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

Ping = new PingClass();

Pinger = Class.create({
	initialize: function(selector){
		this.ping = Ping;
		this.selector = selector;
		this.element = jQuery(selector + " .ping_result");
		this.lastPingID = -1;
		this.histSize = 50;
		setInterval(this.doPing.bind(this), 1000);
		this.pingBuf = []
		for (i = 0; i < this.histSize; i++) {
			this.pingBuf.push(0);
		}
		this.chartElm = jQuery(selector + " .ping_chart");

		if (this.chartElm != null) {
			var data = {
				labels: [],
				datasets: [{
							label: "Ping Times",
							fillColor: "rgba(151,187,205,0.5)",
							strokeColor: "rgba(151,187,205,0.8)",
							highlightFill: "rgba(151,187,205,0.75)",
							highlightStroke: "rgba(151,187,205,1)",
							data: this.pingBuf
			}]};
			for (i = 0; i < this.histSize; i++) {
				data["labels"].push("");
			}
			var opts = { animation: false };
			this.chart = new Chart(this.chartElm[0].getContext("2d")).Bar(data, opts);
		}
	},
	pushTime: function(t) {
		this.pingBuf.shift();
		this.pingBuf.push(t);
		if (this.chart != null) {
			this.update();
		}
	},
	doPing: function() {
		if (this.lastPingID > 0 && this.ping.ready()) {
			var res = this.ping.getResult(this.lastPingID);
			if (res >= 0) {
				if (this.element != null) {
					this.element.html(res);
				}
				this.pushTime(res)
			} else if (res == -2) {
				this.pushTime(2000);
			}
			jQuery(this.selector + " > div").removeClass("panel-danger").addClass("panel-primary");
		} else {
			this.pushTime(0);
			this.element.html("n/a");
			jQuery(this.selector + " > div").removeClass("panel-primary").addClass("panel-danger");
		}
		this.lastPingID = this.ping.ping();
	},
	update: function() {
		for (i = 0; i < this.histSize; i++) {
			this.chart.datasets[0].bars[i].value = this.pingBuf[i];
		}
		this.chart.update();
	}
});

});
