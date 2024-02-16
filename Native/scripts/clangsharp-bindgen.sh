#!/bin/bash

scriptPath=$(dirname $0)

pushd $scriptPath/..

ClangSharpPInvokeGenerator @./scripts/clangsharp-binding.rsp

pushd ClangSharpGenerated

# DllImport("uniasset_platform_dynamic_binding",
cat Interop.cs | sed -e 's#\[DllImport[(]"uniasset_platform_dynamic_binding",#\[DllImport\(NativeLibrary.LibraryName,#g' > Interop.cs

popd

popd