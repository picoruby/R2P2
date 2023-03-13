require "fileutils"

MRUBY_CONFIG = "r2p2-cortex-m0plus"
PICO_SDK_TAG = "1.5.0"

task :default => :all

desc "build production"
task :all => [:libmruby, :cmake_production, :build]

desc "clean then build debug"
task :debug => [:clean, :libmruby, :cmake_debug, :build]

file "lib/picoruby" do
  sh "git submodule update --init --recursive"
end

task :libmruby => "lib/picoruby" do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{MRUBY_CONFIG} rake"
  end
end

def select_msc
  ENV['MSC']&.downcase == 'sd' ? "PICORUBY_MSC_SD=yes" : "PICORUBY_MSC_FLASH=yes"
end

task :cmake_debug do
  sh "#{select_msc} cmake -DCMAKE_BUILD_TYPE=Debug -B build"
end

task :cmake_production do
  sh "#{select_msc} cmake -DCMAKE_BUILD_TYPE=Release -B build"
end

task :check_pico_sdk => :check_pico_sdk_path do
  FileUtils.cd ENV["PICO_SDK_PATH"] do
    unless `git status --branch`.split("\n")[0].end_with?(PICO_SDK_TAG)
      raise <<~MSG
        pico-sdk #{PICO_SDK_TAG} is not checked out!\n
        Tips for dealing with:\n
        cd $PICO_SDK_PATH && git pull && git checkout #{PICO_SDK_TAG} && git submodule update --recursive\n
      MSG
    end
  end
end

task :check_pico_sdk_path do
  unless ENV["PICO_SDK_PATH"]
    raise <<~MSG
      Environment variable `PICO_SDK_PATH` does not exist!
    MSG
  end
end

desc "build without cmake preparation"
task :build => :check_pico_sdk do
  sh "cmake --build build"
end

desc "deep clean built"
task :deep_clean do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{MRUBY_CONFIG} rake deep_clean"
  end
  FileUtils.cd "build" do
    FileUtils.rm_rf "*"
  end
end

desc "clean built"
task :clean do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{MRUBY_CONFIG} rake clean"
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
