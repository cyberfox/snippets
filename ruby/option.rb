# Your risk_free rate is a system-wide rate, your volatility, price,
# and dividend yield are derived from an underlying security.  The
# right thing to do would be to pass a Stock into the constructor, and
# have the option defer to its values for all those, and to a global
# for risk free rates.  (With the potential to override, of course.)

# TODO -- Currently only working for European options; need to figure out how to convert for American options.
class Option
  def self.call(price, strike, years, div_yield=nil)
    new(:call, price, strike, years, div_yield)
  end

  def self.put(price, strike, years, div_yield=nil)
    new(:put, price, strike, years, div_yield)
  end

  def value(risk_free, volatility)
    precision(black_scholes(risk_free, volatility))
  end

  def delta(risk_free, volatility)
    subtract = (@is_call ? 0 : 1)
    step = d1(risk_free, volatility)
    precision(Math::exp(-@div_yield*@expires)*(cnd(step) - subtract))
  end

  def rho(risk_free, volatility)
    if @is_call
      precision(@strike*@expires*Math::exp(-risk_free*@expires)*cnd(d2(risk_free, volatility)))
    else
      precision(-@strike*@expires*Math::exp(-risk_free*@expires)*cnd(-d2(risk_free, volatility)))
    end
  end

  def theta(risk_free, volatility)
    my_d1 = d1(risk_free, volatility)
    my_d2 = d2(my_d1, risk_free, volatility)

    nd1 = (1/Math::sqrt(2*Math::PI))*Math::exp(-0.5*(my_d1**2))

    first_half = -(nd1 * volatility * @price) / (2*Math::sqrt(@expires))
    if @is_call
      precision(first_half+@div_yield*cnd(my_d1)*@price-risk_free*@strike*Math::exp(-risk_free*@expires)*cnd(my_d2))
    else
      precision(first_half-@div_yield*cnd(-my_d1)*@price+risk_free*@strike*Math::exp(-risk_free*@expires)*cnd(-my_d2))
    end
  end

  def gamma(risk_free, volatility)
    my_d1 = d1(risk_free, volatility)
    nd1 = (1/Math::sqrt(2*Math::PI))*Math::exp(-0.5*(my_d1**2))

    precision(nd1*Math::exp(-@div_yield*@expires)/(@unadjusted*volatility*Math::sqrt(@expires)))
  end

  def vega(risk_free, volatility)
    my_d1 = d1(risk_free, volatility)
    nd1 = (1/Math::sqrt(2*Math::PI))*Math::exp(-0.5*(my_d1**2))
    precision(Math::sqrt(@expires)*nd1*@price)
  end

  private
  def initialize(option_type, price, strike, years_to_expiration, div_yield=nil)
    @is_call = (option_type == :call)
    @price = price
    @strike = strike
    @expires = years_to_expiration
    @div_yield = div_yield || 0.0
    @unadjusted = @price
    @price = @price * (Math::exp(-@div_yield*@expires))
  end

  def precision(x)
    (x*10000.0).round/10000.0
  end

  def cnd(x)
    a1, a2, a3, a4, a5 = 0.31938153, -0.356563782, 1.781477937, -1.821255978, 1.330274429
    l = x.abs
    k = 1.0 / (1.0 + 0.2316419 * l)
    w = 1.0 - 1.0 / Math::sqrt(2*Math::PI)*Math::exp(-l*l/2.0) * (a1*k + a2*k*k + a3*(k**3) + a4*(k**4) + a5*(k**5))
    w = 1.0 - w if x < 0
    return w
  end

  def d1(risk_free, volatility)
    (Math::log(@price/@strike)+(risk_free+(volatility**2)/2.0)*@expires)/(volatility*Math::sqrt(@expires))
  end

  def d2(*params)
    my_d1 = 0.0
    risk_free, volatility = if params.length == 2
                              my_d1 = d1(*params)
                              params
                            else
                              my_d1 = params.first
                              params[1..-1]
                            end
    my_d1 - volatility*Math::sqrt(@expires)
  end

  def black_scholes(risk_free, volatility)
    base = d1(risk_free, volatility)
    second = d2(base, risk_free, volatility)

    if @is_call
      @price*cnd(base)-@strike*Math::exp(-risk_free*@expires)*cnd(second)
    else
      @strike*Math::exp(-risk_free*@expires)*cnd(-second)-@price*cnd(-base)
    end
  end
