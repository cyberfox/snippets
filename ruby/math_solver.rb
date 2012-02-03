require 'rubygems'
require 'open-uri'
require 'json'
require 'pp'

class Array
  def delete_first(x)
    self.delete_at(self.index(x) || self.length)
  end
end

class String
  def each_char_with_index
    split(//).each_with_index do |c,i|
      yield c, i
    end
  end
end

class MathSolver
  Expression = Struct.new(:start_column, :expression, :op, :solutions) unless defined? Expression

  def expression_count
    @expressions.length
  end

  def initialize(puzzle_id=1)
    @body = open("http://home.avvanta.com/~scruff/StefPuzzles/BlockTransferMathematics/PuzzleData/Data#{puzzle_id || 1}.json").read

    # Trim bad characters; apparently only necessary for Data1.json
    while @body[0] > 127
      @body = @body[1..-1]
    end

    @structure = JSON.parse(@body)

    @columns = @structure['PuzzleHeader']['Cols'].map { |c| c['Numbers'] }

    @cells = @structure['PuzzleBody']['Rows'].map { |r| r['Cols'].map { |c| [c['CellType'], c['CellValue']] } }.flatten(1)
  end

  def generate_expressions(cells)
    @expressions = []
    current_exp = nil
    op = nil
    cells.each_with_index do |cell, idx|
      current_exp ||= Expression.new(idx, "")

      case cell.first
        when 0 # Not-yet-known value
          current_exp.expression << '#'
        when 1 # Operations (+-*/=)
          current_exp.expression << cell.last
          current_exp.op = cell.last if cell.last != '='
        when 2 # Formula seperators
          @expressions << current_exp unless current_exp.expression.empty?
          current_exp = nil
        when 3 # Digits (in later iterations)
          current_exp.expression << cell.last
      end
    end
  end

  def generate_possibilities(expression_index)
    x = @expressions[expression_index]

    offset = x.start_column
    sets = [[]]
    x.expression.each_char_with_index do |ch, idx|
      if ch == '#'
        sets.last << @columns[(offset+idx) % @structure['NumCols']]
      elsif ch >= '0' && ch <= '9'
        sets.last << [ch.to_i]
      else
        sets << []
      end
    end

    x.solutions = sets.collect do |digit_set|
      digit_set.first.product(*digit_set[1..-1]).uniq
    end
  end

  def solve(expression_index)
    exp = @expressions[expression_index]

    all_values = generate_possibilities(expression_index)

    # X, Y, Z (variables should always be 3 entries long)
    variants = all_values.collect do |variables|
      # (for all possible variants of variables)
      variables.reject {|x| x.first == 0}.collect do |potentials|
        # (get this particular variant)
        potentials.inject(0) { |accum,val| accum*10 + val}
      end
    end

    answers = variants[0].product(variants[1]).product(variants[2]).collect {|x| x.flatten}

    answers.reject do |keys|
      eval("#{keys[0]}.to_f #{exp.op} #{keys[1]}.to_f") != keys.last
    end
  end

  def solve_board
    loops = 0
    begin
      loops += 1
      found_move = false

      generate_expressions(@cells)

      solutions = []
      (0...@expressions.length).each do |x|
        solutions << solve(x)
      end

      solutions.each_with_index do |expression_solutions, idx|
        exp = @expressions[idx]
        expressions = expression_solutions.collect {|solution| "#{solution[0]}#{exp.op}#{solution[1]}=#{solution[2]}"}
        exp.solutions = expressions
        exp.expression.each_char_with_index do |ch, idx|
          if ch == '#'
            possibilities = expressions.collect {|e| e[idx]}.uniq
            if possibilities.length == 1
              collapsed_char = "%c" % possibilities.first
              exp.expression[idx] = collapsed_char
              @cells[exp.start_column + idx] = [3, collapsed_char]
              @columns[(exp.start_column + idx) % @structure['NumCols']].delete_first(collapsed_char.to_i)
              found_move = true
            end
          end
        end
      end
    end while found_move

    puts "Passes: #{loops}"
    puts @expressions
  end

  def dump_expression(expression_index)
    x = @expressions[expression_index]
    "Expression #{expression_index} starts at column #{x.start_column}: #{x.expression}"
  end

  def debug
    puts "Columns: #{@columns.inspect}"
  end
end

def time
  start = Time.now.to_f
  yield
  puts "Elapsed time: #{Time.now.to_f - start}"
end

m = MathSolver.new(ARGV[0])
time { m.solve_board }
