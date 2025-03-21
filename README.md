# DigiCron
Software and hardware for the DigiCron retro smartwatch.

Want to try out the software on your device? Just visit the [DigiCron Simulator](simulator)!

## Getting the code
You must clone this repo like this:

```bash
git clone --recurse-submodules https://github.com/devicefuture/digicron
```

This is because this repo contains submodules which are used as libraries for the firmware and simulator.

## Building
Before building for the first time, install dev dependencies:

```bash
./build.sh --install-dev
```

To build the firmware and upload it to the target device over USB, run:

```bash
./build.sh --upload
```

You can also check for compilation errors without uploading by removing the `--upload` flag.

To build the firmware and the simulator, run:

```bash
./build.sh --sim
```

Then to run the simulator locally, start a web server in this directory. Run the following to start the web server at [localhost:8000](http://localhost:8000):

```bash
python3 -m http.server
```

### Building a system app
All system apps are already built when running the main build script, but it is possible to build apps separately anyway for debugging purposes.

First install the required dependencies (these are installed for you when you run `./build.sh --install-dev`):

```bash
sudo apt install clang lld xxd
```

Then run (where `$name` is the name of the app's source directory, such as `test`):

```bash
apps/build.sh $name
```

> [!NOTE]
> If you are experiencing segmentation faults with `clang` and `lld` (on version 19), then you may want to try installing `clang-14` and `lld-14`, which are known working versions. The build script will automatically choose `clang-14` (if available) over other installed versions.