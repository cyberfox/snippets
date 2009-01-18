class DuplicateManager
  attr_reader :existing

  def initialize(opt_file=nil)
    load_checksums(opt_file) unless opt_file.nil?
  end

  def load_checksums(file)
    @existing = YAML.load(File.open(file).read)
  end

  def save_checksums(file)
    File.open(file, 'w+').write(YAML.dump(@existing))
  end

  # The imported file should be in the form: {hash} {filepath}
  # i.e. 281bd8ecf036183516642ebca8cae38c wham/Unknown/careless whispers.mp3
  def import_checksums(file)
    @existing = File.open(file).inject({}) do |accum, line|
      hash = line.split(' ').first
      accum[hash] = line[hash.length+1..-1].chomp
      accum
    end
  end

  def find_duplicates(duplicate_manager)
    @existing.select do |hash, line|
      duplicate_manager.existing.has_key?(hash)
    end
  end
end

# diff('music.yml', '../mrs/music.yml')
def act_on_differences(file1, file2)
  old = DuplicateManager.new(file1)
  current = DuplicateManager.new(file2)

  yield old.find_duplicates(current)
end

def kill_duplicates(file1, file2)
  act_on_differences(file1, file2) do |diffs|
    diffs.collect {|x| x.last}.collect {|file| File.unlink(file) }
  end
end

if ARGV.length == 2
  act_on_differences(ARGV[1], ARGV[2]) do |diffs|
    puts diffs.length
  end
end
