# Changelog

## v0.2.1-rc1

### Added

- Added `try_clone` to `AudioAsset`

### Improvements

- Introduced Cargo workspace structure
- Unified FFI exports with `uniasset-bridge`
- Replaced `AtomicPtr` with safe `Option`
- Lock-free PCM read performance optimization

### Fixes

- Fixed CI workflow issues

## v0.2.0

**Current version**

### Improvements

- Released the final `v0.2.0`
- Improved native-layer memory safety and thread safety
- Adjusted the project directory layout

### Fixes

- Fixed `Uniasset.Editor` symbolic link issues
- Fixed `CancellationTokenSource` disposal
- Fixed Y-axis flipping when using the `stbi` image backend

## v0.2.0-rc11

### Changes

- Released `v0.2.0-rc11`

## v0.2.0-rc10

### Fixes

- Fixed `Uniasset.Editor` symbolic link issues
- Fixed `CancellationTokenSource` disposal
- Fixed Y-axis flipping when using the `stbi` image backend

## v0.2.0-rc9

### Added

- Added the documentation site and usage guides
- Added `ResizeFilter` to the C# SDK

### Improvements

- Reorganized `Uniasset.Editor` to use symbolic links

## v0.2.0-rc8

### Added

- Editor tools:
- **Download Native Libraries** for one-click native library downloads
- **Configure Native Libraries** for automatic importer configuration

### Fixes

- MP3 (MPEG-2 Layer 3) format support
- Default sample-format behavior

## v0.2.0-rc7

### Fixes

- Fixed Android static linking against `libc++`

## v0.2.0-rc6

### Added

- Added `Unload` to `AudioAsset`

## v0.2.0-rc5

### Improvements

- Reduced native binary size

## v0.2.0-rc4

### Added

- Added audio seeking support

## v0.2.0-rc3

### Fixes

- Fixed a panic caused by buffer sizing

## v0.2.0-rc2

### Added

- Added Symphonia-based audio decoding integration

## v0.2.0-rc1

### Added

- Introduced the new Rust-native implementation branch and shipped the first `0.2.0` release candidate

## v0.1.1

### Added

- Added external decoder support
- Added managed audio decoder support
- Added `ImageAsset::CropMultiple`

### Improvements

- Reduced Linux release size with symbol stripping
- Synced Unity C# scripts

### Fixes

- Fixed header include issues
- Fixed `Result` move-constructor behavior
- Fixed MP3 magic-number detection
- Fixed release archive packaging

## v0.1.0-fix3

### Fixes

- Fixed missing `setTime` symbol
- Fixed `BufferedAudioDecoder` issues during large frame reads
- Fixed iOS build and symbol export issues

## v0.1.0-fix2

### Improvements

- Bundled Unity scripts and Windows symbol files in releases
- Improved README coverage and corrected C# binding parameter names

## v0.1.0-fix1

### Fixes

- Fixed the SEH issue in `readinternal`

## v0.1.0

### Added

- Added `ToAudioClip`
- Added `BufferedAudioDecoder`
- Added the automated release workflow

### Improvements

- Refactored instance management with `std::shared_ptr`

### Fixes

- Fixed iOS symbol-table issues
- Fixed Windows CI and header include issues

## v0.1.0-preview2

### Fixes

- Fixed Windows CI issues

## v0.1.0-preview1

### Fixes

- Fixed CI issues on Android, macOS, and Linux
