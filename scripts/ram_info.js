log("Sway: load ram info")

function on_update_ram_info() {
	//sim.climate.set(climate);
	log("ram_info")
}

subscribe_on_update("on_update_ram_info", 1000, on_update_ram_info)