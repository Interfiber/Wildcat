# Common Problems

## Failed to open serial device from: /dev/ttyACM[...]
This usually occurs when you are not in the ```dialout``` group, you can add yourself to this group using:
```shell
sudo usermod -a -G dialout $(whoami)
```
Then logout to apply the changes

## The font looks pixelated!!
This might be because resource couldn't be loaded, make sure all font files are located in ```/usr/local/share/wildcat/resources/fonts```

## I don't have root access! How can I use wildcat?
Sadly wildcat requires root access in order to run the wildcatdriver, this executable only needs to be ran once for every machine boot. If you can get a superuser to execute this file then you can set ```WILDCAT_NO_DRIVER``` to disable launching the driver

## My scanner is on a serial port other than 0!
You can set the environment variable ```WILDCAT_SERIAL_PORT``` to the port number in order to change this!

## Wildcat could not find a suitable polkit wrapper to use!
Please make sure you have ```pkexec```, or ```kdesu``` installed and in your PATH

## I have a problem which isn't listed here!
Please submit an issue on GitHub :)