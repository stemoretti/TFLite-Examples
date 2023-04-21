#!/bin/sh

set -e

cd $(dirname "$0")

git clone --depth 1 https://github.com/stemoretti/BaseUI
git clone --depth 1 -b v2.10.1 https://github.com/tensorflow/tensorflow
