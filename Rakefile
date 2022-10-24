require "fileutils"

task :default => :all

task :all => [:libmruby, :main]

file "lib/picoruby" do
  FileUtils.cd "lib" do
    sh "git clone recursive https://github.com/picoruby/picoruby.git"
  end
end

task :libmruby => "lib/picoruby" do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=prsh-lib_arm-none-eabi rake"
  end
end

task :main do
  sh "cmake -B build"
  sh "cmake --build build"
end

task :clean do
  sh "cmake --build build --target clean"
end
