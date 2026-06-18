// This fallback is only for non-Unity compilation contexts (e.g. analyzers/IDE builds).
#if !UNITY_5_3_OR_NEWER
using System;

namespace AOT
{
    [AttributeUsage(AttributeTargets.Method)]
    public sealed class MonoPInvokeCallbackAttribute : Attribute
    {
        public MonoPInvokeCallbackAttribute(Type type)
        {
        }
    }
}
#endif
