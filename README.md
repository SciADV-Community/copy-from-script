# copy-from-script
Simple utility that copies lines sequentially from a script when a push-button or when a controller-key is pressed.

## Functionality and Justification
The reason this exists is because certain Visual Novels that run on console or on emulator are simply not text-hookable whatsoever, 
but through file dumps there is access to their scripts in plaintext. 
Thus, a text-hooking solution for these cases would be to have a utility copy the next line from the script every time you press the button
in-game to go to the next line. It's a relatively dumb way to get around this issue, but hey, it works.
This utility does exactly that, and even picks up controller input, meaning that just pressing the button to progress the game should progress the copy-from-script program too.

## Usage
To open a script file, simply press `Open` on the menu-bar or `Ctrl+O`. After that you can input a line number to jump to in the appropriate
text-box and press enter. Clicking "Next" should display and copy the next line from the script. Pressing the eastern button on the controller
(O for DualShock, B for XBOX) should do exactly this as well. If you wish the southern button (X in DualShock, A in XBOX) to progress the script instead, tick the appropriate check-box.
Finally, you can save the current progress on the script you're going through by pressing `Save` or `Ctrl+S`.

There are some additional controller inputs though. Pressing the Up Arrow on the D-Pad should also progress the script. 
Additionally, pressing the Left Arrow on the D-PAD toggles `PAUSE` mode in the script, which ignores any controller inputs. This is certainly helpful for when you want to go through menues and don't want the script to be picking up controller input.

## Additional Usage Instructions
It would be advised to sort game scripts by chapter into their separate folder. That way the script can continue onto the next line etc and you only need to change the script its looking at once you finish a chapter.
The reason having them all in the same 1 folder is because the chapter order might be messed up.

## Technologies used
The project was written in Qt 5 in C++ and relies on XInput drivers.
