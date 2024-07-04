# Wildcat
Open-Source Uniden BC125AT GUI programmer for Linux 

<img align="right" src="./preview_dark.png?raw=true" height="280" />
<img align="right" src="./preview_light.png?raw=true" height="280" />

## Notice!
Wildcat will use root privileges once to create the scanners device files, in order for an ordinary user to write to them you must be in the ```dialout``` group.
You can add yourself to this group using:
```bash
sudo usermod -a -G dialout $(whoami)
# Make sure to logout!
```
