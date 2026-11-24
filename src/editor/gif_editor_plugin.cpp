#include "gif_editor_plugin.h"

#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_resource_preview.hpp>
#include <godot_cpp/classes/engine.hpp>

namespace godot {
	Ref<ResourceImporterGIFTexture> GIFEditorPlugin::startup_importer;

	void GIFEditorPlugin::prepare_registration() {
		if (!Engine::get_singleton()->is_editor_hint()) {
			return;
		}

		const EditorInterface* editor = EditorInterface::get_singleton();
		if (editor == nullptr || editor->get_resource_filesystem() == nullptr || startup_importer.is_valid()) {
			return;
		}

		// 首次扫描会延迟创建编辑器插件，需要提前注册导入器
		startup_importer.instantiate();
		EditorPlugin* registrar = memnew(EditorPlugin);
		registrar->add_import_plugin(startup_importer);
		memdelete(registrar);
	}

	void GIFEditorPlugin::cleanup_registration() {
		if (startup_importer.is_null()) {
			return;
		}

		EditorPlugin* registrar = memnew(EditorPlugin);
		registrar->remove_import_plugin(startup_importer);
		startup_importer.unref();
		memdelete(registrar);
	}

	void GIFEditorPlugin::_enter_tree() {
		if (startup_importer.is_valid()) {
			importer = startup_importer;
			startup_importer.unref();
		} else {
			importer.instantiate();
			add_import_plugin(importer);
		}

		preview.instantiate();
		EditorInterface::get_singleton()->get_resource_previewer()->add_preview_generator(preview);
	}

	void GIFEditorPlugin::_exit_tree() {
		EditorInterface::get_singleton()->get_resource_previewer()->remove_preview_generator(preview);
		preview.unref();
		remove_import_plugin(importer);
		importer.unref();
	}
}
