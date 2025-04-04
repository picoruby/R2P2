require "fileutils"

PICO_SDK_TAG = "2.1.1"
if PICO_SDK_TAG == "2.1.1"
  PICO_EXTRAS_TAG = "sdk-2.1.0" # workaround. 2.1.1 and 2.1.0 are the same
else
  PICO_EXTRAS_TAG = "sdk-#{PICO_SDK_TAG}"
end

def mruby_config
  case ENV['BOARD']&.downcase
  when 'pico2'
    'r2p2-microruby-cortex-m33'
  when 'pico_w'
    'r2p2_w-microruby-cortex-m0plus'
  when 'pico2_w'
    'r2p2_w-microruby-cortex-m33'
  else
    #'r2p2-cortex-m0plus'
    'r2p2-microruby-cortex-m0plus'
  end
end

def cmake_flags
  flags = []
  flags << (ENV['MSC']&.downcase == 'sd' ? "PICORUBY_MSC_SD=yes" : "PICORUBY_MSC_FLASH=yes")
  case ENV['BOARD']&.downcase
  when 'pico_w'
    flags << "PICO_W=yes"
  when 'pico2'
    flags << "PICO2=yes"
  when 'pico2_w'
    flags << "PICO2_W=yes"
  end
  flags.join(" ")
end

def def_board
  case ENV['BOARD']&.downcase
  when 'pico2_w'
    '-DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2_w'
  when 'pico2'
    '-DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2'
  when 'pico_w'
    '-DPICO_PLATFORM=rp2040 -DPICO_BOARD=pico_w'
  else
    '-DPICO_PLATFORM=rp2040 -DPICO_BOARD=pico'
  end
end

def build_dir
  case ENV['BOARD']&.downcase
  when 'pico2_w'
    'build_pico2_w'
  when 'pico2'
    'build_pico2'
  when 'pico_w'
    'build_pico_w'
  else
    'build_pico'
  end
end

task :default do
  puts "Specify a task:"
  puts "  rake pico       # build for RP2040"
  puts "  rake pico_w     # build for RP2040 with CYW43"
  puts "  rake pico2      # build for RP2350"
  puts "  rake pico2_w    # build for RP2350 with CYW43"
end

desc "build for RP2040"
task :pico do
  ENV['BOARD'] = 'pico'
  sh "rake all"
end

desc "build for RP2040 with debug flags"
task :pico_debug do
  ENV['BOARD'] = 'pico'
  ENV['PICORUBY_DEBUG'] = '1'
  sh "rake debug"
end

desc "build for RP2040 with CYW43"
task :pico_w do
  ENV['BOARD'] = 'pico_w'
  sh "rake all"
end

desc "build for RP2350 with CYW43"
task :pico2_w do
  ENV['BOARD'] = 'pico2_w'
  sh "rake all"
end

desc "build for RP2040 with CYW43 with debug flags"
task :pico_w_debug do
  ENV['BOARD'] = 'pico_w'
  ENV['PICORUBY_DEBUG'] = '1'
  sh "rake debug"
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

desc "build for RP2350 with CYW43 with debug flags"
task :pico2_w_debug do
  ENV['BOARD'] = 'pico2_w'
  ENV['PICORUBY_DEBUG'] = '1'
  sh "rake debug"
end

desc "clean built for RP2xxx"
task clean_all: [:clean_pico, :clean_pico_w, :clean_pico2, :clean_pico2_w]

desc "clean built for RP2040"
task :clean_pico do
  ENV['BOARD'] = 'pico'
  sh "rake clean"
  sh "cmake --build build_pico --target clean"
end

desc "clean built for RP2040 with CYW43"
task :clean_pico_w do
  ENV['BOARD'] = 'pico_w'
  sh "rake clean"
  sh "cmake --build build_pico_w --target clean"
end

desc "clean built for RP2350"
task :clean_pico2 do
  ENV['BOARD'] = 'pico2'
  sh "rake clean"
  sh "cmake --build build_pico2 --target clean"
end

desc "clean built for RP2350 with CYW43"
task :clean_pico2_w do
  ENV['BOARD'] = 'pico2_w'
  sh "rake clean"
  sh "cmake --build build_pico2_w --target clean"
end
desc "clean built for RP2350 with CYW43"
task :clean_pico2_w do
  ENV['BOARD'] = 'pico2_w'
  sh "rake clean"
  sh "cmake --build build_pico2_w --target clean"
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
task :debug => [:libmruby, :cmake_debug, :build]

file "lib/picoruby" do
##  sh "git submodule update --init --recursive"
end

task :libmruby => "lib/picoruby" do
  FileUtils.cd "lib/picoruby" do
   # sh "rake test"
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

BUILD_DIRS = %w(build_pico build_pico_w build_pico2)

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
end
