# Contributing to Godot-GIF

## Prerequisites

- Git, Python 3.8 or newer, and SCons 4.0 or newer.
- A C++ toolchain for the target platform. The SCons build uses C++17. The Visual Studio project is configured for C++20.
- A Godot editor compatible with the pinned `godot-cpp` bindings for local testing. The current submodule targets the Godot 4.5 GDExtension API. Use Godot 4.7 to test the native GIF Inspector preview.

## Set up the checkout

Clone the repository with its `godot-cpp` submodule and install SCons.

```sh
git clone https://github.com/godothub/godot-gif.git
cd godot-gif
python install pip
```

For an existing checkout, run `git submodule update --init --recursive` from the repository root to use the pinned submodule revision.

## Build the add-on

Run SCons from the repository root. These commands build the libraries for Windows x64 with the standard single-precision Godot configuration.

```sh
scons platform=windows target=template_debug
scons platform=windows target=template_release
```

- `template_debug` is loaded by the editor and debug exports. It embeds the class reference from `doc_classes/`.
- `template_release` is loaded by release exports. Build it to test a release export on the same platform and architecture.

SCons writes the libraries to `addons/godot_gif/bin/`. The [GDExtension manifest](addons/godot_gif/bin/godot_gif.gdextension) lists the expected file names for each platform, architecture, and build type. `Godot-GIF.slnx` can be used to browse the C++ sources in Visual Studio. Use SCons to build the add-on libraries.

### Other platforms

Use the same commands with the target platform and architecture below. Use a toolchain that can produce binaries for that architecture. Run `scons -h` to see the options exposed by the pinned bindings.

| Platform | `platform` | `arch` values | Toolchain |
| --- | --- | --- | --- |
| Windows | `windows` | `x86_64`, `x86_32`, `arm64` | Visual Studio C++ Build Tools or MinGW-w64 for the target architecture |
| Linux | `linux` | `x86_64`, `x86_32`, `arm64`, `arm32` | GCC or Clang for the target architecture. The x86 32-bit build on an x64 host needs a multilib toolchain. |
| macOS | `macos` | `universal` | Xcode command-line tools and the macOS SDK |
| Android | `android` | `arm64`, `arm32`, `x86_64`, `x86_32` | Android SDK and NDK. Set `ANDROID_HOME` to the SDK root. The pinned bindings default to NDK `28.1.13356709`. |
| iOS | `ios` | `arm64` | Xcode with the iOS device SDK |
| Web | `web` | `wasm32` | Emscripten SDK with `emcc` on `PATH` |

The [GitHub Actions build workflow](.github/workflows/make_build.yml) lists the CI build matrix and packages the libraries together with the add-on files.

### Automated builds and releases

Pushes to `main` and pull requests run a Windows x64 debug build. To publish a version, push a tag such as `v1.0.1`, or run **Build and Release Godot-GIF** from the repository's **Actions** tab and enter a tag.

The release workflow builds all 14 platform and architecture combinations in both debug and release configurations. It verifies every library listed in the GDExtension manifest, then creates `godot-gif-<tag>.zip` with an `addons/godot_gif/` directory containing the libraries, icons, translations, and licenses. The ZIP is attached to a GitHub Release with generated release notes. Tags containing a hyphen, such as `v1.0.1-rc1`, create prereleases.

For a manual run, an existing tag selects that tag's commit. A new tag is created at the commit selected for the workflow run. Each build job uses the same resolved commit. Rerunning the release workflow replaces the ZIP asset for that release.

## Test in Godot

1. Close the editor before rebuilding or replacing a library that it has loaded.
2. Copy or symlink the complete `addons/godot_gif/` folder into a test project's `addons/` directory.
3. Open the project. The GDExtension automatically registers GIF importing and resource previews.
4. Add a `.gif` file to the project and check that it imports as a `GIFTexture`. On Godot 4.7, inspect the resource and change `Frame` to check the preview. To check playback, assign the resource to a `GIFPlayer` in a scene and run it.
