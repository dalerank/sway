log("Sway: load modules started")

var modules = [
    ":ram_info.js",
    ":main_menu.js"
]

for (var i in modules) {
    log("Loading module " + modules[i])
    load_module(modules[i])
}
