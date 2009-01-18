#!/usr/bin/env ruby
require 'svn/client'

# Get the first argument from the command line
target = $*[0]

ctx = Svn::Client::Context.new

receiver = Proc.new do |path, info|
  puts "Path: #{path}"
  puts "URL: #{info.url}"
  puts "Repository Root: #{info.repos_root_url}"
  puts "Repostiory UUID: #{info.repos_UUID}"
  puts "Revision: #{info.rev}"
  puts "Node Kind: #{info.kind}"
  puts "Schedule: #{info.schedule}"
  puts "Last Changed Author: #{info.last_changed_author}"
  puts "Last Changed Rev: #{info.last_changed_rev}"
  last_changed_time = Time.from_apr_time(info.last_changed_date)
  puts "Last Changed Date: #{last_changed_time}"
end

Svn::Client.info(target, nil, nil, receiver, false, ctx)
