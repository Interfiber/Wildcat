#!/usr/bin/env bash

sudo cp -v bin/wildcat /usr/local/bin/wildcat
sudo cp -v bin/wildcatuserdriver /usr/local/bin/wildcatdriver

sudo cp -rv resources /usr/local/share/wildcat
sudo cp -v Wildcat.desktop /usr/local/share/applications

sudo xdg-mime install --mode system wildcat-mime.xml
sudo xdg-mime default Wildcat.desktop application/x-wcat