#!/bin/env ruby

# Example use:
#   ruby tax_calc.rb 21478 2004-04-15

require 'date'

def months_between(date1=Date.today, date2=Date.today)
  (date1 > date2) ? (recent_date, past_date = date1, date2) : (recent_date, past_date = date2, date1)
  (recent_date.year - past_date.year) * 12 + (recent_date.month - past_date.month)
end

base_amount = ARGV[0].to_f
date_due_string = ARGV[1]
date_due = Date.parse(date_due_string)
date_checked = Date.parse(ARGV[2]) if ARGV[2]
date_checked ||= Date.today

months_ago = months_between(date_due, date_checked)
puts "Months ago: #{months_ago}"
f2f = base_amount * ([22.5, months_ago * 4.5].min / 100.0)
f2p = base_amount * ([25.0, months_ago * 0.5].min / 100.0)
puts "Base F2F Penalty: #{f2f}"
puts "Base F2P Penalty: #{f2p} (limited penalties model)"

AVERAGE_YEARLY_INTEREST = 0.06875
MONTHLY_PENALTY = 0.005

def unlimited_penalties(base_amount, months_ago, f2f)
  total = (1..months_ago).inject(base_amount) do |accum, value|
    accum + accum * (AVERAGE_YEARLY_INTEREST / 12.0) + accum * MONTHLY_PENALTY + ((value <= 5) ? f2f / 5 : 0)
  end
end

def limited_penalties(base_amount, months_ago, f2f, f2p)
  total = (1..months_ago).inject(base_amount) do |accum, value|
    accum + accum * (AVERAGE_YEARLY_INTEREST / 12.0) +
      ((value <= 5) ? f2f / 5 : 0) +
      ((value <= 50) ? f2p / 50 : 0)
  end
end

def error_penalties(base_amount, months_ago, f2f, f2p)
  total = (1..months_ago).inject(base_amount) do |accum, value|
    accum + accum * (AVERAGE_YEARLY_INTEREST / 12.0)
  end

  total + f2f + f2p
end

unlimited = unlimited_penalties(base_amount, months_ago, f2f)
puts "Unlimited penalties: #{unlimited}"
limited = limited_penalties(base_amount, months_ago, f2f, f2p)
puts "Limited penalties: #{limited}"
error = error_penalties(base_amount, months_ago, f2f, f2p)
puts "Error penalties: #{error}"
