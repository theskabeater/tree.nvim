# Tree.nvim - File explorer powered by C++.

[![Build Status](https://circleci.com/gh/zgpio/tree.nvim.svg?style=svg)](https://circleci.com/gh/zgpio/tree.nvim)
[![Build Status](https://travis-ci.org/zgpio/tree.nvim.svg?branch=master)](https://travis-ci.org/zgpio/tree.nvim)
[![Build status](https://ci.appveyor.com/api/projects/status/v0tb04id681b49xx/branch/master?svg=true)](https://ci.appveyor.com/project/zgpio/tree-nvim/branch/master)

🎉: `QT` dependencies removed (2020-03-21)

## Requirements
- [NeoVim nightly](https://github.com/neovim/neovim/releases/tag/nightly)

## Features
- NeoVim line-based highlight (`conceal` free).
- High performance.
- Defx's UI/API (minor revision).
- For more information refer to [doc](src/app/viml/doc/tree.txt).

## Usage
![Demo](https://user-images.githubusercontent.com/19503791/86912092-f3326f00-c14e-11ea-9d98-b65563c1bd6c.png)

### Config
Ref to [config.vim](src/app/dev.vim).

## Build
```sh
git clone https://github.com/zgpio/tree.nvim
brew install boost
brew info boost

cmake -DCMAKE_INSTALL_PREFIX=../INSTALL -DBoost_USE_STATIC_LIBS=ON -DCMAKE_BUILD_TYPE=Release -S ~/project/tree.nvim/ -B ~/project/tree.nvim/build
make -C ~/project/tree.nvim/build/
```
