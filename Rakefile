require "fileutils"

PICO_SDK_TAG = "2.0.0"
PICO_EXTRAS_TAG = "sdk-#{PICO_SDK_TAG}"

def mruby_config
  case ENV['BOARD']&.downcase
  when 'pico2'
    'r2p2-cortex-m33'
  when 'pico_wifi'
    'r2p2_wifi-cortex-m0plus'
  when 'pico_ble'
    'r2p2_ble-cortex-m0plus'
  else
    'r2p2-cortex-m0plus'
  end
end

def cmake_flags
  flags = []
  flags << (ENV['MSC']&.downcase == 'sd' ? "PICORUBY_MSC_SD=yes" : "PICORUBY_MSC_FLASH=yes")
  case ENV['BOARD']&.downcase
  when 'pico_wifi'
    flags << "PICO_WIFI=yes"
  when 'pico_ble'
    flags << "PICO_BLE=yes"
  when 'pico2'
    flags << "PICO2=yes"
  end
  flags.join(" ")
end

def def_board
  case ENV['BOARD']&.downcase
  when 'pico2'
    '-DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2'
  when 'pico_wifi', 'pico_ble'
    '-DPICO_PLATFORM=rp2040 -DPICO_BOARD=pico_w'
  else
    '-DPICO_PLATFORM=rp2040 -DPICO_BOARD=pico'
  end
end

def build_dir
  case ENV['BOARD']&.downcase
  when 'pico2'
    'build_pico2'
  when 'pico_wifi'
    'build_pico_wifi'
  when 'pico_ble'
    'build_pico_ble'
  else
    'build_pico'
  end
end

task :default do
  puts "Specify a task:"
  puts "  rake pico       # build for RP2040"
  puts "  rake pico_wifi  # build for RP2040 with CYW43 WiFi"
  puts "  rake pico_ble   # build for RP2040 with CYW43 BLE"
  puts "  rake pico2      # build for RP2350"
end

desc "build for RP2040"
task :pico do
  ENV['BOARD'] = 'pico'
  sh "rake all"
end

desc "build for RP2040 with CYW43 WiFi"
task :pico_wifi do
  ENV['BOARD'] = 'pico_wifi'
  sh "rake all"
end

desc "build for RP2040 with CYW43 BLE"
task :pico_ble do
  ENV['BOARD'] = 'pico_ble'
  sh "rake all"
end

desc "build for RP2350"
task :pico2 do
  ENV['BOARD'] = 'pico2'
  sh "rake all"
end

desc "build for RP2350 with debug flags"
task :pico2_debug do
  ENV['BOARD'] = 'pico2'
  ENV['PICORUBY_DEBUG'] = '1'
  sh "rake debug"
end

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
  "#{cmake_flags} cmake #{def_board} -DCMAKE_BUILD_TYPE=#{env} -B #{build_dir}"
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
  sh "cmake --build #{build_dir}"
end

BUILD_DIRS = %w(build_pico build_pico_wifi build_pico_ble build_pico2)

desc "deep clean built"
task :deep_clean do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{mruby_config} rake deep_clean"
  end
  BUILD_DIRS.each do |dir|
    FileUtils.rm_rf "#{dir}/*"
  end
end

desc "clean built"
task :clean do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{mruby_config} rake clean"
  end
  BUILD_DIRS.each do |dir|
    FileUtils.rm_f Dir.glob("#{dir}/R2P2*.*")
    begin
      sh "cmake --build #{dir} --target clean"
    rescue => e
      puts "Ignoring an error: #{e.message}"
    end
  end
end
