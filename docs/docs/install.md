# Installing Wildcat

## Dependencies
* [Mesa build](https://github.com/Interfiber/Mesa.git)
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
mesa build
make -j
```

## Step 4. Install
```shell
sudo sh install.sh
```
Now wildcat has been installed to ```/usr/local``` and can be launched using ```wildcat``` or using its installed desktop launcher entry