using System.Collections;
using System.IO;
using System.Threading.Tasks;
using UnityEngine;
using UnityEngine.Networking;
using Application = UnityEngine.Device.Application;

public static class Utils
{
    public static Task<byte[]> LoadStreamingAsset(string path, MonoBehaviour context)
    {
#if UNITY_ANDROID
        var tcs = new TaskCompletionSource<byte[]>();

        IEnumerator LoadCoroutine()
        {
            using var uwr = UnityWebRequest.Get(Application.streamingAssetsPath + "/" + path);

            yield return uwr.SendWebRequest();

            tcs.SetResult(uwr.downloadHandler.data);
            
        }

        context.StartCoroutine(LoadCoroutine());

        return tcs.Task;
#else
        return File.ReadAllBytesAsync(Application.streamingAssetsPath + "/" + path);
#endif
    }
}