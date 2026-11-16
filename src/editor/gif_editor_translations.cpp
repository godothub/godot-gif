#include "gif_editor_translations.h"

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/translation.hpp>
#include <godot_cpp/classes/translation_domain.hpp>
#include <godot_cpp/classes/translation_server.hpp>

namespace godot {
	namespace {
		struct TranslationCatalog {
			const char* domain;
			const char* path;
		};

		constexpr TranslationCatalog TRANSLATION_CATALOGS[] = {
			{ "godot.properties", "res://addons/godot_gif/translations/properties.zh_Hans.po" },
			{ "godot.documentation", "res://addons/godot_gif/translations/documentation.zh_Hans.po" },
		};
	}

	void GIFEditorTranslationPlugin::_enter_tree() {
		// The editor resets its built-in translation domains during language setup.
		ResourceLoader* loader = ResourceLoader::get_singleton();
		TranslationServer* server = TranslationServer::get_singleton();
		for (const TranslationCatalog& catalog : TRANSLATION_CATALOGS) {
			if (!loader->exists(catalog.path, "Translation")) {
				continue;
			}
			const Ref<Translation> translation = loader->load(catalog.path, "Translation");
			if (translation.is_valid()) {
				server->get_or_add_domain(catalog.domain)->add_translation(translation);
				translations.insert(catalog.domain, translation);
			}
		}
	}

	void GIFEditorTranslationPlugin::_exit_tree() {
		TranslationServer* server = TranslationServer::get_singleton();
		for (const KeyValue<StringName, Ref<Translation>>& entry : translations) {
			server->get_or_add_domain(entry.key)->remove_translation(entry.value);
		}
		translations.clear();
	}
}
