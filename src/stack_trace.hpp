#pragma once

#include "platform.hpp"

namespace crashhandler
{
void install();
void remove();
void printstack(bool showMessage = true, unsigned int starting_frame = 0, unsigned int max_frames = 63 );
}

