log("Sway: load ram info")

var ModuleRam = {
	data : [],
	state : null,
	datalen : 70,
	wsize : {x:90, y:g_config.window_height}
}

ModuleRam.init = function() {
	for (var i=0; i < ModuleRam.datalen; i++)
		ModuleRam.data.push(0)
}

ModuleRam.on_update_ram_info = function() {
	log("ram_info")

	ModuleRam.state = hw_ram.get()
	ModuleRam.data.push(ModuleRam.state.UsagePercentage / 100)
	if (ModuleRam.data.length > ModuleRam.datalen)
		ModuleRam.data = ModuleRam.data.slice(1)
}

ModuleRam.on_frame_ram_info = function() {
	if (!ModuleRam.state)
		return

  //ui.SetNextWindowPos({x:0, y:0})
  var wsize = _sscale(ModuleRam.wsize)
  ui.SetNextWindowSize(wsize)
  ui.Begin("#a", true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize)
  ui.PushStyleColor(ImGuiCol_FrameBg, {a:0, r:0, g:0, b:0})
  ui.PlotHistogram("", ModuleRam.data, 0, "", -1.0, 1.0, wsize)
  ui.PopStyleColor(1)
  ui.SetCursorPos({x:10, y:5})
  ui.Text("RAM")
  ui.SetCursorPos({x:10, y:20})
  var dwMBFactor = 1024 * 1024 * 1024;
  var used_mb = to_string("{:#.3g}", (ModuleRam.state.UsagePercentage / 100) * (ModuleRam.state.TotalPhys / dwMBFactor))
  var all_mb = to_string("{:#.3g}", (ModuleRam.state.TotalPhys / dwMBFactor))
  var percents_mb = to_string("{:02}", ModuleRam.state.UsagePercentage)
  ui.Text(_format("{0}/{1}Gb {2}%%", used_mb, all_mb, percents_mb))
  ui.End()
}

ModuleRam.init()

subscribe_on_update("on_update_ram_info", 1000, ModuleRam.on_update_ram_info)
subscribe_on_frame("on_frame_ram_info", ModuleRam.on_frame_ram_info)