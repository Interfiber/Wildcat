# Wildcat
Graphical Uniden BC125AT programmer for Linux 

<p float="left">
    <img src="./preview_light.png?raw=true" height="280" />
    <img src="./preview_dark.png?raw=true" height="280" />
</p>

## Features
* Load channels from the scanner
* Write channels to the scanner
* Easy to use UI
* Save/load channels from disk
* Ability to erase the scanners memory

## Documentation
For more information on now to install Wildcat see the documentation located [here](https://www.interfiber.dev/wildcat)

## Notice!
Wildcat will use root privileges once to create the scanners device files, in order for an ordinary user to write to them you must be in the ```dialout``` group.
You can add yourself to this group using:
```bash
sudo usermod -a -G dialout $(whoami)
# Make sure to logout!
```
