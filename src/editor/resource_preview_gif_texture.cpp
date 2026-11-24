#include "resource_preview_gif_texture.h"

#include "../core/gif_texture.h"
#include "../core/gif_reader.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/file_access.hpp>

namespace godot {
	static void post_process_preview(Ref<Image> p_image) {
		if (p_image->get_format() != Image::FORMAT_RGBA8) {
			p_image->convert(Image::FORMAT_RGBA8);
		}

		const int width = p_image->get_width();
		const int height = p_image->get_height();
		const int radius = MIN(width, height) / 32;
		const int radius_squared = radius * radius;
		const Color transparent = Color(0, 0, 0, 0);

		for (int x = 0; x < radius; x++) {
			for (int y = 0; y < radius; y++) {
				const int dx = x - radius;
				const int dy = y - radius;
				if (dx * dx + dy * dy > radius_squared) {
					p_image->set_pixel(x, y, transparent);
					p_image->set_pixel(width - 1 - x, y, transparent);
					p_image->set_pixel(width - 1 - x, height - 1 - y, transparent);
					p_image->set_pixel(x, height - 1 - y, transparent);
				}
				else {
					break;
				}
			}
		}
	}

	static Ref<Texture2D> generate_preview_from_image(const Ref<Image>& p_image, const Vector2i& p_size) {
		if (p_image.is_null() || p_image->is_empty() || p_size.x <= 0 || p_size.y <= 0) {
			return Ref<Texture2D>();
		}

		Ref<Image> img = p_image->duplicate();
		img->clear_mipmaps();

		if (img->is_compressed() && img->decompress() != OK) {
			return Ref<Texture2D>();
		}

		Vector2i img_size = img->get_size();
		if (img_size.x > p_size.x || img_size.y > p_size.y) {
			float scale = MIN(static_cast<float>(p_size.x) / img_size.x, static_cast<float>(p_size.y) / img_size.y);
			img->resize(MAX(1, static_cast<int>(img_size.x * scale)), MAX(1, static_cast<int>(img_size.y * scale)), Image::INTERPOLATE_CUBIC);
		}

		post_process_preview(img);
		return ImageTexture::create_from_image(img);
	}

	void ResourcePreviewGIFTexture::_bind_methods() {

	}

	bool ResourcePreviewGIFTexture::_handles(const String& p_type) const {
		return p_type == "GIFTexture";
	}

	Ref<Texture2D> ResourcePreviewGIFTexture::_generate(const Ref<Resource>& p_resource, const Vector2i& p_size, const Dictionary& p_metadata) const {
		Ref<GIFTexture> gif = p_resource;
		if (gif.is_null() || gif->get_frame_count() == 0) {
			return Ref<Texture2D>();
		}

		Ref<ImageTexture> frame_texture = gif->get_frame_texture(0);
		if (frame_texture.is_null()) {
			return Ref<Texture2D>();
		}

		Ref<Image> img = frame_texture->get_image();
		if (img.is_null()) {
			return Ref<Texture2D>();
		}

		const_cast<Dictionary&>(p_metadata)["dimensions"] = img->get_size();

		return generate_preview_from_image(img, p_size);
	}

	Ref<Texture2D> ResourcePreviewGIFTexture::_generate_from_path(const String& p_path, const Vector2i& p_size, const Dictionary& p_metadata) const {
		Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ);
		if (file.is_null()) {
			return Ref<Texture2D>();
		}

		const uint64_t file_size = file->get_length();
		if (file_size == 0) {
			file->close();
			return Ref<Texture2D>();
		}

		PackedByteArray gif_data;
		gif_data.resize(file_size);
		const uint64_t bytes_read = file->get_buffer(gif_data.ptrw(), file_size);
		file->close();

		if (bytes_read != file_size) {
			return Ref<Texture2D>();
		}

		Ref<GIFReader> reader;
		reader.instantiate();
		if (reader->open_from_buffer(gif_data) != GIFReader::SUCCEEDED || reader->get_image_count() == 0) {
			return Ref<Texture2D>();
		}

		Ref<Image> img = reader->get_image(0);
		if (img.is_valid()) {
			const_cast<Dictionary&>(p_metadata)["dimensions"] = img->get_size();
		}
		return generate_preview_from_image(img, p_size);
	}

	bool ResourcePreviewGIFTexture::_generate_small_preview_automatically() const {
		return true;
	}

	bool ResourcePreviewGIFTexture::_can_generate_small_preview() const {
		return true;
	}
}
