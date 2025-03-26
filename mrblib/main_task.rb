require "machine"
require "watchdog"
Watchdog.disable
require "shell"
STDOUT = IO
STDIN = IO

# Setup flash disk
begin
  STDIN.echo = false
  puts "Initializing FLASH disk as the root volume... "
  Shell.setup_root_volume(:flash, label: "R2P2")
  Shell.setup_system_files

  begin
    require "cyw43"
    ENV['WIFI_MODULE'] = "cwy43"
  rescue LoadError
    # No WiFi module
  end

  Shell.bootstrap("/etc/init.d/r2p2")

  $shell = Shell.new(clean: true)
  puts "Starting shell...\n\n"

  $shell.show_logo
  $shell.start

rescue => e
  puts "#{e.message} (#{e.class})"
end

