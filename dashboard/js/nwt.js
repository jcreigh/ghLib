var EntryType = Object.freeze({
	"BOOLEAN": 0x00, "DOUBLE": 0x01, "STRING": 0x02, "BOOLEAN ARRAY": 0x10, "DOUBLE ARRAY": 0x11, "STRING ARRAY": 0x12,
	"properties": {
		0x00: {"name": "Boolean", "value": 0x00, "short": "B"},
		0x01: {"name": "Double", "value": 0x01, "short": "D"},
		0x02: {"name": "String", "value": 0x02, "short": "S"},
		0x10: {"name": "Boolean Array", "value": 0x10, "short": "[B]"},
		0x11: {"name": "Double Array", "value": 0x11, "short": "[D]"},
		0x12: {"name": "String Array", "value": 0x12, "short": "[S]"}
	}
});

var Entry = Class.create({
	initialize: function(name, type, value, eid, seq) {
		this.name = name;
		this.type = type;
		this.value = value;
		this.eid = eid;
		this.seq = seq;
		this.dirty = false;
		console.log("New Entry: " + name + "(" + eid + ") = " + value);
	},
	set: function(newValue) {
		this.dirty = true;
		this.value = newValue;
		if (this.eid != 0xffff) {
			this.seq++;
		}
	},
	get: function() {
		return this.value;
	}
})

var encodeEntry = function(entry) {
	var out = [];
	if (entry.eid == 0xffff) { // new Entry
		out.push(0x10);
		var ev = encodeValue(EntryType["STRING"], entry.name);
		out = out.concat(ev);
		out.push(entry.type);
		out.push(0xff); // eid is 0xffff for new entry
		out.push(0xff);
		out.push(0x00); // seq doesn't matter
		out.push(0x00);
	} else {
		out.push(0x11);
		out.push(entry.eid >> 8);
		out.push(entry.eid & 0xff);
		out.push(entry.seq >> 8);
		out.push(entry.seq & 0xff);
	}
	var ev = encodeValue(entry.type, entry.value);
	out = out.concat(ev);
	//console.log("encodeEntry: |" + entry.name + " (" + entry.eid + "): " + out)
	return out;
}

var encodeValue = function(type, value) {
	var out = [];
	if (type == EntryType["BOOLEAN"]) {
		out.push(value ? 0x01 : 0x00);
	} else if (type == EntryType["DOUBLE"]) {
		var buf = new ArrayBuffer(8);
		var flt = new Float64Array(buf);
		var u8 = new Uint8Array(buf);
		flt[0] = 0.0 + value;
		for (var i = 0; i < 8; i++) {
			out.push(u8[i]);
		}
	} else if (type == EntryType["STRING"]) {
		value = "" + value;
		out.push(value.length >> 8);
		out.push(value.length & 0xff);
		for (var i = 0; i < value.length; i++) {
			out.push(value.charCodeAt(i));
		}
	} else if (type & 0xf0 > 0) { // We're an array
		out.push(value.length);
		for (var i = 0; i < value.length; i++) {
			var ev = encodeValue(type & 0x0f, value[i]);
			out = out.concat(ev);
		}
	}
	return out;
}

var decodeValue = function(type, data) {
	var value;
	var rest;
	var valid = true;
	if (type == EntryType["BOOLEAN"] && data.length >= 1) {
		value = data[0];
		rest = data.slice(1);
	} else if (type == EntryType["DOUBLE"] && data.length >= 8) {
		var buf = new ArrayBuffer(8);
		var flt = new Float64Array(buf);
		var u8 = new Uint8Array(buf);
		for (var i = 0; i < 8; i++) {
			u8[i] = data[i];
		}
		value = flt[0];
		rest = data.slice(8);
	} else if (type == EntryType["STRING"] && data.length >= 2) {
		var len = (data[0] << 8) + data[1];
		//console.log("STRING of length: " + len + "|" + data.charCodeAt(0) + "|" + data.charCodeAt(1));
		if (data.length >= (2 + len)) {
			value = STRING.fromCharCode.apply("", data.slice(2, 2 + len));
			rest = data.slice(2 + len);
		} else {
			valid = false;
		}
	} else if (type & 0xf0 > 0 && data.length >= 1) { // We're an array
		value = [];
		var len = data[0];
		var rest = data.slice(1);
		for (var i = 0; i < len; i++) {
			var r = decodeValue(type & 0x0f, rest);
			if (!r["valid"]) {
				valid = false;
				break;
			}
			value.push(r["value"]);
			rest = r["rest"];
		}
	} else {
		console.log("Invalid: type: " + type);
		valid = false;
	}
	if (valid == false) { // Not enough data to parse
		rest = data;
	}
	return {"value": value, "rest": rest, "valid": valid}
}

