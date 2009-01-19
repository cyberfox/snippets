require 'rubygems'

# This requires the 'cyberfox-gchart' gem, as the standard gchart gem
# is woefully broken for this kind of graph.  Broken to the point that
# it's an inherent design choice that doesn't work well for this kind
# of chart.  I'm sure that the cyberfox-gchart gem won't work well for
# many kinds of charts, as well.

require 'gchart'

chart = GChart.scatter do |g|
  # Mon @ 1am, Tue @ 4am, Wed @ 7am, Thu @ 11am, Fri @ 3pm, Sat @ 7pm, Sun @ 11pm
  #  points = [[1, 7, 1], [4, 6, 2], [7, 5, 3], [11, 4, 4], [15, 3, 5], [19, 2, 6], [23, 1, 7]]
  xd = [1, 4, 7, 11, 15, 19, 23]
  yd = [7, 6, 5,  4,  3,  2,  1]
  sizes = (1..7).to_a

  g.data = [xd, yd, sizes]

  g.axis(:left) do |axis_left|
    axis_left.labels = ['', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun', ''].reverse
  end

  g.axis(:bottom) do |axis_right|
    axis_right.labels = ['', "12am"] + (1..11).to_a + ["12pm"] + (1..11).to_a + ['']
  end

  g.extras = { 'chm' => 'o,333333,1,1.0,25.0', 'chds' => '-1,24,0,8,0,9' }

  g.colors = [ :black ]
  g.width=800
  g.height=300
end

puts URI.decode(chart.to_url)
chart.write("chart.png")
system('open chart.png')
