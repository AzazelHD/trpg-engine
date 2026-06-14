#pragma once

#include "engine/statemachine/StateMachine.h"

// SceneStack is the preferred engine-facing name for the generic stack manager.
// It aliases the existing implementation to preserve compatibility with older code.
template <typename T>
using SceneStack = StateMachine<T>;