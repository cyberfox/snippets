require 'rubygems'
gem 'hpricot'
require 'hpricot'
require 'open-uri'

class Object
  def blank?
    nil? || to_s == ''
  end
end

tag = ARGV[0]
subdomain = ARGV[1]
token = ARGV[2]

if tag.blank? || subdomain.blank? || token.blank?
  puts "Usage:\n\tgen_list {tag} {subdomain} {API token}"
  exit
end

auth_info = [token, '']

tag_list = open("http://#{subdomain}.highrisehq.com/tags.xml", :http_basic_authentication => auth_info)
body = tag_list.read
hp = Hpricot.parse(body)

tag_ids = (hp / 'tag').collect {|x| [(x / 'name').text, (x / 'id').text]}
tag_map = tag_ids.inject({}) { |hash, entry| hash.merge(entry.first => entry.last)}

#puts "Found tags: #{tag_map.keys.join(", ")}"

tag_url = "http://#{subdomain}.highrisehq.com/tags/#{tag_map[tag]}.xml"

hp = Hpricot.parse(open(tag_url, :http_basic_authentication => auth_info));

emails = (hp / '//email-address/address/').collect {|x| x.content}
emails.sort!
puts emails.join(', ')
