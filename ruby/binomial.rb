def binomial(price, strike, rate, vol, expires, steps)
  prices = Array.new(steps+1)
  calls = Array.new(steps+1)

  delta_t = (expires/steps)
  _r = Math::exp(rate*delta_t)

  _rInv = 1.0/_r
  u = Math::exp(vol*Math::sqrt(delta_t))
  d = 1.0/u
  uu = u*u
  pUp = (_r - d)/(u - d)
  pDown = 1.0 - pUp

  prices[0]=price*(d**steps)
  1.upto(steps) {|idx| prices[idx] = uu*prices[idx-1]}
  0.upto(steps) {|idx| calls[idx] = [0.0, prices[idx]-strike].max}
  (steps-1).downto(0) do |cur_step|
    0.upto(cur_step) do |idx|
      calls[idx] = (pUp*calls[idx+1]+pDown*calls[idx])*_rInv
      prices[idx] = d*prices[idx+1]
      calls[idx] = [calls[idx].to_f, (prices[idx]-strike).to_f].max
    end
  end

  calls[0]
end

def binomial_partials(price, strike, rate, vol, expires, steps)
  prices = Array.new(steps+1)
  calls = Array.new(steps+1)
  delta_t = (expires/steps)
  _r = Math::exp(rate*delta_t)
  _rInv = 1.0/_r
  u = Math::exp(vol*Math::sqrt(delta_t))
  d = 1.0/u
  uu = u*u
  pUp = (_r - d)/(u - d)
  pDown = 1.0 - pUp

  prices[0]=price*(d**steps)
  1.upto(steps) {|idx| prices[idx] = uu*prices[idx-1]}
  0.upto(steps) {|idx| calls[idx] = [0.0, (prices[idx]-strike)].max}
  (steps-1).downto(2) do |cur_step|
    0.upto(cur_step) do |idx|
      prices[idx] = d*prices[idx+1]
      calls[idx] = (pDown*calls[idx]+pUp*calls[idx+1])*_rInv
      calls[idx] = [calls[idx], prices[idx]-strike].max
    end
  end

  f22 = calls[2]
  f21 = calls[1]
  f20 = calls[0]

  0.upto(1) do |idx|
    prices[idx] = d*prices[idx+1]
    calls[idx] = (pDown*calls[idx]+pUp*calls[idx+1])*_rInv
    calls[idx] = [calls[idx], prices[idx]-strike].max
  end

  f11 = calls[1]
  f10 = calls[0]

  prices[0] = d*prices[1]
  calls[0] = (pDown*calls[0]+pUp*calls[1])*_rInv
  calls[0] = [calls[0], price-strike].max

  f00 = calls[0]

  delta = (f11-f10)/(price*u-price*d)
  h = 0.5*price*(uu - d*d)
  gamma = ((f22-f21)/(price*(uu-1)) - (f21-f20)/(price*(1-d*d)))/h
  theta = (f21-f00)/(2*delta_t)

  diff = 0.02
  tmp_sigma = vol+diff
  tmp_prices = binomial(price, strike, rate, tmp_sigma, expires, steps)
  vega = (tmp_prices-f00)/diff
  diff = 0.05
  tmp_r = rate+diff
  tmp_prices = binomial(price, strike, tmp_r, vol, expires, steps)

  rho = (tmp_prices-f00)/diff
  [delta, gamma, theta, vega, rho]
end

delta, gamma, theta, vega, rho = binomial_partials(50.0, 55.0, 0.09, 0.20, 0.75, 10)
puts "Delta: #{delta}\nGamma: #{gamma}\nTheta: #{theta}\nVega: #{vega}\nRho: #{rho}\n"

puts binomial(50.0, 55.0, 0.09, 0.20, 1.0, 100)
#puts binomial(25.0, 30.0, 0.03, 0.25, 0.75, 10)
puts binomial(47.49, 50.0, 0.01, 0.1647, 26.0/365.0, 1000)
