/* logic/templates.hpp */

#pragma once

#include <flecs.h>

#include <array>
#include <string_view>

namespace logic {

// 装配函数签名：往一个空实例上挂逻辑组件。
// 返回 false = 该种类的逻辑尚未设计（占位行），spawn 侧报错处置
using BuildFn = bool (*)(flecs::entity);

// 装配函数即编译期身份：logic 内部若编译期就知道要哪种，直接调函数；
// 表只服务数据驱动路径（编辑器来的字符串）
bool build_bullet(flecs::entity p_e);
bool build_enemy(flecs::entity p_e);

// 装载后的组件：只带载荷（装配）。名字不进 prefab——
// 它只是查找键，装载期比对完就完成使命
struct LogicTemplate {
    BuildFn build;
};

// 查找表条目：键 + 载荷，名字只活在这里
struct LookupEntry {
    std::string_view name;
    BuildFn          build;
};

// 唯一清单：加模板 = 写 build 函数 + 这里加一行。
// 长度与条目数失配会在编译期报错（条目无默认构造，少编不过；多也编不过）
inline constexpr std::array<LookupEntry, 2> TEMPLATES = {
    LookupEntry{ "bullet", &build_bullet },
    LookupEntry{ "enemy",  &build_enemy  },
};

// 名字 → 装配（装载期调用）。
// nullptr = 设计师填了 logic 侧没有的名字
BuildFn build_from_name(std::string_view p_name);

} // namespace logic
