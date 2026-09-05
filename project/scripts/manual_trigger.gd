extends Node

@onready var _world: Node = $FlecsWorld
@onready var _bridge: Node = $FlecsWorld/BridgeNode

func _unhandled_key_input(event: InputEvent) -> void:
	if event is InputEventKey and event.pressed and not event.echo:
		match event.keycode:
			KEY_5: _do_t5_reparent()
			KEY_6: _do_t6_world_reenter()

# ---- T5：把 BridgeNode 从 FlecsWorld 下摘走，挂到根节点 ----
func _do_t5_reparent() -> void:
	print("[T5] reparent bridge -> root")
	# remove_child + add_child 才会触发 _exit_tree/_enter_tree，
	# reparent() 默认保全局 transform 但也会走完整通知，两者皆可，这里用直白写法
	_world.remove_child(_bridge)
	add_child(_bridge)
	# 此时观察点：
	# 1. stderr 恰好一条 _was_bound 警告
	# 2. 输出里没有新的 "[bind]" 日志（没有第二实体）

# ---- T6：把 FlecsWorld 整个拔下来再插回去 ----
func _do_t6_world_reenter() -> void:
	print("[T6] remove & re-add FlecsWorld")
	remove_child(_world)
	await get_tree().process_frame      # 让 _exit_tree 彻底走完
	add_child(_world)
	# 此时观察点：
	# 1. "[bind]" 日志没有第二次出现（world 没被重建）
	# 2. T2 的 import 打印若在每帧循环里，会继续输出相同数值（实体还活着）
