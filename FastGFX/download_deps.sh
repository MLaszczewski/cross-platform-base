#!/usr/bin/env bash

cd js
  npm install
cd ..

mkdir -p deps
cd deps
  git clone https://github.com/g-truc/glm
  cd glm
    git checkout 0.9.7.5
  cd ..
cd ..
