AutoRun
--------
A windows program which adds autorunning (or automatic key holding of any kind) universally
to all games. To use it, just hold down the keys you would like to stay pressed and then
press the PAUSE key. You can now take your hand off the keyboard, and the keys will remain
pressed until you press a key on the keyboard again.

### How it works
AutoRun monitors the status of the keyboard using a [low level keyboard hook](https://docs.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc). 
When the hotkey PAUSE/BREAK is pressed, it takes all currently held keys (listed in src/vkarray.cpp) 
and holds them down. Keys are then released when **any** key is pressed on the keyboard.

### Building
#### Dependencies
* Visual Studios C/C++ (recommended 2015 or above)
* CMake 3.1 or newer
* Git

#### Build process
1. Clone the source
    ```
    git clone https://github.com/divisionind/AutoRun.git
    cd AutoRun
    ```
2. Generate the build files with CMake
    * Release: `cmake . -DRELEASE_BUILD=1`
    * Debug: `cmake . -DRELEASE_BUILD=0` or `cmake .`
    * _NOTE: You can specify the `-G` option to cmake to choose a specific compiler. See `cmake -h` 
      for a list of compilers installed on your system._
3. Open the build files
    * For visual studios this will be the `AutoRun.sln` file.
    * Ensure you have Debug or Release selected in the editor as appropriate then go to the menu
      bar and select `Build->Rebuild Solution`
4. The executable will be located in `[Release or Debug]/AutoRun.exe`
5. If you would like AutoRun to start automatically when you start Windows, create a shortcut to
   the program and place it in `%appdata%/Microsoft/Windows/Start Menu/Programs/Startup`

### Other notes
My main reason for writing this was to explore low level inputs on Windows so how
I do stuff here is not the best or easiest in all cases but instead its in service of 
that original idea.

### Donate
- XMR: `83vzgeeKebLh6pj2YtBqn7PqxY47CkyzmLzUhmHfhTCQdj9Mfad4FUF12Yu9ry5uUh5JASTcXg5Fwji5ibjUngw9LomnH6Z`
- ETH: `0x1bdA7dB6484802DFf4945edc52363B4A8FAcb470`
- ETC: `0x4a368bb4cd854f650169ce207268c303ffecafb2`
