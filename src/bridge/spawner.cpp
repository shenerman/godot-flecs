#include "bridge/spawner.hpp"

#include <flecs.h>

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "bridge/bridge_node.hpp"
#include "bridge/despawn.hpp"
#include "bridge/node_ref.hpp"
#include "bridge/template_registry.hpp"
#include "logic/components.hpp"

namespace bridge {
namespace {

// ── 逻辑半张模板表（方案 A）：逻辑组件配置是程序员的领域，编译期定 ──
// TemplateRegistry 只管视图，两半职责两清
bool build_logic(logic::TemplateId p_id, flecs::entity p_e) {
    if (p_id == logic::BULLET_ID) {
        p_e.add<logic::Bullet>()
            .set<logic::Life>({2.0F});
        return true;
    }
    return false;
}

}  // namespace

void register_spawner(flecs::world &p_w,
                      TemplateRegistry *p_registry,
                      godot::Node *p_view_host) {
    p_w.system<const logic::SpawnRequest>("spawn_from_requests")
        // PreUpdate 消费上一帧 OnUpdate 产出的请求——产与消相位错开，
        // 排序由管线硬保证，不依赖同相位内的声明顺序
        .kind(flecs::PreUpdate)
        .each([p_registry, p_view_host](flecs::iter &p_it, size_t p_i,
                                        const logic::SpawnRequest &p_req) 
            {
            flecs::entity e = p_it.entity(p_i);

            // 1. 查视图模板。find 缺席返回 nullptr 不报错——
            //    报错责任在写点（此处），带上下文并干净处置请求
            
            godot::PackedScene *scene = p_registry->find(p_req.identity);
            if (scene == nullptr) {
                godot::UtilityFunctions::push_error(
                    godot::vformat("spawn: unknown TemplateId %d",
                                   static_cast<int64_t>(p_req.identity)));
                despawn(e);
                return;
            }

            // 2. 逻辑半张表
            if (!build_logic(p_req.identity, e)) {
                godot::UtilityFunctions::push_error(
                    godot::vformat("spawn: TemplateId %d 无逻辑模板",
                                   static_cast<int64_t>(p_req.identity)));
                despawn(e);
                return;
            }

            // 3. 实例化视图，根必须是 BridgeNode（场景模板契约 1）
            auto *view = godot::Object::cast_to<BridgeNode>(scene->instantiate());
            if (view == nullptr) {
                godot::UtilityFunctions::push_error(
                    godot::vformat("spawn: 模板 %d 的场景根节点缺少 BridgeNode",
                                   static_cast<int64_t>(p_req.identity)));
                despawn(e);
                return;
            }

            // 4. NodeRef 写点——view 非空已由上一步 cast 保证
            e.set<NodeRef>({view});

            // 5. 绑定先于入树（_enter_tree 验牌契约）
            view->assign_entity(e.id());

            // 6. 覆盖位姿；optional 缺席 = 场景出厂值（契约 2），不动
            if (p_req.position) {
                view->set_position(
                    {p_req.position->x, p_req.position->y, p_req.position->z});
            }
            if (p_req.rotation) {
                view->set_rotation(
                    {p_req.rotation->x, p_req.rotation->y, p_req.rotation->z});
            }
            if (p_req.scale) {
                view->set_scale(
                    {p_req.scale->x, p_req.scale->y, p_req.scale->z});
            }

            // 7. 入树——_enter_tree 验牌在此通过
            p_view_host->add_child(view);

            e.remove<logic::SpawnRequest>();
        });
}

}  // namespace bridge