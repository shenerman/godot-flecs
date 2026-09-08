#pragma once

namespace logic {

// 动作清单：唯一登记处，新增动作只改这里
struct ActionList {
	static constexpr const char *MOVE_LEFT = "move_left";
	static constexpr const char *MOVE_RIGHT = "move_right";
	static constexpr const char *MOVE_UP = "move_up";
	static constexpr const char *MOVE_DOWN = "move_down";
	static constexpr const char *FIRE = "fire";
};

// 输入快照：逻辑系统唯一能看到的输入形态
// 每帧由 bridge 层轮询 Godot 填充，作为单例组件存入 world
struct InputState {
	float move_x = 0.0F; // -1 左 / +1 右
	float move_y = 0.0F; // -1 上 / +1 下
	bool fire = false;
};

} // namespace logic
