using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEditor;
using UnityEngine;

namespace Uniasset.Editor
{
    public static class NativeLibraryConfigurator
    {
        private static readonly Dictionary<string, string> ArchToCpu = new()
        {
            ["aarch64"] = "ARM64",
            ["arm-neon"] = "ARMv7",
            ["x64"] = "x86_64",
            ["x86"] = "x86",
        };

        private static readonly Dictionary<string, BuildTarget> PlatformToBuildTarget = new()
        {
            ["android"] = BuildTarget.Android,
            ["ios"] = BuildTarget.iOS,
            ["macos"] = BuildTarget.StandaloneOSX,
            ["linux"] = BuildTarget.StandaloneLinux64,
            ["windows"] = BuildTarget.StandaloneWindows64,
        };

        [MenuItem("Tools/Uniasset/Configure Native Libraries")]
        private static void ConfigureNativeLibraries()
        {
            var nativeDir = Path.Combine(Application.dataPath, "Plugins", "Uniasset", "Native");
            if (!Directory.Exists(nativeDir))
            {
                Debug.LogError($"[Uniasset] Native library directory not found: {nativeDir}");
                EditorUtility.DisplayDialog("Configure Native Libraries",
                    $"Native library directory not found:\n{nativeDir}\n\nRun 'Download Native Libraries' first.",
                    "OK");
                return;
            }

            var configuredCount = 0;
            var dirs = Directory.GetDirectories(nativeDir, "*", SearchOption.AllDirectories);

            foreach (var dir in dirs)
            {
                var dirName = Path.GetFileName(dir);
                var (arch, platform) = ParseDirectoryName(dirName);
                if (arch == null || platform == null)
                    continue;

                var libFiles = Directory.GetFiles(dir).Where(f => !f.EndsWith(".meta")).ToArray();
                foreach (var libPath in libFiles)
                {
                    if (ConfigureLibrary(libPath, platform, arch))
                        configuredCount++;
                }
            }

            AssetDatabase.Refresh();

            // Write .gitignore to exclude downloaded native binaries from VCS
            WriteGitIgnore(nativeDir);

            Debug.Log($"[Uniasset] Configured {configuredCount} native libraries.");
            EditorUtility.DisplayDialog("Configure Native Libraries",
                $"Configured {configuredCount} native libraries.", "OK");
        }

        private static void WriteGitIgnore(string nativeDir)
        {
            var gitignorePath = Path.Combine(nativeDir, ".gitignore");
            var content = "# Downloaded native libraries — do not track\n*\n!.gitignore\n";
            File.WriteAllText(gitignorePath, content);
        }

        private static (string arch, string platform) ParseDirectoryName(string dirName)
        {
            // Expected format: {prefix}{arch}-{platform}
            // e.g. "libuniasset-aarch64-android", "uniasset-x64-windows", "libuniasset-arm-neon-android"
            // Match known platform suffix first, then known arch suffix from remainder

            foreach (var plat in PlatformToBuildTarget.Keys)
            {
                var platSuffix = $"-{plat}";
                if (!dirName.EndsWith(platSuffix))
                    continue;

                var remainder = dirName[..^platSuffix.Length];
                foreach (var a in ArchToCpu.Keys)
                {
                    var archSuffix = $"-{a}";
                    if (remainder.EndsWith(archSuffix))
                        return (a, plat);
                }
            }

            return (null, null);
        }

        private static bool ConfigureLibrary(string libPath, string platform, string arch)
        {
            var relativePath = GetRelativePath(libPath);
            var importer = AssetImporter.GetAtPath(relativePath) as PluginImporter;
            if (importer == null)
            {
                Debug.LogWarning($"[Uniasset] No PluginImporter for: {relativePath}");
                return false;
            }

            var buildTarget = PlatformToBuildTarget[platform];

            // Disable all platforms
            importer.SetCompatibleWithAnyPlatform(false);
            importer.SetCompatibleWithEditor(false);

            // Enable target platform
            importer.SetCompatibleWithPlatform(buildTarget, true);

            // Set CPU architecture
            importer.SetPlatformData(buildTarget, "CPU", ArchToCpu[arch]);

            importer.SaveAndReimport();
            return true;
        }

        private static string GetRelativePath(string absolutePath)
        {
            // Normalize to forward slashes — Unity asset paths use "/"
            var normalized = absolutePath.Replace('\\', '/');
            var projectRoot = Path.GetDirectoryName(Application.dataPath)?.Replace('\\', '/');

            if (projectRoot != null && normalized.StartsWith(projectRoot + "/"))
                return normalized[(projectRoot.Length + 1)..];

            return normalized;
        }
    }
}
