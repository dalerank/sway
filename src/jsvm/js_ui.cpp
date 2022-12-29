#include "js_ui.h"

#include "js_defines.h"

#include "mujs.h"

void js_ui_show_escape_dialog(js_State *J)
{
    //hotkey_handle_escape();
    js_pushundefined(J);
}

void js_ui_window_new_career_show(js_State *J)
{
    //window_new_career_show();
    js_pushundefined(J);
}

void js_ui_window_cck_selection_show(js_State *J)
{
    //window_cck_selection_show();
    js_pushundefined(J);
}

void js_ui_window_file_dialog_show(js_State *J)
{
    int type = js_tointeger(J, 1);
    int dialog_type = js_tointeger(J, 2);
    //window_file_dialog_show(type, dialog_type);
    js_pushundefined(J);
}

void js_ui_window_mission_editor_show(js_State *J)
{
    js_pushundefined(J);
}

void js_ui_window_config_show(js_State *J)
{
    //window_config_show(CONFIG_FIRST_PAGE, 1);
    js_pushundefined(J);
}

static void confirm_exit(int accepted, int checked)
{
    if (accepted) {
        //system_exit();
    }
}

void js_ui_window_popup_dialog_show(js_State *J)
{
    //window_popup_dialog_show(POPUP_DIALOG_QUIT, confirm_exit, 1);
    js_pushundefined(J);
}

void js_register_ui_functions(js_State *J)
{
    DEF_GLOBAL_OBJECT(J, ui)
    REGISTER_GLOBAL_OBJECT(J, ui)

    REGISTER_GLOBAL_FUNCTION(J, js_ui_show_escape_dialog, "show_escape_dialog", 0);
    REGISTER_GLOBAL_FUNCTION(J, js_ui_window_file_dialog_show, "window_file_dialog_show", 2);
    REGISTER_GLOBAL_FUNCTION(J, js_ui_window_new_career_show, "window_new_career_show", 0);
    REGISTER_GLOBAL_FUNCTION(J, js_ui_window_cck_selection_show, "window_cck_selection_show", 0);
    REGISTER_GLOBAL_FUNCTION(J, js_ui_window_mission_editor_show, "window_mission_editor_show", 0);
    REGISTER_GLOBAL_FUNCTION(J, js_ui_window_config_show, "window_config_show", 0);
    REGISTER_GLOBAL_FUNCTION(J, js_ui_window_popup_dialog_show, "window_popup_dialog_show", 0);
}
