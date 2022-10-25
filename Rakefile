require "fileutils"

MRUBY_CONFIG = "prsh-lib_arm-none-eabi"

task :default => :all

task :all => [:libmruby, :main]

file "lib/picoruby" do
  FileUtils.cd "lib" do
    sh "git clone --recursive https://github.com/picoruby/picoruby.git"
  end
end

task :libmruby => "lib/picoruby" do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{MRUBY_CONFIG} rake"
  end
end

task :main do
  sh "cmake -B build"
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
  sh "cmake --build build --target clean"
end
