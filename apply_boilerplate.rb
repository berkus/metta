#!/usr/bin/env ruby
#
# Copyright 2007 - 2009, Stanislav Karchebnyy <berkus+metta@madfire.net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See file LICENSE_1_0.txt or a copy at http:#www.boost.org/LICENSE_1_0.txt)
#
#
# Apply license and modeline changes to text source files.
#
require 'find'

exclude_dirs = ['./vesper/src/lib/bstrlib', './vesper/src/build']
no_license_dirs = ['./vesper/src/lib/oskit', './vesper/src/lib/atomic']

class Array
    def do_not_has?(path)
        count {|x| path.start_with?(x)} === 0
    end
end

license = IO.readlines('license_header').join
modelines = IO.readlines('modelines.txt').join
exts = {
    '.cpp'=>[license, modelines],
    '.c'=>[license, modelines],
    '.h'=>[license, modelines],
    '.s'=>[license.gsub("//",";"), modelines.gsub("//",";")],
    '.rb'=>[license.gsub("//","#"), modelines.gsub("//","#")]
}

ok_count = 0
modified_count = 0

Find.find('./') do |f|
    if File.file?(f) && exts.include?(File.extname(f)) && exclude_dirs.do_not_has?(File.dirname(f))
        lic = exts[File.extname(f)][0]
        mod = exts[File.extname(f)][1]
        modified = false
        content = IO.readlines(f).join
        if content.index(lic).nil? && no_license_dirs.do_not_has?(File.dirname(f))
            content = lic + content
            modified = true
        end
        if content.index(mod).nil?
            content = content + mod
            modified = true
        end
        if modified
            File.open(f+".new", "w") do |out|
                out.write content
            end
            begin
                File.rename(f+".new", f)
            rescue SystemCallError
                puts "Couldn't rename file #{f+".new"} to #{f}:", $!
            end
            puts "#{f} is UPDATED"
            modified_count += 1
        else
            puts "#{f} is ok"
            ok_count += 1
        end
    end
end

puts "#{modified_count} files changed, #{ok_count} files ok."

# kate: indent-width 4; replace-tabs on;
# vim: set et sw=4 ts=4 sts=4 cino=(4 :

