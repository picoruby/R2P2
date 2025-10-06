#!/usr/bin/env ruby

require 'fileutils'

DEFAULT_DEVICE = '/dev/ttyUSB0'
DEFAULT_BAUDRATE = '115200'

if ARGV.include?('--help') || ARGV.include?('-h')
  puts <<~HELP
    This script connects to a serial device using 'screen' and automatically
    reconnects if the connection is lost. It waits for the device to become
    available if it's not currently connected.
    Usage: ruby screen.rb [device] [baud_rate]
    Arguments:
      device     - The serial device to connect to (default: #{DEFAULT_DEVICE})
      baud_rate  - The baud rate for the connection (default: #{DEFAULT_BAUDRATE})
    Example:
      ruby screen.rb #{DEFAULT_DEVICE} #{DEFAULT_BAUDRATE}
  HELP
  exit
end

DEVICE = ARGV[0] || DEFAULT_DEVICE
BAUD_RATE = ARGV[1] || DEFAULT_BAUDRATE

def device_exists?(device)
  File.exist?(device) && File.readable?(device)
end

def start_screen(device, baud_rate)
  puts "#{Time.now}: Connecting to #{device} at #{baud_rate} baud..."
  system("screen #{device} #{baud_rate}")
  exit_status = $?.exitstatus
  puts "#{Time.now}: Screen session exited with status #{exit_status}."
end

def wait_for_device(device)
  puts "#{Time.now}: Waiting for #{device}..."
  until device_exists?(device)
    sleep 1
    print "."
  end
  puts "\n#{Time.now}: Device #{device} is available!"
  sleep 0.5
end

puts "Auto-reconnecting screen wrapper"
puts "Device: #{DEVICE}, Baud rate: #{BAUD_RATE}"
puts "Press Ctrl+C to exit"
puts "-" * 40

trap("INT") { puts "\nExiting..."; exit 0 }

loop do
  if device_exists?(DEVICE)
    start_screen(DEVICE, BAUD_RATE)
    puts "\n#{Time.now}: Screen session ended. Reconnecting..."
    sleep 1
  else
    wait_for_device(DEVICE)
  end
end
