# Pacx
A simple blazing-fast Pacman wrapper inspired by **Powerpill** and **Nala** that makes package management feel effortless and enjoyable.

This lightweight wrapper brings the speed of parallel downloads to your Arch Linux system using `aria2c`, while keeping things simple and elegant and having a beautiful and intuitive interface inspired by **Nala**

## Installation
The package can be directly installed through any AUR helper:
```
yay -S pacx
```

## Building from Source
To compile the project from source code, clone the repository and run `make`. The binary will be inside the **bin** directory.

```
git clone https://github.com/abdurehmanimran/pacx.git
cd pacx
make
```
For optimized builds, use `build-release` instead of `build`.
```
make build-release
```
The binary can be installed as:

```
sudo make install
```

## See It In Action

<img width="1908" height="1022" alt="image" src="https://github.com/user-attachments/assets/1b18e5da-5083-495b-aa08-ef676d6ba71b" />


<img width="1908" height="1022" alt="image" src="https://github.com/user-attachments/assets/46330bb5-e4af-4694-a24f-72eae1d8b1d0" />


## Why C?
Just so that I could learn and practice C.

## Credit
- `aria2` for making ultra-fast downloads of packages possible.
- `pacman` for the solid foundation.
- `powerpill` and `nala` for inspiring the beautiful and elegant design.
