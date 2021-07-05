1. Run "make windows_debugging" to generate the windows-debugging folder from windows-debugging-templates.
2. Installations:
2.1. Visual Studio.
2.2. Instlal Windows Subsystem for Linux
3. Do the following:
3.0. Compile the project and run it:
3.0.1. 'make clean'
3.0.2. 'make qemu-nox-gdb'
3.0.3. if qemu doesn't start paused (the system boots), run: 'make pause_debug=true qemu-nox-gdb'
3.1. Run the following in the visual studio command in the command window (CTRL+ALT+A to open) and make sure to adjust the path:
3.1.1. alias d Debug.MIDebugLaunch /Executable:C:/ /OptionsFile:[project_root_path]/windows-debugging/options.xml
3.1.2. alias e Debug.MIDebugExec
3.2. Run the command "d" which we defined as alias for debug launch, in the command window.
3.3. Stop the debugger using the pause button.
3.4. Run the command "e zstartw" (z is just an arbitrary unique prefix, w stands for windows) in the command window.
3.5. Perform a single step in the assembly visual studio window.
3.6. Close the assembly visual studio window and press the current
instruction button which should open the source line in the kernel
source code.
