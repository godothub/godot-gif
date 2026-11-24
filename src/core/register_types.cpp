#include "register_types.h"

#include "gif_reader.h"
#include "gif_writer.h"
#include "gif_texture.h"
#include "../editor/gif_editor_plugin.h"
#include "../editor/gif_editor_translations.h"
#include "../editor/resource_importer_gif_texture.h"
#include "../editor/resource_preview_gif_texture.h"
#include "../node/gif_player.h"

#include <gdextension_interface.h>
#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_godot_gif_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_CLASS(ResourceImporterGIFTexture);
		GDREGISTER_CLASS(ResourcePreviewGIFTexture);
		GDREGISTER_INTERNAL_CLASS(GIFEditorPlugin);
		GIFEditorPlugin::prepare_registration();
		EditorPlugins::add_by_type<GIFEditorPlugin>();
		GDREGISTER_INTERNAL_CLASS(GIFEditorTranslationPlugin);
		EditorPlugins::add_by_type<GIFEditorTranslationPlugin>();
		return;
	}

	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(GIFReader);
	GDREGISTER_CLASS(GIFWriter);
	GDREGISTER_CLASS(GIFTexture);
	GDREGISTER_CLASS(GIFPlayer);
}

void uninitialize_godot_gif_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::remove_by_type<GIFEditorTranslationPlugin>();
		EditorPlugins::remove_by_type<GIFEditorPlugin>();
		GIFEditorPlugin::cleanup_registration();
		return;
	}

	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
	// Initialization.
	GDExtensionBool GDE_EXPORT godot_gif_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization) {
		godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

		init_obj.register_initializer(initialize_godot_gif_module);
		init_obj.register_terminator(uninitialize_godot_gif_module);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);

		return init_obj.init();
	}
}
