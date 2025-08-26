**dwm - dynamic window manager**
============================
dwm is an extremely fast, small, and dynamic window manager for X.


**Requirements:**
------------
In order to build dwm you need the Xlib header files.

This dwm is already patched. I left the patches' files here for you (and me) to know which patches I use in my dwm config.

_You just need to compile it._

Optionally you need:

[spm](https://github.com/brookiestein/spm/)

[slock](https://github.com/brookiestein/slock/)

[dmenu](https://github.com/brookiestein/dmenu/)

Among other utilities.

**Note that there's a number of keyboard shortcuts you may not need.**

In that case, delete them.

**If you use this setting, these keyboard shortcuts might interest you:**
```
# For this keyboard shortcuts you will need: syspoweradmin
Windows key + Shift key + Delete = Show a window with power options

# Check out what xinput shows and change id in the config.h for your touchpad
Windows key + Control key + Period = Enable touchpad
Windows key + Control key + Comma  = Disable touchpad
```
**In this configuration the keyboard shortcut to close dwm itself was changed to:**
```
Windows key + Shift key + e
```
**And to close a window:**
```
Windows key + Shift key + q
```

**In regards to the status bar. Look at my repository of 
[dotfiles.](https://github.com/brookiestein/dotfiles/tree/master/.config/dwm/)**

**Also, you can use [slstatus.](https://github.com/brookiestein/slstatus/)**

**Installation**
------------
Edit config.mk to match your local setup (dwm is installed into
the /usr/local namespace by default).

Take special care when patching. You probably need to patch some manually.
Afterwards enter the following command to build and install dwm (if
necessary as root):
```
# make clean install
```

**Running dwm**
-----------
Add the following line to your .xinitrc to start dwm using startx:
```
exec /usr/bin/local/dwm
```
In order to connect dwm to a specific display, make sure that
the DISPLAY environment variable is set correctly, e.g.:
```
DISPLAY=foo.bar:1 exec dwm
```
(This will start dwm on display :1 of the host foo.bar.)

**How will the status bar be shown, will depend on which you had chose.**

If you chose the first option, it's say:
My repository of [dwmrc](https://github.com/brookiestein/dotfiles/tree/master/.config/dwm/)

In that case you can do something like this in your .xinitrc:
```
DIR=${HOME}/.dwm
if [ -f "${DIR}"/dwmrc ]; then
        /bin/sh "${DIR}"/dwmrc &
else
        while xsetroot -name "`date` `uptime | sed 's/.*,//'`"; do
              sleep 1
        done &
fi
exec dwm
```

In case your choice had been [slstatus:](https://github.com/brookiestein/slstatus)

Add this value to the autostart array in config.h
```
slstatus
```

**Configuration**
-------------
The configuration of dwm is done by creating a custom config.h
and (re)compiling the source code.
