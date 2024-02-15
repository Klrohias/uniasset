#!/bin/bash

scriptPath=$(dirname $0)

pushd $scriptPath/..

ClangSharpPInvokeGenerator @./scripts/clangsharp-binding.rsp

popd