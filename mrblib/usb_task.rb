class LoadError < StandardError
  # ScriptError is the super class in CRuby
end

class Object
  alias _puts puts
  def puts(*args)
    USB.tud_task
    args << "" if args.empty?
    args.each do |arg|
      _puts arg
      USB.tud_task
    end
    return nil
  end

  alias _print print
  def print(*args)
    USB.tud_task
    args << "" if args.empty?
    args.each do |arg|
      _print arg
      USB.tud_task
    end
    return nil
  end
end

while true
  USB.tud_task
  sleep_ms 5
end
