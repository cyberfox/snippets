# One hard choice is determining if the model is the One True Source
# (i.e. delete all enumerations not defined in the model) or just a
# better way of listing them.  I lean towards the first, but it's not
# backwards compatible.  One solution would be that if #enum_key is
# used, then it's the One True Source, if it's not, leave the database
# alone.  Another option is to make it explicit.  The model developer
# has to choose to clear otherwise undefined enumerations by adding a
# call after the end of their enum_key calls, let's say... #enum_flush_unset
# It's a terrible name, but it works for now.

module EnumerationsSugar
  def self.assert_includes(hash, *keys)
    missing = keys.select {|key| !hash.include?(key) }
    raise(ArgumentError, "Missing key(s): #{missing.join(", ")}") unless missing.empty?
  end

  def self.included(base)
    base.class_eval do
      @@enumeration_names = []

      def base.enum_flush_unset
        eliminated = find(:all, :conditions => ['name NOT IN (?)', @@enumeration_names])
        self.enumeration_model_updates_permitted = true
        eliminated.each do |enum|
          ActiveRecord::Base.logger.warn "Deleting enumeration #{enum.class}['#{enum.name}']"
          enum.destroy
        end
        self.enumeration_model_updates_permitted = false
      end

      def base.enum_key(options)
        EnumerationsSugar.assert_includes options, :name
        # In case it was passed in as a symbol
        options[:name] = options[:name].to_s
        exists = self.find_by_name(options[:name])
        @@enumeration_names << options[:name]
        self.enumeration_model_updates_permitted = true
        if exists
          # Update the database if the keys change, so you can manage
          # keys entirely from the model.
          changes = options.keys.select do |key|
            exists[key].to_s != options[key].to_s
          end
          update_attributes changes unless changes.empty?
        else
          begin
            self.create!(options) unless exists
          rescue Exception => e
            ActiveRecord::Base.logger.warn "#{e.class}: #{e.message}"
          end
        end
        self.enumeration_model_updates_permitted = false
      end
    end
  end
end
