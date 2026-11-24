#ifndef RESOURCE_PREVIEW_GIF_TEXTURE_H
#define RESOURCE_PREVIEW_GIF_TEXTURE_H

#include <godot_cpp/classes/editor_resource_preview_generator.hpp>

namespace godot {
	class ResourcePreviewGIFTexture : public EditorResourcePreviewGenerator {
		GDCLASS(ResourcePreviewGIFTexture, EditorResourcePreviewGenerator)

	protected:
		static void _bind_methods();

	public:
		virtual bool _handles(const String& p_type) const override;
		virtual Ref<Texture2D> _generate(const Ref<Resource>& p_resource, const Vector2i& p_size, const Dictionary& p_metadata) const override;
		virtual Ref<Texture2D> _generate_from_path(const String& p_path, const Vector2i& p_size, const Dictionary& p_metadata) const override;
		virtual bool _generate_small_preview_automatically() const override;
		virtual bool _can_generate_small_preview() const override;
	};
}

#endif // !RESOURCE_PREVIEW_GIF_TEXTURE_H
