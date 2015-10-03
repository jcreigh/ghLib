function loadCSS(url) {
    var link = document.createElement("link");
    link.type = "text/css";
    link.rel = "stylesheet";
    link.href = url;
    document.getElementsByTagName("head")[0].appendChild(link);
}

requirejs.config({
	appDir: ".",
	baseUrl: "js",
	paths: {
		'jquery': ['https://ajax.googleapis.com/ajax/libs/jquery/2.1.4/jquery.min'],
		'bootstrap': ['https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min'],
		'prototype': ['https://cdnjs.cloudflare.com/ajax/libs/prototype/1.7.2/prototype.min'],
		'chart': ['https://cdnjs.cloudflare.com/ajax/libs/Chart.js/1.0.2/Chart.min'],
		'ping': ['ping'],
		'nwt': ['nwt'],
		'time': ['time'],
		'logger': ['logger']
	},
	shim: {
		//'prototype': { exports: 'Prototype'},
		'bootstrap': ['jquery']
		//'nwt': ['prototype', 'ping'],
		//'ping': ['prototype', 'chart', 'jquery'],
		//'time': ['prototype', 'ping'],
		//'logger': ['prototype', 'ping']
	},
	map: {
		"*": { "jquery": "jq_noconflict" },
		"jq_noconflict": { "jquery": "jquery" }
	}
});
define('jq_noconflict', ["jquery"], function(jq) {
	return jq.noConflict();
});

require(['jquery', 'bootstrap', 'ping', 'time', 'logger'], function($) {
	console.log("Loaded :)");

	// http://jsfiddle.net/dgervalle/e8Apv/ Silly Prototypejs and jquery conflicts
	if (Prototype.BrowserFeatures.ElementExtensions) {
			require(['jquery', 'bootstrap'], function ($) {
					// Fix incompatibilities between BootStrap and Prototype
					var disablePrototypeJS = function (method, pluginsToDisable) {
									var handler = function (event) {
											event.target[method] = undefined;
											setTimeout(function () {
													delete event.target[method];
											}, 0);
									};
									pluginsToDisable.each(function (plugin) {
											$(window).on(method + '.bs.' + plugin, handler);
									});
							},
							pluginsToDisable = ['collapse', 'dropdown', 'modal', 'tooltip', 'popover', 'tab'];
					disablePrototypeJS('show', pluginsToDisable);
					disablePrototypeJS('hide', pluginsToDisable);
			});
	}

	////p = new Ping();
	//updateStatus = function(sel, cl) {
		//var ws = cl.ws;
		//var s = "Uninitialized";
		//if (ws) {
			//s = ["Connecting", "Open", "Closing", "Connecting"][ws.readyState];
		//}
		//$(sel).html(s)

	//}
	//setInterval(function() { updateStatus("#status", nwt) }, 250);
	//nwt = new NWT("#nwt");
	p = new Pinger("#pinger");
	t = new TimeSetter();
	l = new Logger("#logger");
	return {};
});

