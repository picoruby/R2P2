require "fileutils"

unless ENV.include? "PICO_SDK_PATH"
  ENV["PICO_SDK_PATH"] = "#{File.dirname(__FILE__)}/lib/pico-sdk"
end
unless ENV.include? "PICO_EXTRAS_PATH"
  ENV["PICO_EXTRAS_PATH"] = "#{File.dirname(__FILE__)}/lib/pico-extras"
end

PICO_SDK_TAG = "2.2.0"
PICO_EXTRAS_TAG = "sdk-#{PICO_SDK_TAG}"

def mruby_config(vm, board)
  name = (vm == 'mruby' ? 'microruby-' : '')
  case board
  when 'pico2'
    "r2p2-#{name}cortex-m33"
  when 'pico_w'
    "r2p2_w-#{name}cortex-m0plus"
  when 'pico2_w'
    "r2p2_w-#{name}cortex-m33"
  else
    "r2p2-#{name}cortex-m0plus"
  end
end

def def_board(board)
  case board
  when 'pico2_w'
    '-D PICO_PLATFORM=rp2350 -D PICO_BOARD=pico2_w -D USE_WIFI=1'
  when 'pico2'
    '-D PICO_PLATFORM=rp2350 -D PICO_BOARD=pico2'
  when 'pico_w'
    '-D PICO_PLATFORM=rp2040 -D PICO_BOARD=pico_w -D USE_WIFI=1'
  else
    '-D PICO_PLATFORM=rp2040 -D PICO_BOARD=pico'
  end
end

def def_build_type(mode)
  case mode
  when 'debug'
    "-D CMAKE_BUILD_TYPE=Debug -D PICORUBY_DEBUG=1"
  else
    "-D CMAKE_BUILD_TYPE=Release -D NDEBUG=1"
  end
end

def def_r2p2_name(board)
  case board
  when 'pico'
    '-D R2P2_NAME=R2P2_PICO'
  when 'pico2'
    '-D R2P2_NAME=R2P2_PICO2'
  when 'pico_w'
    '-D R2P2_NAME=R2P2_PICO_W'
  when 'pico2_w'
    '-D R2P2_NAME=R2P2_PICO2_W'
  else
    raise "Unknown board: #{board}"
  end
end

def def_msc(mode)
  '-D PICORUBY_MSC_FLASH=1 -D MSC_NAME=FLASH_MSC'
end

def def_picorb_vm(vm)
  vm == 'mrubyc' ? '-D PICORB_VM_MRUBYC=1' : '-D PICORB_VM_MRUBY=1'
end

task :setup do
  sh "git submodule update --init"
  FileUtils.cd "lib/picoruby" do
    sh "bundle install"
  end
end

%w[mrubyc mruby].each do |vm|
  namespace vm do
    %w[pico pico_w pico2 pico2_w].each do |board|
      namespace board do
        %w[debug production].each do |mode|
          desc "Build for #{board} with #{vm} VM (#{mode})"
          task mode => :check_pico_sdk do
            FileUtils.cd "lib/picoruby" do
              sh "rake test" if ENV['DO_TEST']
              sh "MRUBY_CONFIG=#{mruby_config(vm, board)} #{mode=='debug' ? 'PICORUBY_DEBUG=1' : ''} rake"
            end
            build_dir = "build_#{board}"
            defs = <<~DEFS
              -D PICO_CYW43_SUPPORTED=1 \
              -D MRUBY_CONFIG=#{mruby_config(vm, board)} \
              -D BUILD_DIR=#{build_dir} \
              #{def_picorb_vm(vm)} \
              #{def_r2p2_name(board)} \
              #{def_board(board)} \
              #{def_build_type(mode)} \
              #{def_msc(mode)}
            DEFS
            sh "cmake -B #{build_dir} #{defs}"
            sh "cmake --build #{build_dir}"
          end
        end
      end
    end
  end
end

namespace :clean do
  %w[mrubyc mruby].each do |vm|
    namespace vm do
      %w[pico pico_w pico2 pico2_w].each do |board|
        desc "Clean build for #{board} with #{vm} VM"
        task board do
          FileUtils.cd "lib/picoruby" do
            if File.exist?("build_config/#{mruby_config(vm, board)}.rb")
              sh "MRUBY_CONFIG=#{mruby_config(vm, board)} rake clean"
            end
          end
          FileUtils.rm_f(Dir["build_#{board}/R2P2*.*"])
        end
      end
    end
  end
end

task :check_pico_sdk => :check_pico_sdk_path do
  FileUtils.cd ENV['PICO_SDK_PATH'] do
    if `git describe --tags --exact-match`.chomp != PICO_SDK_TAG
      raise <<~MSG
        pico-sdk #{PICO_SDK_TAG} is not checked out!\n
        Tips for dealing with:\n
        cd #{ENV['PICO_SDK_PATH']} && \\
          git fetch origin --tags && \\
          git checkout #{PICO_SDK_TAG} && \\
          git submodule update --recursive\n
      MSG
    end
  end
  FileUtils.cd ENV['PICO_EXTRAS_PATH'] do
    if `git describe --tags --exact-match`.chomp != PICO_EXTRAS_TAG
      raise <<~MSG
        pico-extras #{PICO_EXTRAS_TAG} is not checked out!\n
        Tips for dealing with:\n
        cd #{ENV['PICO_EXTRAS_PATH']} && \\
          git fetch origin --tags && \\
          git checkout #{PICO_EXTRAS_TAG} && \\
          git submodule update --recursive\n
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
