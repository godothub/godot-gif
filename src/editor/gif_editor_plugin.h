#pragma once

#include "resource_importer_gif_texture.h"
#include "resource_preview_gif_texture.h"

#include <godot_cpp/classes/editor_plugin.hpp>

namespace godot {
	class GIFEditorPlugin : public EditorPlugin {
		GDCLASS(GIFEditorPlugin, EditorPlugin)

	public:
		static void prepare_registration();
		static void cleanup_registration();

		void _enter_tree() override;
		void _exit_tree() override;

	protected:
		static void _bind_methods() {}

	private:
		static Ref<ResourceImporterGIFTexture> startup_importer;

		Ref<ResourceImporterGIFTexture> importer;
		Ref<ResourcePreviewGIFTexture> preview;
	};
}
