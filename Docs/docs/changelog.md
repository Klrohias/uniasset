# 更新日志

## v0.2.1-rc1

### 新增

- `AudioAsset` 新增 `try_clone` 方法

### 改进

- 引入 Cargo 工作区结构
- 使用 `uniasset-bridge` 统一 FFI 导出
- 使用安全的 `Option` 替代 `AtomicPtr`
- 无锁 PCM 读取性能优化

### 修复

- 修复 CI 工作流问题

## v0.2.0

**当前版本**

### 改进

- 完成 `v0.2.0` 正式版发布
- 优化原生层的内存安全与线程安全
- 调整项目目录结构

### 修复

- 修复 `Uniasset.Editor` 符号链接相关问题
- 修复 `CancellationTokenSource` 的释放问题
- 修复使用 `stbi` 后端时图片 Y 轴翻转问题

## v0.2.0-rc11

### 变更

- 发布 `v0.2.0-rc11` 候选版本

## v0.2.0-rc10

### 修复

- 修复 `Uniasset.Editor` 符号链接相关问题
- 修复 `CancellationTokenSource` 的释放问题
- 修复使用 `stbi` 后端时图片 Y 轴翻转问题

## v0.2.0-rc9

### 新增

- 新增文档站点与使用指南
- C# SDK 新增 `ResizeFilter`

### 改进

- 将 `Uniasset.Editor` 调整为使用符号链接组织

## v0.2.0-rc8

### 新增

- 编辑器工具（Editor Tools）：
  - **Download Native Libraries**：一键下载对应平台的原生库
  - **Configure Native Libraries**：自动配置原生库的平台导入设置

### 修复

- MP3（MPEG-2 Layer 3）格式支持
- 默认采样格式改进

## v0.2.0-rc7

### 修复

- 修复 Android 平台静态链接 `libc++` 的问题

## v0.2.0-rc6

### 新增

- `AudioAsset` 新增 `Unload` 方法

## v0.2.0-rc5

### 改进

- 减小原生库二进制体积

## v0.2.0-rc4

### 新增

- 新增音频 Seek 能力

## v0.2.0-rc3

### 修复

- 修复缓冲区大小导致的 panic 问题

## v0.2.0-rc2

### 新增

- 新增基于 Symphonia 的音频解码封装

## v0.2.0-rc1

### 新增

- 引入新的 Rust 原生实现分支并完成首个 `0.2.0` 候选版本整合

## v0.1.1

### 新增

- 新增外部解码器支持
- 新增托管音频解码器支持
- 新增 `ImageAsset::CropMultiple`

### 改进

- Linux 平台增加符号裁剪，减小发布体积
- 同步 Unity C# 脚本

### 修复

- 修复头文件包含问题
- 修复 `Result` 移动构造问题
- 修复 MP3 文件魔数识别问题
- 修复发布压缩包流程问题

## v0.1.0-fix3

### 修复

- 修复 `setTime` 缺失符号问题
- 修复大量帧读取时 `BufferedAudioDecoder` 的异常
- 修复 iOS 构建与符号导出问题

## v0.1.0-fix2

### 改进

- 发布时附带 Unity 脚本与 Windows 符号文件
- 补充 README 与 C# 绑定参数命名修正

## v0.1.0-fix1

### 修复

- 修复 `readinternal` 中的 SEH 问题

## v0.1.0

### 新增

- 新增 `ToAudioClip`
- 新增 `BufferedAudioDecoder`
- 增加自动发布流程

### 改进

- 使用 `std::shared_ptr` 重构对象实例管理

### 修复

- 修复 iOS 符号表相关问题
- 修复 Windows CI 与头文件包含问题

## v0.1.0-preview2

### 修复

- 修复 Windows CI 问题

## v0.1.0-preview1

### 修复

- 修复 Android、macOS 与 Linux 平台的 CI 问题
