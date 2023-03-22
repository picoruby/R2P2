require "spi"
require "shell"
require "adafruit_pcf8523"

def exit
  raise # to restart
end

ENV['TZ'] = "JST-9"
ENV['PATH'] = "/bin"
ENV['HOME'] = "/home"

begin
  Shell.setup(:flash, label: "FLASH")
  IO.wait_and_clear
  begin
    $rtc = PCF8523.new(unit: :RP2040_I2C0, sda_pin: 4, scl_pin: 5)
    Time.hwclock = $rtc.current_time
  rescue => e
    puts "#{e.message} (#{e.class})"
    sleep 1
    retry
  end
  unless VFS::VOLUMES.any?{|v| v[:driver].prefix == "sd:" }
    spi = SPI.new(
            unit: :RP2040_SPI0,
            frequency: 5_000_000,
            sck_pin:  22,
            cipo_pin: 20,
            copi_pin: 23,
            cs_pin:   21
          )
    sd = FAT.new(:sd, label: "SD", driver: spi)
    VFS.mount(sd, "/sd")
  end
  Shell.new.start(:shell)
rescue => e
  puts "#{e.message} (#{e.class})"
  puts "Rebooting"
  sleep 1
  retry
end

