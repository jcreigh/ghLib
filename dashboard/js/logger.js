define(["jquery", "prototype"], function(jq) {

var LogLevel = Object.freeze({
	"TRACE": 1, "DEBUG": 2, "INFO": 3, "WARN": 4, "ERROR": 5, "FATAL": 6, "DISABLED": 7,
	"properties": {
		1: {"name": "TRACE", "value": 1},
		2: {"name": "DEBUG", "value": 2},
		3: {"name": "INFO", "value": 3},
		4: {"name": "WARN", "value": 4},
		5: {"name": "ERROR", "value": 5},
		6: {"name": "FATAL", "value": 6},
		7: {"name": "DISABLED", "value": 7}
	}
});

LogEntry = Class.create({
	initialize: function(data) {
		this.level = data["level"];
		this.text = data["text"];
		this.baseLogger = data["logger"];
		this.time = data["time"];
	}
});

Logger = Class.create({
	initialize: function(selector) {
		this.selector = selector;
		jQuery(window).resize(this.resize.bind(this));
		var elm = jQuery(this.selector + " tbody");
		elm.scroll(this.scroll.bind(this));
		elm.data("Scroll_AtBottom", true);
		jQuery(this.selector + " .logger-level").change(this.levelChangeHandler.bind(this));
		this.logs = [];
		setInterval(this.handler.bind(this), 1000);
		setInterval(this.connect.bind(this), 100);
		setInterval(this.checkVisible.bind(this), 100);
		this.connect();

	},
	connect: function() {
		if (this.ws && this.ws.readyState != 3) { // Connecting, connected, or closing
			return;
		}
		// Closed or doesn't exist
		if (Ping.ready()) { // Ping has connected, so this should be good
			this.ws = new WebSocket("ws://localhost:8080/", ["logger"]);
			this.ws.onmessage = this.onmessage.bind(this);
			this.logs = [];
			jQuery(this.selector + " tbody tr").remove();
		}
	},
	ready: function() {
		return this.ws && this.ws.readyState == 1;
	},
	handler: function() {
		if (!this.ready()) { // Not open yet
			jQuery(this.selector + " > div").removeClass("panel-primary").addClass("panel-danger");
			return
		}
		jQuery(this.selector + " > div").removeClass("panel-danger").addClass("panel-primary");
		this.ws.send("G," + (this.logs.length) + "\n");
	},
	onmessage: function(event) {
		var data = JSON.parse(event.data);
		for (var i = 0; i < data["count"]; i++) {
			var newLog = new LogEntry(data["entries"][i]);
			this.logs[data["start"] + i] = newLog;
			//console.log("|" + (elm.scrollTop() + elm.innerHeight()) + "|" + elm[0].scrollHeight + "|");

			var elm = jQuery(this.selector + " tbody");
			var newTR = jQuery("<tr><td>" + newLog.time + "</td><td>" + newLog.level+ "</td><td>" + newLog.baseLogger + "</td><td>" +newLog.text + "</td></tr>");
			if (LogLevel[newLog.level] < LogLevel[this.getSelectedLevel()]) {
				newTR.hide();
			}
			elm.append(newTR);
			if (elm.data("Scroll_AtBottom")) {
				elm.scrollTop(elm[0].scrollHeight);
			}
			this.resize();

		}
	},
	resize: function(e) { // This is silly
		var table = jQuery(this.selector + " table");
		var tr = jQuery(table).find("tbody tr:visible:first");
		var cells = jQuery(tr).children().slice(0, 3);
		var w = 0;
		for (i = 0; i < 3; i++) {
			w += jQuery(cells[i]).outerWidth();
		}
		var newWidth = tr.width() - w - scrollbarWidth() - 1;
		table.find("tr").each(function (i, v) {
			jQuery(v).children().last().width(newWidth);
		});
	},
	scroll: function(e) {
		var elm = jQuery(this.selector + " tbody");
		elm.data("Scroll_AtBottom", elm.scrollTop() + elm.innerHeight() >= elm[0].scrollHeight - 20);
	},
	checkVisible: function() { // Not great, but there's no event to catch
		var elm = jQuery(this.selector + " tbody");
		if (this.hidden === undefined) {
			this.hidden = !elm.is(":visible");
			return
		}
		if (this.hidden && elm.is(":visible")) {
			// Was hidden, now isn't
			if (elm.data("Scroll_AtBottom")) {
				elm.scrollTop(elm[0].scrollHeight);
				this.resize();
			}
		}
	},
	getSelectedLevel: function() {
		return jQuery(this.selector + " .logger-level").val();
	},
	levelChangeHandler: function(e) {
		var newValue = this.getSelectedLevel();
		var trs = jQuery(this.selector + " tbody tr");
		for (var i = 0; i < this.logs.length; i++) {
			if (LogLevel[this.logs[i].level] < LogLevel[newValue]) {
				trs[i].hide();
			} else {
				trs[i].show();
			}
		}
	}

});

// http://stackoverflow.com/a/9908849
scrollbarWidth = function() {
	var parent, child, width;
	if (width === undefined) {
		parent = jQuery('<div style="width:50px;height:50px;overflow:auto"><div/></div>').appendTo('body');
		child = parent.children();
		width = child.innerWidth() - child.height(99).innerWidth();
		parent.remove();
	}

	return width;
};


});
