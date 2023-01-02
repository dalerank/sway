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
	window_xscale : 1,
	window_yscale : 1,
	modules : {},
}

function _xpos(x) { return x * g_config.window_xscale; }
function _ypos(y) { return y * g_config.window_yscale; }
function _sscale(p) { return {x:_xpos(p.x), y:_ypos(p.y)}}
