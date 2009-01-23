require 'rubygems'

# This requires the 'cyberfox-gchart' gem (0.5.4), as the standard
# gchart gem is woefully broken for this kind of graph.  Broken to the
# point that it's an inherent design choice that doesn't work well for
# this kind of chart.  I'm sure that the cyberfox-gchart gem won't
# work well for many kinds of charts, as well.

require 'gchart'

def make_timecard(triples)
  xd = triples.collect {|triple| triple[0] }
  yd = triples.collect {|triple| triple[1] }
  sizes = triples.collect {|triple| triple[2] }

  GChart.encoding = :text

  chart = GChart.scatter do |g|
    g.data = [xd, yd, sizes]

    g.axis(:left) do |axis_left|
      axis_left.labels = ['', 'Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', '']
    end

    g.axis(:bottom) do |axis_right|
      axis_right.labels = ['', "12am"] + (1..11).to_a + ["12pm"] + (1..11).to_a + ['']
    end

    g.extras = { 'chm' => 'o,333333,1,1.0,25.0', 'chds' => "-1,24,0,8,0,#{sizes.max}", 'chf' => 'bg,s,efefef' }

    g.colors = [ :black ]
    g.width=800
    g.height=300
  end

#  puts URI.decode(chart.to_url)
  chart.write("chart.png")
end

def git_timecard(path = nil)
  if path
    Dir.chdir path
  end
  log = `git log --pretty=format:%at}`.split
  count = []
  8.times { count << [0]*25}

  pairs = log.collect do |ut|
    logtime = Time.at(ut.to_i)
    count[logtime.wday+1][logtime.hour] += 1
    [logtime.wday+1, logtime.hour]
  end.uniq

  max = 0
  triples = pairs.collect do |pair|
    total = count[pair.first][pair.last]
    max = [max, total].max

    # Swap, because in the resultant graph hours are 'x', and days are 'y'.
    [pair.last, pair.first, total]
  end

  make_timecard(triples)
end

if ARGV[0]
  git_timecard(ARGV[0])
  system('open chart.png')
end
