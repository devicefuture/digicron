# DigiCron
Software and hardware for the DigiCron smartwatch.

Want to try out the DigiCron's firmware in your browser? Just visit the [DigiCron Simulator](https://digicron.devicefuture.org/simulator)!

## Getting the code
You must clone this repo like this:

```bash
git clone --recurse-submodules https://github.com/devicefuture/digicron
```

`--recurse-submodules` must be used because this repo contains Git submodules which are used as libraries for the firmware and simulator.

## Building
Before building the firmware for the first time, you will need to install the development dependencies:

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

Then to run the simulator locally, you need to start a web server in this directory. Run the following to start the web server at [localhost:8000](http://localhost:8000):

```bash
python3 -m http.server
```