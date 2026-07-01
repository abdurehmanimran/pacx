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
make build
```
For optimized builds, use `build-release` instead of `build`.
```
make build-release
```
The release binary can be installed as:

```
sudo make install
```
Otherwise, in case of debug build (simple build) as:
```
sudo make install-debug
```

## See It In Action
<img width="1908" height="1022" alt="image" src="https://github.com/user-attachments/assets/35b55460-4c3b-4db9-8a4e-dd6f9b1a469c" />

<img width="1908" height="1022" alt="image" src="https://github.com/user-attachments/assets/3383b1fc-624e-4644-b495-1cd38be3472b" />

## Why C?
Just so that I could learn and practice C.

## Credit
- `aria2` for making ultra-fast downloads of packages possible.
- `pacman` for the solid foundation.
- `powerpill` and `nala` for inspiring the beautiful and elegant design.
