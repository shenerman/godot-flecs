/* logic/templates.hpp */
#pragma once

#include <flecs.h>

#include "bridge/template_list.hpp"

namespace bridge {

// 装载流水线：清单（设计师域）→ 模板实体（引擎域）的唯一关口。
// 步骤：查重 → 验场景 → 建 prefab + ViewTemplate → 解析 LogicKind → 验牌。
// 任何一步失败都是启动期写点报错并跳过该条，不静默、不拖到首次 spawn
void load_templates(flecs::world &p_w,
                    const godot::Ref<bridge::TemplateList> &p_list);

} // namespace logic
