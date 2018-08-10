#!/usr/bin/env bash

cmake .
make

cd js
  npm install
  webpack
cd ..
