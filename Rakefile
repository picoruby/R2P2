require "fileutils"

PICO_SDK_TAG = "1.5.1"
PICO_EXTRAS_TAG = "sdk-#{PICO_SDK_TAG}"

def mruby_config
  if ENV['BOARD']&.downcase == 'pico_w'
    use_wifi = ENV['WIFI']&.downcase == 'yes'
    use_ble  = ENV['BLE']&.downcase == 'yes'
    if use_wifi && use_ble
      'r2p2_w_ble_wifi-cortex-m0plus'
    elsif use_wifi
      'r2p2_w_wifi-cortex-m0plus'
    elsif use_ble
      'r2p2_w_ble-cortex-m0plus'
    else
      raise 'Either BLE or WiFi needs to be enabled for BOARD=pico_w'
    end
  else
    'r2p2-cortex-m0plus'
  end
end

def select_flags
  flags = []
  flags << (ENV['MSC']&.downcase == 'sd' ? "PICORUBY_MSC_SD=yes" : "PICORUBY_MSC_FLASH=yes")
  if ENV['BOARD']&.downcase == 'pico_w'
    flags << "PICO_W=yes"
    if ENV['WIFI']&.downcase == 'yes'
      flags << "PICO_W_WIFI=yes"
    end
    if ENV['BLE']&.downcase == 'yes'
      flags << "PICO_W_BLE=yes"
    end
  end
  flags.join(" ")
end

def def_board
  ENV['BOARD']&.downcase == 'pico_w' ? '-DPICO_BOARD=pico_w' : ''
end

task :default => :all

task :setup do
  sh "git submodule update --init"
  FileUtils.cd "lib/picoruby" do
    sh "bundle install"
  end
end

desc "build production"
task :all => [:libmruby, :cmake_production, :build]

desc "clean then build debug"
task :debug => [:clean, :libmruby, :cmake_debug, :build]

file "lib/picoruby" do
  sh "git submodule update --init --recursive"
end

task :libmruby => "lib/picoruby" do
  FileUtils.cd "lib/picoruby" do
    sh "rake test"
    sh "MRUBY_CONFIG=#{mruby_config} rake"
  end
end

def cmake_cmd(env)
  "#{select_flags} cmake #{def_board} -DCMAKE_BUILD_TYPE=#{env} -B build"
end

task :cmake_debug do
  sh cmake_cmd("Debug")
end

task :cmake_production do
  sh cmake_cmd("Release")
end

task :check_pico_sdk => :check_pico_sdk_path do
  FileUtils.cd ENV['PICO_SDK_PATH'] do
    if `git describe --tags --exact-match`.chomp != PICO_SDK_TAG
      raise <<~MSG
        pico-sdk #{PICO_SDK_TAG} is not checked out!\n
        Tips for dealing with:\n
        cd $PICO_SDK_PATH && git pull && git checkout #{PICO_SDK_TAG} && git submodule update --recursive\n
      MSG
    end
  end
  FileUtils.cd ENV['PICO_EXTRAS_PATH'] do
    if `git describe --tags --exact-match`.chomp != PICO_EXTRAS_TAG
      raise <<~MSG
        pico-extras #{PICO_EXTRAS_TAG} is not checked out!\n
        Tips for dealing with:\n
        cd $PICO_EXTRAS_PATH && git pull && git checkout #{PICO_EXTRAS_TAG} && git submodule update --recursive\n
      MSG
    end
  end
end

task :check_pico_sdk_path do
  %w(PICO_SDK_PATH PICO_EXTRAS_PATH).each do |env|
    unless ENV[env]
      raise <<~MSG
        Environment variable `#{env}` does not exist!
      MSG
    end
  end
end

desc "build without cmake preparation"
task :build => :check_pico_sdk do
  sh "cmake --build build"
end

desc "deep clean built"
task :deep_clean do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{mruby_config} rake deep_clean"
  end
  FileUtils.cd "build" do
    FileUtils.rm_rf "*"
  end
end

desc "clean built"
task :clean do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{mruby_config} rake clean"
  end
  FileUtils.cd "build" do
    FileUtils.rm_rf Dir.glob("R2P2*.*")
  end
  begin
    sh "cmake --build build --target clean"
  rescue => e
    puts "Ignoring an error: #{e.message}"
  end
end
