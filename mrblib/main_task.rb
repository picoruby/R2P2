require "machine"
require "watchdog"
Watchdog.disable
require "shell"
STDOUT = IO.new
STDIN = IO.new

# TODO: Fix mruby-time
if RUBY_ENGINE == "mruby"
  class Time
    def self.now(in: nil)
      ts = Machine.get_hwclock
      tv_sec = ts[0]
      tv_nsec = ts[1].to_f / 1_000_000_000
      Time.at(tv_sec + tv_nsec)
    end
  end
end

begin
  sleep 1
  STDIN.echo = false
  puts "Initializing FLASH disk as the root volume... "
  Shell.setup_root_volume(:flash, label: "R2P2")
  Shell.setup_system_files

  Shell.bootstrap("/etc/init.d/r2p2")

  shell = Shell.new(clean: true)
  puts "Starting shell...\n\n"

  shell.show_logo
  shell.start
rescue => e
  puts "#{e.message} (#{e.class})"
end

