require "machine"
require "watchdog"
Watchdog.disable
require "shell"

STDOUT = IO
STDIN = IO

# Setup flash disk
begin
  STDIN.echo = false
  $shell = Shell.new(clean: true)
  puts "Initializing FLASH disk as the root volume... "
  $shell.setup_root_volume(:flash, label: "R2P2")
  $shell.setup_system_files
  puts "Available"
rescue => e
  puts "Not available"
  puts "#{e.message} (#{e.class})"
end

# Putting this before the shell setup causes the shell to hang
begin
  require "cyw43"
  ENV['WIFI_MODULE'] = "cwy43"
rescue LoadError
  # No WiFi module
end

begin
  $shell.bootstrap("/etc/init.d/r2p2")

  puts "Starting shell...\n\n"

  $shell.show_logo
  $shell.start

rescue => e
  puts "#{e.message} (#{e.class})"
  Watchdog.reboot 1000
end

