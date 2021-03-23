clear
scons -j14 custom_modules=./modules/dynamic_fluids platform=x11 target=debug
cd bin 
./godot.linuxbsd.tools.64
