/**************************************************************************/
/*  components.hpp                                                        */
/**************************************************************************/
/*                        This file is part of:                           */
/*                             GODOT-FLECS                                */
/*                https://github.com/shenerman/godot-flecs                */
/**************************************************************************/
/* Copyright (c) 2026 shenerman.                                          */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "flecs.h"

namespace logic {

struct Position { float x{}, y{}, z{}; };
struct Rotation { float x{}, y{}, z{}; };
struct Scale    { float x{1}, y{1}, z{1}; };

inline void register_components(flecs::world& p_w) {
    p_w.component<Position>();
    p_w.component<Rotation>();
    p_w.component<Scale>();
}


}
