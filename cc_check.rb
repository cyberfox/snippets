#!/usr/bin/ruby

def check_value(card)
  even=false
  accum = 0

  card.each_byte do |c|
    digit = c - '0'[0]
    addval = digit * (even ? 1 : 2)
    addval += -9 if addval > 9
    accum += addval
    even = !even
  end
  accum
end

def valid(card)
  check = check_value(card)
  [check, (check % 10 == 0)]
end

verify = valid(ARGV[0])
puts verify.first.to_s + ": " + (verify.last ? "Valid." : "Invalid.")
