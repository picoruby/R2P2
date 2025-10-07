# R2P2

R2P2 (Ruby Rapid Portable Platform) is a shell system runs on Raspberry Pi Pico written in [picoruby/picoruby](https://github.com/picoruby/picoruby).

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

### Dual CDC USB ports for debug output

R2P2 uses dual CDC (Communications Device Class) USB ports to separate standard output and debug output:

- **CDC 0** (in Linux, typically `/dev/ttyACM0`): Main terminal for shell interaction and application stdout
- **CDC 1** (as in, `/dev/ttyACM1`): Debug output (stderr) for system messages and debug prints

Debug output is only enabled in debug builds (`rake debug` or `PICORUBY_DEBUG=1 rake`).

To view debug output:
```sh
# Terminal 1: Main shell
gtkterm --port /dev/ttyACM0

# Terminal 2: Debug messages
gtkterm --port /dev/ttyACM1
```

In Ruby code, use `Machine.debug_puts` to output to the debug port:
```ruby
Machine.debug_puts "Debug message"
```

## Demonstration

### Opening crawl

<a href="https://youtu.be/JfN5BpTCYOw"><img src="https://raw.githubusercontent.com/picoruby/R2P2/master/doc/images/openingcralw.png" width="360" /></a>

### Tutorial short clips

#### Part 1

1. <a href="https://youtu.be/s4M4rBnPSus">Preparation | https://youtu.be/s4M4rBnPSus</a>
  - Install R2P2 into Raspberry Pi Pico.
  - Open R2P2 in a terminal emulator.

2. <a href="https://youtu.be/ISU6TbIoIlQ">R2P2 shell | https://youtu.be/ISU6TbIoIlQ</a>
  - Use some shell commands to see a filesystem working on R2P2.

3. <a href="https://youtu.be/2ZKpOOjzKJc">Hello World! | https://youtu.be/2ZKpOOjzKJc</a>
  - Run some small Ruby scripts in IRB.

#### Part 2

1. <a href="https://youtu.be/qbs25xDu7t8">GPIO | https://youtu.be/qbs25xDu7t8</a>
  - Blink the on-board LED of Raspberry Pi Pico using the GPIO class of PicoRuby.

2. <a href="https://youtu.be/dPGCyQrX6Zg">ADC | https://youtu.be/dPGCyQrX6Zg</a>
  - Measure the temperature using Pi Pico's on-chip ADC with the ADC class of PicoRuby.

#### Part 3

1. <a href="https://youtu.be/PVkP_uNBOo0">IRB deeper | https://youtu.be/PVkP_uNBOo0</a>
  - Use the multi-line editor feature of IRB.

2. <a href="https://youtu.be/0uj4m4RI7lE">Time class | https://youtu.be/0uj4m4RI7lE</a>
  - Set the current time in the RTC of Pi Pico.
  - Confirm the `date` command in the shell is implemented with the Time class of PicoRuby.

3. <a href="https://youtu.be/X1RdA6IE780">Text editor | https://youtu.be/X1RdA6IE780</a>
  - Create a Ruby script with the Vim-like text editor and execute it.

4. <a href="https://youtu.be/7nHNEUZnuKQ">Drag and Drop | https://youtu.be/7nHNEUZnuKQ</a>
  - Drag and drop a Ruby script that is written outside R2P2 and execute it.

5. <a href="https://youtu.be/6_RomLChvYE">/home/app.rb | https://youtu.be/6_RomLChvYE</a>
  - Make Pi Pico an autostart device by writing /home/app.rb file.

### Presentation video

The presentation video (JA) from RubyWorld Conference 2022: [Link](https://youtu.be/rSBnpxzB4d8?t=11226)

### Presentation slide

<a href="https://slide.rabbit-shocker.org/authors/hasumikin/RubyWorldConference2022/"><img src="https://raw.githubusercontent.com/picoruby/R2P2/master/doc/images/slidecover.png" width="360" /></a>

## Build and development

### Prerequisites

- git
- pico-sdk including tinyusb
- pico-sdk including btstack (for pico_w)
- env PICO_SDK_PATH
- env PICO_EXTRAS_PATH
- ruby (CRuby)
- cmake
- arm-none-eabi-gcc
- arm-linux-gnueabihf-gcc
- qemu-arm-static

The author is working on WSL2-Ubuntu (x86-64 Windows host) and Ubuntu (x86-64 native)

If you are building on macOS, please use `gcc-arm-embedded`.

```
brew install --cask gcc-arm-embedded
```

### Build

```
git clone https://github.com/picoruby/R2P2.git
cd R2P2
rake setup
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