end

# Stock - 50, Strike - 55, 3/4ths of a year left, 9% risk-free interest rate, 20% volatility
high_call = Option.call(50.0, 55.0, 0.75)
puts "High call: Price @ 50, Strike @ 55, 3/4 year left, 9% risk-free rate, 20% volatility"
puts "Value: #{high_call.value(0.09, 0.20)} vs. 2.8392"
puts "Delta: #{high_call.delta(0.09, 0.20)} vs. 0.4705"
puts "Rho:   #{high_call.rho(0.09, 0.20)} vs. 15.5152"
puts "Theta: #{high_call.theta(0.09, 0.20)} vs. -4.1588"
puts "Gamma: #{high_call.gamma(0.09, 0.20)} vs. 0.0459"
puts "Vega:  #{high_call.vega(0.09, 0.20)} vs. 17.2275"
puts

high_put = Option.put(50.0, 55.0, 0.75)
puts "High put: Price @ 50, Strike @ 55, 3/4 year left, 9% risk-free rate, 20% volatility"
puts "Value: #{high_put.value(0.09, 0.20)} vs. 4.2492"
puts "Delta: #{high_put.delta(0.09, 0.20)} vs. -0.5295"
puts "Rho:   #{high_put.rho(0.09, 0.20)} vs. -23.0423"
puts "Theta: #{high_put.theta(0.09, 0.20)} vs. 0.4681"
puts "Gamma: #{high_put.gamma(0.09, 0.20)} vs. 0.0459"
puts "Vega:  #{high_put.vega(0.09, 0.20)} vs. 17.2275"
puts

# Stock - 25, Strike - 30, 1/2 a year left, 3% risk-free interest rate, 25% volatility
low_call = Option.call(25.0, 30.0, 0.5)
puts "Low call: Price @ 25, Strike @ 30, 1/2 year left, 3% risk-free rate, 25% volatility"
puts "Value: #{low_call.value(0.03, 0.25)} vs. 0.4417"
puts "Delta: #{low_call.delta(0.03, 0.25)} vs. 0.1954"
puts "Gamma: #{low_call.gamma(0.03, 0.25)} vs. 0.0625"
puts "Theta: #{low_call.theta(0.03, 0.25)} vs. -0.0037"
puts

# Stock - 50, Strike - 55, 3/4ths of a year left, 9% risk-free interest rate, 20% volatility
div_call = Option.call(50.0, 55.0, 0.75, 0.03)
puts "Dividend call: Price @ 50, Strike @ 55, 3/4 year left, 9% risk-free rate, 20% volatility, 3% dividend yield"
puts "Value: #{div_call.value(0.09, 0.20)} vs. 2.3442"
puts "Delta: #{div_call.delta(0.09, 0.20)} vs. 0.4099"
puts "Rho:   #{div_call.rho(0.09, 0.20)} vs. 13.6132"
puts "Theta: #{div_call.theta(0.09, 0.20)} vs. -3.2245"
puts "Gamma: #{div_call.gamma(0.09, 0.20)} vs. 0.0441"
puts "Vega:  #{div_call.vega(0.09, 0.20)} vs. 16.5430"
puts

# Stock - 50, Strike - 55, 3/4ths of a year left, 9% risk-free interest rate, 20% volatility
div_put = Option.put(50.0, 55.0, 0.75, 0.03)
puts "Dividend put: Price @ 50, Strike @ 55, 3/4 year left, 9% risk-free rate, 20% volatility, 3% dividend yield"
puts "Value: #{div_put.value(0.09, 0.20)} vs. 4.8667"
puts "Delta: #{div_put.delta(0.09, 0.20)} vs. -0.5678(!)"
puts "Rho:   #{div_put.rho(0.09, 0.20)} vs. -24.9443"
puts "Theta: #{div_put.theta(0.09, 0.20)} vs. -0.0642"
puts "Gamma: #{div_put.gamma(0.09, 0.20)} vs. 0.0441 (same as call)"
puts "Vega:  #{div_put.vega(0.09, 0.20)} vs. 16.5430 (same as call)"

puts "$47.49, strike @ 50, 26 days 'til expiraton, 1% interest rate, 16.47% volatility."
puts Option.call(47.49, 50.0, 26.0/365.0).value(0.01, 0.1647)

#puts Option.call(50.0, 55.0, 1.0).value(0.09, 0.20)
