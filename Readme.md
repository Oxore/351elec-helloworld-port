# Example native 351ELEC app

## Build

You need a toolchain or a virtual machine with such a toolchain first. The toolchain's [target triplet](https://wiki.osdev.org/Target_Triplet) should be `aarch64-libreelec-linux-gnueabi`. It must include `sysfs` with SDL2 library binaries. If you have already built 351ELEC system itself then you already have one. For example in my case I have 351ELEC local repo at `/home/oxore/build-pkgs/351ELEC` and toolchain for my RG351P is `/home/oxore/build-pkgs/351ELEC/build.351ELEC-RG351P.aarch64/toolchain` correspondingly.

Then you run the build command specifying your toolchain location explicitly.

```
make TOOLCHAIN_DIR=/home/oxore/build-pkgs/351ELEC/build.351ELEC-RG351P.aarch64/toolchain
```

Or you can export it as an environment variable

```
export TOOLCHAIN_DIR=/home/oxore/build-pkgs/351ELEC/build.351ELEC-RG351P.aarch64/toolchain
make
```

Or you can build it for the host machine to run on it. Then you should **not** use any toolchain, which usage is described above. Simply run:

```
make
```

If you already `export`ed toolchain path then you can clear `TOOLCHAIN_DIR` variable and run again:

```
make clean
export TOOLCHAIN_DIR=
make
```

## Install

Now you have the `main` executable file which can be copied to the device over SSH or by putting it onto SD card in a dedicated directory (e.g. `demo`) in `ports`.

The partition labeled as `GAMES` is mounted as `/storage/roms` directory in the virtual filesystem tree when 351ELEC is being run on the device. I will refer to it as `/storage/roms` or `/roms` directory further. You may think of this location as a root of the disk with label `GAMES` if you are working with SD card directory and not via SSH.

Create directory `/storage/roms/ports/demo`. When working with SD card directly it must be `<GAMES>/ports/demo`. Via SSH it may be done as follows:

```
ssh root@192.168.88.226 mkdir -p /storage/roms/ports/demo
```

Copy the executable `main` and necessary resources (`controls.png` and `font-manaspc`) into `/storage/roms/ports/demo` directory. When copying onto SD card it will be `<GAMES>/ports/demo`. Via SSH it may be done as follows:

```
scp -r main controls.png font-manaspc root@192.168.88.226:/storage/roms/ports/demo
```

The SSH password is `351elec`.

Copy file `DemoAppLauncher.sh` into `/storage/roms/ports` directory. When copying onto SD card it will be `<GAMES>/ports`. Via SSH it may be done as follows:

```
scp DemoAppLauncher.sh root@192.168.88.226:/storage/roms/ports/
```

Add the following entry into the `/storage/roms/ports/gamelist.xml` file:

```
	<game>
		<path>./DemoAppLauncher.sh</path>
		<name>Demo App</name>
		<desc>Demo App description</desc>
		<developer>You</developer>
		<publisher>non-commercial</publisher>
	</game>

```

## Run

Find "Demo App" in a list of ports on 351ELEC system. A screen with sprites of buttons must be displayed on the screen. When pressing buttons corresponding sprites will be activated as a feedback.

## Roadmap

- [X] Build just C runtime console "printf("Hello, world!)" program with the toolchain and run on 351 via ssh.
- [X] Let it be visible and runnable in the ports list.
- [X] Link with SDL2
- [X] Make it use SDL2 to write "Hello, world!" on the screen
- [ ] Play sound
- [ ] Rumble (rotate the vibration motor) by pressing A
- [X] Display picture with controls mapping and light up corresponding buttons
- [ ] Create a package that is able to be installed into 351ELEC system

## TODO

- [ ] Support joystic hotplug. It happens when device returns from sleep.

## Notes

- Keep it a simple demo app. No complicated mechanics must be involved. It must not be a game, it must be a hardware and maybe software features demonstration for port makers. It must be obvious where the app source code is, where build scripts are and where 351ELEC system related stuff is.
- Document and comment everything explicitly. Explain it like some noob begins his/her own journey to the world of programming and decided to start here. Or at least aspire to this level of detailing.
