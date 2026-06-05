using System;
using System.IO;
using System.IO.Compression;
using System.Net.Http;
using UnityEditor;
using UnityEngine;

namespace Uniasset.Editor
{
    public static class NativeLibraryDownloader
    {
        private const string ZipFileName = "uniasset-native-libraries.zip";

        [MenuItem("Tools/Uniasset/Download Native Libraries")]
        private static async void DownloadNativeLibraries()
        {
            var targetDir = Path.Combine(Application.dataPath, "Plugins", "Uniasset", "Native");

            if (Directory.Exists(targetDir))
            {
                if (!EditorUtility.DisplayDialog(
                    "Download Native Libraries",
                    $"Target directory already exists:\n{targetDir}\n\nOverwrite?",
                    "Overwrite",
                    "Cancel"))
                {
                    return;
                }
                Directory.Delete(targetDir, true);
            }

            var downloadUrl = $"{Config.GitHubRepoURL}/releases/download/{Config.Version}/{ZipFileName}";
            var tempZipPath = Path.Combine(Path.GetTempPath(), ZipFileName);

            try
            {
                EditorUtility.DisplayProgressBar("Download Native Libraries", $"Downloading from {downloadUrl}...", 0.5f);

                using var client = new HttpClient();
                var response = await client.GetAsync(downloadUrl);
                response.EnsureSuccessStatusCode();

                var zipBytes = await response.Content.ReadAsByteArrayAsync();
                File.WriteAllBytes(tempZipPath, zipBytes);

                EditorUtility.DisplayProgressBar("Download Native Libraries", "Extracting...", 0.8f);

                Directory.CreateDirectory(targetDir);
                ZipFile.ExtractToDirectory(tempZipPath, targetDir);

                AssetDatabase.Refresh();

                Debug.Log($"[Uniasset] Native libraries downloaded and extracted to: {targetDir}");
                EditorUtility.DisplayDialog("Download Native Libraries", "Done!", "OK");
            }
            catch (Exception ex)
            {
                Debug.LogError($"[Uniasset] Failed to download native libraries: {ex.Message}");
                EditorUtility.DisplayDialog("Download Native Libraries", $"Failed:\n{ex.Message}", "OK");
            }
            finally
            {
                EditorUtility.ClearProgressBar();
                if (File.Exists(tempZipPath))
                    File.Delete(tempZipPath);
            }
        }
    }
}
