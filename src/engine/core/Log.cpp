#include "engine/core/Log.h"

// Log is header-only (macros). Nothing to implement here.
// This file exists so CMake includes the core/ folder and the header gets compiled.
//
// If you later decide to add a Log::init() that opens a file stream for log output,
// implement it here.
