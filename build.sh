#!/bin/bash

echo "🚀 Compilando The Great Escape (DS Port)..."

sudo docker run --rm \
  -v "$(pwd):/source" \
  -w /source \
  devkitpro/devkitarm \
  #bash -c "dkp-pacman -Syu --noconfirm nds-dev && make && chown $(id -u):$(id -g) *.nds *.elf"
  bash -c "make && chown $(id -u):$(id -g) *.nds *.elf"
