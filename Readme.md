# Example native 351ELEC app

## Build

You need a toolchain or a virtual machine with such a toolchain first. The toolchain's [target triplet](https://wiki.osdev.org/Target_Triplet) should be `aarch64-libreelec-linux-gnueabi`. If you already built 351ELEC system itself then you already have one. For example in my case I have 351ELEC local repo at `/home/oxore/build-pkgs/351ELEC` and toolchain for my RG351P is `/home/oxore/build-pkgs/351ELEC/build.351ELEC-RG351P.aarch64/toolchain` correspondingly.

Then you run the build command specifying your toolchain explicitly

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

Now you have the `main` file which can be copied to the device over SSH or by putting on SD card in a dedicated directory in `ports`, called, for example, `demo`. It must be on the `GAMES` microSD card partition and be mounted on the device under `/storage/roms` and have a symlink `/roms` pointing to `/storage/roms`. It is console application, so you must run it from the system shell. You can accessing the device shell via SSH.

Create directory `/storage/roms/ports/demo`

Copy file `DemoAppLauncher.sh` into `/storage/roms/ports` directory.

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

Find "Demo App" in a list of ports on 351elec system. When it run a turquoise screen is being shown for 5 seconds and then it terminates.

## Roadmap

- [X] Build just C runtime console "printf("Hello, world!)" program with the toolchain and run on 351 via ssh.
- [X] Let it be visible and runnable in the ports list.
- [X] Link with SDL2
- [ ] Make it use SDL2 to write "Hello, world!" on the screen
- [ ] Play sound
- [ ] Move a sprite around by pressing cross buttons and touching L2/L3 sticks
- [ ] Rotate vibration motor by pressing A
- [ ] Display picture with controls mapping and light up correspongin buttons

## Notes

- Keep it a simple demo app. No complicated mechanics must be involved. It must not be a game, it must be a features demo for port makers. It must be obvious where the app source code is, where build scripts are and 351elec system related stuff is.
- Document and comment everything explicitly. Explain it like some noob begins his/her own journey to the world of programming and decided to start here. Or at least aspire to this level of detailing.
