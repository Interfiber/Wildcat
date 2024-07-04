# Installing Wildcat

## Dependencies
* CMake
* Ninja
* Git

## Step 1. Clone the repo
First clone the repo using
```bash
git clone https://github.com/Interfiber/Wildcat.git
```

## Step 2. Clone the dependencies
Now inside of the ```Wildcat``` folder use the ```installdeps.sh``` script to clone required dependencies

## Step 3. Build
```shell
cd build
cmake -S .. -B . -GNinja
ninja
```

## Step 4. Install
```shell
sudo ninja install
```
Now wildcat has been installed to ```/usr/local/share```