# Summary
Fix logind suspending while not idle if you're running a window manager or
desktop environment under the X.Org Server.

Prepend to the command you use to start your window manager or desktop
environment.

To run bspwm with the following `.xinitrc`:
```
exec dbus-run-session bspwm
```
you need to do:
```
setsessiontypeforwm "startx"
```

This will only work with elogind.

# Explanation
logind has idle management, in particular the options `IdleAction` and
`IdleActionSec`. It can do this for tty, for the X.Org Server and for Wayland.
By default your session will be of the `tty` type. It could then be changed if
you start your window manager/desktop environment/compositor from a display
manager. From there, a session property called `IdleHint` could be set to `yes`,
(by, for example, `xss-lock`) and logind would know your session is idle and
then perform the configured idle actions.

If you wanted to run logind without a display manager, you had to disable the
idle management because your session would never get changed from `tty`, and
would become idle after some time had passed, no matter how active you were.

Since systemd v246 the session type can be changed. wlroots and, by extension,
sway [already do this](https://github.com/swaywm/wlroots/pull/2304).

bspwm does not. As far as I can tell it doesn't interact with logind at all.

I have mostly switched to sway, but since the anti-Wayland horseshit is still
plaguing the Wayland desktop, I have to use the X.Org Server every now and then.

I'd like to make use of logind's idle management and it works well under sway,
but under bspwm my desktop suspends every `IdleActionSec`.

This little program manually sets the session type to `x11` and then back to
`tty` once your command exits.
