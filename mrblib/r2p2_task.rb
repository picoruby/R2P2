require "machine"
require "task"
require "shell"

IO.wait_and_clear
$shell = Shell.new
$shell.show_logo

# Real-time clock
def setup_rtc(i2c_unit, sda, scl)
  require "i2c"
  require "adafruit_pcf8523"
  begin
    print "Initializing RTC... "
    ENV['TZ'] = "JST-9"
    i2c = I2C.new(unit: i2c_unit, sda_pin: sda, scl_pin: scl)
    $rtc = PCF8523.new(i2c: i2c)
    Time.hwclock = $rtc.current_time
    FAT.unixtime_offset = Time.unixtime_offset
    puts "Available (#{Time.now})"
  rescue => e
    puts "Not available"
    puts "#{e.message} (#{e.class})"
  end
end

# Flash disk
def setup_flash_disk
  require "spi"
  begin
    print "Initializing FLASH disk as the root volume... "
    $shell.setup_root_volume(:flash, label: "FLASH")
    $shell.setup_system_files
    ENV['PATH'] = "/bin"
    ENV['HOME'] = "/home"
    puts "Available"
  rescue => e
    puts "Not available"
    puts "#{e.message} (#{e.class})"
  end
end

# SD card
def setup_sd_card(spi_unit, sck, cipo, copi, cs)
  begin
    print "Initializing SD card... "
    spi = SPI.new(frequency: 5_000_000, unit: spi_unit,
                  sck_pin:  sck,  cipo_pin: cipo,
                  copi_pin: copi, cs_pin:   cs)
    sd = FAT.new(:sd, label: "SD", driver: spi)
    sd_mountpoint = "/sd"
    VFS.mount(sd, sd_mountpoint)
    puts "Available at #{sd_mountpoint}"
  rescue => e
    puts "Not available"
    puts "#{e.message} (#{e.class})"
  end
end

def m
  PicoRubyVM.memory_statistics
end

# Shell
begin
  puts "Starting shell...\n\n"
  i2c_unit, sda, scl = :RP2040_I2C0, 4, 5
  $shell.simple_question("Do you have RTC? (sda:#{sda}, scl:#{scl}) [y/N] ") do |answer|
    case answer
    when "y", "Y"
      setup_rtc(i2c_unit, sda, scl)
      true
    when "n", "N", ""
      puts "No RTC"
      true
    end
  end
  setup_flash_disk
  spi_unit, sck, cipo, copi, cs = :RP2040_SPI0, 22, 20, 23, 21
  $shell.simple_question("Do you have SD card? (sck:#{sck}, cipo:#{cipo}, copi:#{copi}, cs:#{cs}) [y/N] ") do |answer|
    case answer
    when "y", "Y"
      setup_sd_card(spi_unit, sck, cipo, copi, cs)
      true
    when "n", "N", ""
      puts "No SD card"
      true
    end
  end
  $shell.start
rescue => e
  puts "#{e.message} (#{e.class})"
  puts "Rebooting"
  sleep 1
  retry
end

