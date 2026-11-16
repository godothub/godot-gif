#pragma once

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/translation.hpp>
#include <godot_cpp/templates/hash_map.hpp>

namespace godot {
	class GIFEditorTranslationPlugin : public EditorPlugin {
		GDCLASS(GIFEditorTranslationPlugin, EditorPlugin)

		HashMap<StringName, Ref<Translation>> translations;

	protected:
		static void _bind_methods() {}

	public:
		void _enter_tree() override;
		void _exit_tree() override;
	};
}
