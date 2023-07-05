require "machine"
require "task"
require "shell"
require "spi"

IO.wait_and_clear(timeout: 3)

# Setup flash disk
begin
  $shell = Shell.new
  print "Initializing FLASH disk as the root volume... "
  $shell.setup_root_volume(:flash, label: "FLASH")
  $shell.setup_system_files
  ENV['PATH'] = "/bin"
  ENV['HOME'] = "/home"
  Dir.chdir("/home")
  puts "Available"
rescue => e
  puts "Not available"
  puts "#{e.message} (#{e.class})"
end

$reboot_count = 0
begin
  # Execute /home/app.mrb or /home/app.rb
  if File.exist?("/home/app.mrb")
    Shell::Command.new.exec("/home/app.mrb")
  elsif File.exist?("/home/app.rb")
    Shell::Command.new.exec("/home/app.rb")
  end
  # Start shell if terminal is available
  IO.wait_and_clear
  $shell.show_logo
  puts "Starting shell...\n\n"
  $shell.start
rescue => e
  puts "#{e.message} (#{e.class})"
  puts "Rebooting"
  sleep 1
  if $reboot_count < 10
    $reboot_count += 1
    retry
  else
    puts "Rebooting count hit the limit. Aborting."
  end
end

