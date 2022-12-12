# R2P2

R2P2 (Ruby on Raspberry Pi Pico) is a shell system written in [picoruby/picoruby](https://github.com/picoruby/picoruby).

## Usage

(TODO)

- Download a release binary
- Drag and drop it into the RPI-RP2 drive of Pi Pico
- Connect the shell through a serial port with a terminal emulator
  ```sh
  gtkterm --port /dev/ttyACM0
  ```

### About serial port terminal emulators

GTKTerm is strongly recommended.
Traditional CUI/TUI emulator such as cu, screen, and minicom don't work well.

If you use Windows or macOS, try TeraTerm or PuTTY though the author didn't confirm them.
You may need to look for a fine configuration.

Anyway, it seems almost problems on the terminal emulator come from CR/LF handling.

## Demonstration

### Opening crawl

<a href="https://youtu.be/JfN5BpTCYOw" target="_blank"><img src="https://raw.githubusercontent.com/picoruby/R2P2/master/doc/images/openingcralw.png" width="360" /></a>

### Presentation video

The presentation video (JA) from RubyWorld Conference 2022: [Link](https://youtu.be/rSBnpxzB4d8?t=11226)

### Presentation slide

<a href="https://slide.rabbit-shocker.org/authors/hasumikin/RubyWorldConference2022/" target="_blank"><img src="https://raw.githubusercontent.com/picoruby/R2P2/master/doc/images/slidecover.png" width="360" /></a>

## Build and development

### Prerequisites

- git
- pico-sdk including tinyusb
- env PICO_SDK_PATH
- ruby (CRuby)
- cmake
- arm-none-eabi-gcc
- arm-linux-gnueabihf-gcc
- qemu-arm-static

The author is working on WSL2-Ubuntu (x86-64 Windows host) and Ubuntu (x86-64 native)

### Build

Build process below doesn't work on macOS.
Docker will solve it (TODO).

```
git clone https://github.com/picoruby/R2P2.git
cd R2P2
rake
```

You'll find an executable in `build` directory.

### Debug build

```
rake debug
```

The elf file can be debugged with Picoprobe and gdb-multiarch.

#### Usage of Picoprobe

See the gist: https://gist.github.com/hasumikin/f508c092ced0b5d51779d472fbaf81e8

## Documentation

(TODO)

## Roadmap

* Fix many bugs, and
* PicoRuby and Shell
  * More functionality of vim
  * UTF-8 support
  * Multi-task controll on the fly
  * Redirect and pipeline
* Peripherals
  * FAT filesystem with SD card (SPI)
  * Real-time clock (RTC)
  * WiFi and TCP/IP on Raspberry Pi Pico W