NWT = Class.create({
	initialize: function() {
		this.connect();
		setInterval(this.handler.bind(this), 50);
		setInterval(this.keepAlive.bind(this), 1000);
		setInterval(this.connect.bind(this), 1000);
		this.entries = {}
	},
	connect: function() {
		if (this.ws && this.ws.readyState != 3) { // Connecting, connected, or closing
			return;
		}
		this.ws = new WebSocket("ws://localhost:8080/", ["nwtproxy"]);
		this.ws.binaryType = "arraybuffer";
		this.ws.onmessage = this.onmessage.bind(this);
		this.outbuf = [];
		this.inbuf = [];
		this.statusText = "";
		for (var n in this.entries) {
			this.entries[n].eid = 0xffff;
			this.entries[n].dirty = true;
		}
		this.sendRaw([0x01, 0x02, 0x00]); // Opening handshake
	},
	keepAlive: function() {
		if (!this.ws || this.ws.readyState != 1) {
			return;
		}
		this.sendRaw([0x00]);
	},
	handler: function() {
		if (!this.ws || this.ws.readyState != 1) {
			return;
		}
		if (this.outbuf.length > 0) {
			this.ws.send(new Uint8Array(this.outbuf));
			this.outbuf = []
		} else if (this.inbuf.length > 0) {
			var data = this.inbuf;
			//console.log("|" + data + "|");
			while (data) {
				var field = data[0];
				if (field == 0x00) { // Keep Alive
					data = data.slice(1);
				} else if (field == 0x03) { // Server Hello Complete
					console.log("Server Hello");
					data = data.slice(1);
				} else if (field == 0x02 && data.length >= 3) { // Protocol Version Unsupported
					data = data.slice(3);
				} else if (field == 0x10 || field == 0x11) { // New entry / Updated entry
					var name;
					var type = 0xff;
					var rest = data.slice(1);
					if (field == 0x10) {
						var r = decodeValue(EntryType["STRING"], rest);
						if (!r["valid"]) {
							break;
						}
						name = r["value"];
						rest = r["rest"];
						if (rest.length < 1) {
							break;
						}
						type = rest[0];
						rest = rest.slice(1);
					}
					if (rest.length < 4) { // Not enough data for type, eid, and seq
						break;
					}
					var eid = (rest[0] << 8) + rest[1];
					var seq = (rest[2] << 8) + rest[3];
					if (type == 0xff) {
						for (var n in this.entries) {
							if (this.entries[n].eid == eid) {
								type = this.entries[n].type;
								name = this.entries[n].name;
								break;
							}
						}
					}
					r = decodeValue(type, rest.slice(4));
					if (!r["valid"]) { // Not enough data for value
						break;
					}
					data = r["rest"];
					this.entries[name] = new Entry(name, type, r["value"], eid, seq);
					$(document).trigger("nwt:" + (field == 0x10 ? "new" : "update"), [name]);
				}
				break;
			}
			this.inbuf = data;
		}
		for (var n in this.entries) {
			var entry = this.entries[n];
			if (entry.dirty) {
				entry.dirty = false;
				this.sendRaw(encodeEntry(entry));
			}
		}
	},
	sendRaw: function(data) {
		this.outbuf = this.outbuf.concat(data);
	},
	onmessage: function(event) {
		var data = event.data;
		var u8 = new Uint8Array(data);
		//console.log("|" + data + "|" + u8 + "|");
		for (var i = 0; i < u8.length; i++) {
			this.inbuf.push(u8[i]);
		}
	},
	getEntry: function(name) {
		return this.entries[name];
	},
	get: function(name) {
		var entry = this.getEntry(name);
		if (!(entry === undefined)) {
			return entry.value;
		}
		return undefined;
	},
	setEntry: function(newEntry) {
		if (this.entries[newEntry.name] === undefined) {
			newEntry.eid = 0xffff;
			newEntry.dirty = true;
		}
		this.entries[newEntry.name] = newEntry;
	},
	set: function(name, value) {
		var entry = this.getEntry(name);
		if (!entry) {
			entry = new Entry(name, EntryType["STRING"] /* TODO: Detect type */ , value, 0xffff, 0);
		}
		entry.set(value);
		this.entries[name] = entry;
	}
})
