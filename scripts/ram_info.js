log("Sway: load ram info")

var data = []
var state = null
var datalen = 70

for (var i=0; i < datalen; i++)
 	data.push(0)

function on_update_ram_info() {
	log("ram_info")

	state = raminfo.get()
	data.push(state.UsagePercentage / 100)
	if (data.length > datalen)
		data = data.slice(1)

	// log(to_string("{:3}", data.length))
	// var out = '';
  // for (var p in state) {
  //   log( p + ': ' + state[p]);
  // }
}

function on_frame_ram_info() {
	if (!state)
		return

  //ui.SetNextWindowPos({x:0, y:0})
  ui.SetNextWindowSize({x:120, y:60})
  ui.Begin("#a", true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize)
  ui.PushStyleColor(ImGuiCol_FrameBg, {a:0, r:0, g:0, b:0})
  ui.PlotHistogram("", data, 0, "", -1.0, 1.0, {x:120, y:100})
  ui.PopStyleColor(1)
  ui.SetCursorPos({x:10, y:5})
  ui.Text("RAM")
  ui.SetCursorPos({x:10, y:20})
  var dwMBFactor = 1024 * 1024 * 1024;
  var used_mb = to_string("{:#.3g}", (state.UsagePercentage / 100) * (state.TotalPhys / dwMBFactor))
  var all_mb = to_string("{:#.3g}", (state.TotalPhys / dwMBFactor))
  var percents_mb = to_string("{:02}", state.UsagePercentage)
  ui.Text(_format("{0}/{1}Gb {2}%%", used_mb, all_mb, percents_mb))
  ui.End()

}

subscribe_on_update("on_update_ram_info", 1000, on_update_ram_info)
subscribe_on_frame("on_frame_ram_info", on_frame_ram_info)