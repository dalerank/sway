log("Sway: load config started")

function _format() {
    var formatted = arguments[0]
    for (var arg in arguments) {
         if(arg==0)
            continue
        formatted = formatted.replace("{" + (arg-1) + "}", arguments[arg])
    }
    return formatted
}

function _printobj(o) {
    for (var p in o)
        log( p + ': ' + o[p]);
}

var g_config = {
	window_height : 64,
	window_width : -1,
	window_ypos : -64,
	window_xpos : 0,
	modules : {},
}
