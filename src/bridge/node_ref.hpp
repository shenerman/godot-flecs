/**************************************************************************/
/*  bridge/node_ref.hpp                                                   */
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

#include <flecs.h>
#include <godot_cpp/classes/node3d.hpp>

namespace bridge {

// 关系 tag：视图实体 ──(ViewOf)──> 产品实体。
// 边挂在视图上、指向产品（"此视图是某产品的视图"），即：
//     view.add<ViewOf>(product);
// sync 热路径用 view.target<ViewOf>() 直读边，O(1)；
// 产品死亡由 ViewOf 的 (OnDeleteTarget, Delete) 清理策略连带删除
// 持边视图——无级联、无收割扫描、无回指针
struct ViewOf {};

// 挂在视图实体上：一行记录 = 一个视图。
// 连接关系的唯一真源是那条 (ViewOf, product) 边，本组件不再冗余存储
// 产品句柄（1:N 时同一边型可挂任意多个视图实体）
struct NodeRef {
    godot::Node3D *node = nullptr; 
};

} // namespace bridge