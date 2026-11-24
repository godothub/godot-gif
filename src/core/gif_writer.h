#ifndef GIF_WRITER_H
#define GIF_WRITER_H

#include "gif_utils.hpp"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/file_access.hpp>

namespace godot {
	class GIFWriter : public RefCounted {
		GDCLASS(GIFWriter, RefCounted)

	public:
		// GIF 错误码
		enum GIFError {
			SUCCEEDED,
			OPEN_FAILED,												// 未能打开给定文件
			WRITER_FAILED,												// 未能写入给定文件
			HAS_SCRN_DSCR,												// 屏幕描述符已设置好，但尝试写第二屏幕描述符
			HAS_IMAG_DSCR,												// 图像描述符仍在激活
			NO_COLOR_MAP,												// 既非全局也非局部色彩映射
			DATA_TOO_BIG,												// 像素数大于 宽度 * 高度
			NOT_ENOUGH_MEM,												// 未能分配
			DISK_IS_FULL,												// 写入失败
			CLOSE_FAILED,												// 未能关闭给定文件
			NOT_WRITEABLE,												// 给定文件不是 打开写入
			INVALID_DIMENSIONS,											// 尺寸无效 (0 或超过 65535)
			NO_FILE_OPEN,												// 没有打开文件
			PALETTE_TOO_LARGE,											// 调色板颜色数超过256
			NO_SCRN_DSCR,												// 未设置屏幕描述符
			NO_IMAG_DSCR,												// 未设置图像描述符
		};

		// 处置方式（同 GIFReader）
		enum DisposalMethod {
			DISPOSAL_METHOD_UNSPECIFIED = 0,							// 未指定
			DISPOSAL_METHOD_DO_NOT = 1,									// 不处置，保留当前帧
			DISPOSAL_METHOD_BACKGROUND = 2,								// 恢复到背景色
			DISPOSAL_METHOD_PREVIOUS = 3								// 恢复到前一帧
		};

	private:
		GifFileType* file_type = nullptr;

		// 内存写入用的数据
		PackedByteArray mem_output_data;
		static int _mem_output_func(GifFileType* gif, const GifByteType* bytes, int size);

		// 当前写入状态
		bool has_screen_desc = false;
		bool has_image_desc = false;
		int current_frame = 0;

		// 当前帧状态 (用于 GCB)
		int frame_left = 0;
		int frame_top = 0;
		int frame_width = 0;
		int frame_height = 0;
		bool frame_interlace = false;
		int frame_delay = 100;											// 默认 100ms
		DisposalMethod frame_disposal = DISPOSAL_METHOD_UNSPECIFIED;
		int frame_transparent_index = -1;								// -1 表示无透明色
		ColorMapObject* frame_color_map = nullptr;

		Ref<FileAccess> fa; // 用于保存打开的文件句柄
		static int _file_write_callback(GifFileType* gif, const GifByteType* bytes, int size);

	protected:
		static void _bind_methods();

	public:
		~GIFWriter();

		GIFError open(const String& p_path);							// 打开文件进行写入 (覆盖模式)
		GIFError open_from_buffer();									// 打开内存缓冲区进行写入
		GIFError close();												// 关闭文件/流，完成写入
		PackedByteArray get_output_buffer() const;						// 获取内存写入后的缓冲区（仅在 open_from_buffer 后有效）

		// 全局属性设置 (必须在写入任何帧之前设置)
		GIFError set_canvas_size(
			const int p_width,
			const int p_height,
			const int p_background_color = 0
		);																// 设置画布尺寸 (屏幕描述符)，必须在写入第一帧之前调用，且只能调用一次
		GIFError set_loop_count(const int p_loop_count);				// 设置循环次数 (Netscape 应用扩展)
		GIFError set_global_palette(
			const PackedColorArray& p_palette
		);																// 设置全局调色板，如果不设置，每帧必须使用局部调色板
		GIFError set_color_resolution(const int p_bits);				// 设置颜色分辨率 (位深度: 1-8，默认 8)

		// 逐帧写入 (顺序写入模式)
		GIFError begin_frame(
			const int p_left,
			const int p_top,
			const int p_width,
			const int p_height,
			const bool p_interlace = false
		);																// 开始写入一帧，调用后必须跟着 write_frame_pixels 或 write_frame_image
		GIFError set_frame_delay(int p_delay_ms);						// 设置当前帧的延迟时间 (ms)
		GIFError set_disposal_method(DisposalMethod p_method);			// 设置当前帧的处置方式
		GIFError set_transparent_color(const int p_index);				// 设置当前帧的透明色索引 (-1 表示无透明色)
		GIFError set_frame_palette(
			const PackedColorArray& p_palette
		);																// 设置当前帧的局部调色板
		GIFError write_frame_pixels(
			const PackedByteArray& p_indices
		);																// 写入帧像素数据 (8位索引数据，数据长度必须等于 width * height)
		GIFError write_frame_image(
			Ref<Image> p_image,
			const bool p_quantize = true
		);																// 写入帧图像并自动量化颜色
		GIFError end_frame();											// 结束当前帧写入

		// 一次性写入 (简化 API)
		GIFError write_gif(
			const TypedArray<Image>& p_images,
			const PackedInt32Array& p_delays,
			const int p_loop_count = 0,
			const bool p_quantize = true
		);																// 一次性写入整个 GIF（Spew 模式）
		static GIFError save_to_file(
			const String& p_path,
			const TypedArray<Image>& p_images,
			const PackedInt32Array& p_delays,
			const int p_loop_count = 0,
			const bool p_quantize = true
		);																// 从文件路径一次性写入 GIF
		static Dictionary save_to_buffer(
			const TypedArray<Image>& p_images,
			const PackedInt32Array& p_delays,
			const int p_loop_count = 0,
			const bool p_quantize = true
		);																// 一次性写入 GIF 到内存缓冲区

		// 扩展块
		GIFError add_comment(const String& p_comment);					// 添加注释块

		// 状态查询
		int get_frame_count() const;									// 获取已写入的帧数
		Vector2i get_canvas_size() const;								// 获取画布尺寸
	};
}

VARIANT_ENUM_CAST(GIFWriter::GIFError);
VARIANT_ENUM_CAST(GIFWriter::DisposalMethod);

#endif // !GIF_WRITER_H
