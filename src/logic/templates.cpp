/* logic/templates.cpp */

#include "logic/templates.hpp"
#include "components.hpp"

namespace logic {

// ── 各种类的装配函数。新增种类 = 写一个 build_<名字> + TEMPLATES 加一行 ──

bool build_bullet(flecs::entity p_e) {
    p_e.add<Bullet>()
        .set<Life>({2.0F});
    return true;
}

bool build_enemy(flecs::entity /*p_e*/) {
    // 敌人逻辑尚未设计——占位行，spawn 到它会报错并处置请求
    return false;
}

BuildFn build_from_name(std::string_view p_name) {
    for (const LookupEntry &entry : TEMPLATES) {
        if (entry.name == p_name) {
            return entry.build;
        }
    }
    return nullptr;
}

} // namespace logic
