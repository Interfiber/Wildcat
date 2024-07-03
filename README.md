# Wildcat
Open-Source Uniden BC125AT GUI programmer for Linux 

## Notice!
Wildcat will use root privileges once to create the scanners device files, in order for an ordinary user to write to them you must be in the ```dialout``` group.
You can add yourself to this group using:
```bash
sudo usermod -a -G dialout $(whoami)
# Make sure to logout!
```
