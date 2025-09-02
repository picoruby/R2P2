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
  "r2p2-#{vm}-#{board}"
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

def def_r2p2_name(vm, board)
  "-D R2P2_NAME=R2P2-#{vm.upcase}-#{board.upcase}"
end

def def_msc(mode)
  '-D PICORUBY_MSC_FLASH=1'
end

def def_picorb_vm(vm)
  vm == 'picoruby' ? '-D PICORB_VM_MRUBYC=1' : '-D PICORB_VM_MRUBY=1'
end

def build_dir(vm, board, mode)
  "build/#{vm}/#{board}/#{mode}"
end

task :setup do
  sh "git submodule update --init"
  FileUtils.cd "lib/picoruby" do
    sh "bundle install"
  end
end

%w[picoruby microruby].each do |vm|
  namespace vm do
    %w[pico pico_w pico2 pico2_w].each do |board|
      namespace board do
        %w[debug prod].each do |mode|
          desc "Build #{vm} for #{board} (#{mode})"
          task mode => :check_pico_sdk do
            dir = build_dir(vm, board, mode)
            FileUtils.mkdir_p dir
            FileUtils.cd "lib/picoruby" do
              sh "rake test" if ENV['DO_TEST']
              sh "MRUBY_CONFIG=#{mruby_config(vm, board)} #{mode=='debug' ? 'PICORUBY_DEBUG=1' : ''} rake"
            end
            defs = <<~DEFS
              -D PICO_CYW43_SUPPORTED=1 \
              -D MRUBY_CONFIG=#{mruby_config(vm, board)} \
              -D BUILD_DIR=#{dir} \
              #{def_picorb_vm(vm)} \
              #{def_r2p2_name(vm, board)} \
              #{def_board(board)} \
              #{def_build_type(mode)} \
              #{def_msc(mode)}
            DEFS
            sh "cmake -B #{dir} #{defs}"
            sh "cmake --build #{dir}"
          end
        end
      end
    end
  end
end

namespace :clean do
  %w[picoruby microruby].each do |vm|
    namespace vm do
      %w[pico pico_w pico2 pico2_w].each do |board|
        desc "Clean #{vm} for #{board} (both debug and prod)"
        task board do
          FileUtils.cd "lib/picoruby" do
            if File.exist?("build_config/#{mruby_config(vm, board)}.rb")
              sh "MRUBY_CONFIG=#{mruby_config(vm, board)} rake clean"
            end
          end
          %w[debug prod].each do |mode|
            dir = build_dir(vm, board, mode)
            FileUtils.rm_f(Dir["#{dir}/R2P2*.*"]) if Dir.exist? dir
          end
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
