"""Stage build outputs and assemble the Godot-GIF release archive."""

from argparse import ArgumentParser
from configparser import ConfigParser
from json import loads
from pathlib import Path
from shutil import copy2, rmtree
from zipfile import ZIP_DEFLATED, ZipFile


ROOT = Path(__file__).resolve().parents[1]
ADDON_PATH = Path("addons/godot_gif")
ADDON = ROOT / ADDON_PATH
MANIFEST = Path("bin/godot_gif.gdextension")


def read_manifest():
    config = ConfigParser(interpolation=None)
    config.read(ADDON / MANIFEST, encoding="utf-8")
    return config


def read_libraries():
    return {
        key: MANIFEST.parent / loads(value)
        for key, value in read_manifest()["libraries"].items()
    }


def library_file(path):
    if path.suffix == ".framework":
        return path / path.stem
    return path


def require_file(path):
    if not path.is_file() or path.stat().st_size == 0:
        raise SystemExit(f"Missing or empty package file: {path}")


def copy_file(source, destination):
    require_file(source)
    destination.parent.mkdir(parents=True, exist_ok=True)
    copy2(source, destination)


def clean_build_outputs():
    for library in read_libraries().values():
        path = ADDON / library
        if path.is_dir():
            rmtree(path)
        elif path.is_file():
            path.unlink()


def stage_build(destination, platform, arch, target):
    key = f"{platform}.{target[len('template_'):]}"
    if platform not in ("macos", "ios"):
        key += f".{arch}"
    library = read_libraries().get(key)
    if library is None:
        raise SystemExit(f"No library declared for {platform} {arch} {target}")
    binary = library_file(library)
    copy_file(ADDON / binary, destination / ADDON_PATH / binary)


def assemble(destination):
    addon = destination / ADDON_PATH
    libraries = read_libraries()
    for library in libraries.values():
        require_file(addon / library_file(library))

    copy_file(ADDON / MANIFEST, addon / MANIFEST)
    for value in read_manifest()["icons"].values():
        icon = MANIFEST.parent / loads(value)
        copy_file(ADDON / icon, addon / icon)
    translations = sorted((ADDON / "translations").glob("*.po"))
    if not translations:
        raise SystemExit("Missing add-on translations")
    for source in translations:
        copy_file(source, addon / source.relative_to(ADDON))

    print(f"Verified {len(libraries)} libraries in {addon}")


def archive(source, destination):
    destination.parent.mkdir(parents=True, exist_ok=True)
    files = sorted(path for path in source.rglob("*") if path.is_file())
    if not files:
        raise SystemExit(f"No package files in {source}")
    with ZipFile(destination, "w", ZIP_DEFLATED, compresslevel=9) as output:
        for path in files:
            output.write(path, path.relative_to(source).as_posix())
    with ZipFile(destination) as output:
        corrupt = output.testzip()
        if corrupt is not None:
            raise SystemExit(f"Corrupt archive entry: {corrupt}")
    print(f"Created {destination} with {len(files)} files")


def main():
    parser = ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("clean")
    stage = commands.add_parser("stage")
    stage.add_argument("destination", type=Path)
    stage.add_argument("platform")
    stage.add_argument("arch")
    stage.add_argument("target", choices=("template_debug", "template_release"))
    merge = commands.add_parser("assemble")
    merge.add_argument("destination", type=Path)
    package = commands.add_parser("archive")
    package.add_argument("source", type=Path)
    package.add_argument("destination", type=Path)
    args = parser.parse_args()

    if args.command == "clean":
        clean_build_outputs()
    elif args.command == "stage":
        stage_build(args.destination, args.platform, args.arch, args.target)
    elif args.command == "assemble":
        assemble(args.destination)
    else:
        archive(args.source, args.destination)


if __name__ == "__main__":
    main()
