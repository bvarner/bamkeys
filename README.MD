# BamKeys - Spatial Keyboard-Based Window Navigation for BeOS R5

BamKeys is an input_server filter for BeOS R5 that lets you spatially map your display for navigating windows with your keyboard.
This code was hacked up in 2005, and could use a little scrubbing, but isn't too bad.

## What it does

BamKeys let's you break your display up into a grid of zones. Each zone is assigned a keyboard combination. Whenever you press that keyboard combination, BamKeys will focus the front-most window that intersects with that zone, regardless of the program it belongs to! Pressing the same zone's keyboard combination repeatedly will focus widows further back. If you have three windows that intersect a zone, pressing that zones hotkey three times will cycle through all those windows.

It's a pretty simple concept, really.

## Installation Instructions

1.) Copy the BamKeysFilter into
	/boot/home/config/add-ons/input_server/filters
	Or just drop the file on the linked folder.
2.) Restart the input_server like so
	/boot/beos/system/servers/input_server -q
	Or just double-click the Double-Click Me file.

## Configuring
Open up the BamKeys Application (it's a preference tool)
At this point, you'll see a little display with a 3x3 grid, a check box, two pop up menus, and a bunch of key cap looking things.

The default key mappings for this 3x3 grid are: the left "command" (or alt) key + numeric keypad 1 through 9 (with NumLock ON).

To change these settings:

Using the mouse:

Click the zone on the little display you want to change. It will darken. Press the key combination you want to use. Click the zone again to de-select it. Notice that the key caps things on the side will reflect the keys you've pressed (once you let go of them that is).

Using the keyboard:

Tab until the screen is selected. Press the space-bar. A zone will darken in the grid. You can use the arrow keys to change the selected zone. Pressing a keyboard shortcut will set the hotkey for that zone. Pressing space bar will unselect all zones.

I should also mention (there's no interface for setting this yet) but if you press (and hold) the left "command" (alt) and the "enter" from the numeric keypad you'll probably like what you see.

## History:

Back in the day I was chatting with Jonathan Thompson on BeShare about some features he planned for a BeOS IDE he was planning, named Project X.
He mentioned somethings about spatially mapping keyboard navigation to windows on screen. The more he described it, the more I thought, "This should be more than just Project X... This should be OS-Wide." Keep in mind that OS X's Expose feature was pretty hot at the time.

I did an input_server device addon a few years earlier and really enjoyed it. At the time I was talking with Jonathan, I was working on implementing keyboard handling and the focus sub-system of the AWT for the Java port to BeOS.. so this was all intersecting with interests and problems I was already working with. I hadn't do anything with keyboard events in BeOS before, and this sounded like a great project to get my feet wet with. I volunteerd (or did I demand?) to write the code and see if it could work well.

Since I was targeting R5, and not the exp/dano releases, the app_server didn't have true compositing of transparent regions of overlay windows, so to 'simulate' that effect for the 'ctrl-enter' shortcut, I have it resizing the window to draw the grid to full screen and taking a bitmap screenshot of the screen, then drawing that as the background of the window that the grid is drawn into. It gives the 'appearance' of an overlay, without it being a true, live compositied overlay. The kludge to wait a half-second if the workspace switches still makes me cringe 11 years later. :-)

It's been more than 10 years since I hacked this stuff together, and it was so much fun to write that I feel it needs to be 'freed' from the confines of history and my few copies that I have floating around.
