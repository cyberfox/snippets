class Regexp
  def |(other)
    Regexp.new("(?:" + source + ")|(?:" + other.source + ")")
  end
end
