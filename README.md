[![Linux CI](https://github.com/Pugnator/FagotronBot/actions/workflows/linux_build.yml/badge.svg)](https://github.com/Pugnator/FagotronBot/actions/workflows/linux_build.yml)
[![Windows CI](https://github.com/Pugnator/FagotronBot/actions/workflows/windows_build.yml/badge.svg)](https://github.com/Pugnator/FagotronBot/actions/workflows/windows_build.yml)

# FagotronBot
A simple text based rulette bot for a Telegram chats.

# Build:
## Under Windows

 - Install https://www.msys2.org/
 - Run msys2 console
 - Install next packages
```
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-boost
```

- Clone repo:
```
$ git clone git@github.com:Pugnator/FagotronBot.git FagotronBot
```
- Recursive update submodules
```$ cd FagotronBot/
$ git submodule update --init --recursive
```
- Create token.txt with your BOT_TOKEN
```
echo "YOUR_SECRET_TOKEN" > token.txt
```
- Run build.sh script
```
./build.sh
```
