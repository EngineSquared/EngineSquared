#!/bin/bash
set -e

rm -rf ./src/plugin/template/
xmake scaffold --create
xmake f --ExampleName=y
xmake build ExampleName
xmake run ExampleName
xmake build TestFile
xmake run TestFile
