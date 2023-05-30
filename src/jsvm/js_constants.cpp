#include "mujs.h"
#include "js_defines.h"

#include "ui/imgui.h"

void js_register_constants(js_State *J)
{
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_None                   );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoTitleBar             );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoResize               );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoMove                 );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoScrollbar            );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoScrollWithMouse      );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoCollapse             );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_AlwaysAutoResize       );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoBackground           );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoSavedSettings        );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoMouseInputs          );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_MenuBar                );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_HorizontalScrollbar    );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoFocusOnAppearing     );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoBringToFrontOnFocus  );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_AlwaysUseWindowPadding );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoNavInputs            );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoNavFocus             );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_UnsavedDocument        );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoNav                  );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoDecoration           );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiWindowFlags_NoInputs               );

    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_Text                           );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TextDisabled                   );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_WindowBg                       );  // Background of normal windows
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ChildBg                        );  // Background of child windows
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_PopupBg                        );  // Background of popups, menus, tooltips windows
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_Border                         );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_BorderShadow                   );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_FrameBg                        );  // Background of checkbox, radio button, plot, slider, text input
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_FrameBgHovered                 );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_FrameBgActive                  );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TitleBg                        );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TitleBgActive                  );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TitleBgCollapsed               );
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_MenuBarBg);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ScrollbarBg);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ScrollbarGrab);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ScrollbarGrabHovered);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ScrollbarGrabActive);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_CheckMark);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_SliderGrab);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_SliderGrabActive);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_Button);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ButtonHovered);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ButtonActive);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_Header);                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_HeaderHovered);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_HeaderActive);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_Separator);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_SeparatorHovered);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_SeparatorActive);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ResizeGrip);            // Resize grip in lower-right and lower-left corners of windows.
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ResizeGripHovered);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ResizeGripActive);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_Tab);                   // TabItem in a TabBar
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TabHovered);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TabActive);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TabUnfocused);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TabUnfocusedActive);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_PlotLines);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_PlotLinesHovered);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_PlotHistogram);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_PlotHistogramHovered);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TableHeaderBg);         // Table header background
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TableBorderStrong);     // Table outer and header borders (prefer using Alpha=1.0 here)
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TableBorderLight);      // Table inner borders (prefer using Alpha=1.0 here)
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TableRowBg);            // Table row background (even rows)
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TableRowBgAlt);         // Table row background (odd rows)
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_TextSelectedBg);
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_DragDropTarget);        // Rectangle highlighting a drop target
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_NavHighlight);          // Gamepad/keyboard: current highlighted item
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_NavWindowingHighlight); // Highlight window when using CTRL+TAB
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_NavWindowingDimBg);     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
    REGISTER_GLOBAL_CONSTANT(J, ImGuiCol_ModalWindowDimBg);      // Darken/colorize entire screen behind a modal window, when one is active
}