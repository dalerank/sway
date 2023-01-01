log("Sway: load modules started")

var _format = function() {
    var formatted = arguments[0]
    for (var arg in arguments) {
         if(arg==0)
            continue
        formatted = formatted.replace("{" + (arg-1) + "}", arguments[arg])
    }
    return formatted
};

var modules = [
    ":ram_info.js",
    ":main_menu.js"
]

for (var i in modules) {
    log("Loading module " + modules[i])
    load_module(modules[i])
}
