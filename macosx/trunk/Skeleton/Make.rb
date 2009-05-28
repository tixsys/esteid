#!/usr/bin/ruby

# Copyright (c) 2009 Janek Priimann

require 'rubygems'
require 'date'
require 'fileutils'
require 'optparse' 
require 'ostruct'
require 'pathname'
require 'tmpdir'

class Application
	VERSION = '1.0'
	
	attr_reader :options
	
	def initialize(arguments, stdin)
		@arguments = arguments
		@stdin = stdin
		@path = Pathname.new(__FILE__).realpath.dirname
		@error = false
		@tmp = nil
		
		@options = OpenStruct.new
		@options.verbose = false
		@options.skeleton = nil
		@options.input = nil
		@options.output = nil
		@options.version = nil
	end
	
	def run
		if parsed_options?
			begin
				unless @options.input.nil?
					run_build
				else
					run_usage
				end
			rescue	
				@error = true
				raise
			ensure
				FileUtils.rm_rf(@tmp) unless @tmp.nil?
			end
		else
			run_usage
		end
		
		exit 1 if @error
	end
	
	protected
	
	def run_build
		puts "Building application..." if @options.verbose
		
		if @tmp.nil?
			@tmp = File.join(Dir.tmpdir, 'esteidskeleton')
			FileUtils.rm_rf(@tmp) if File.exists? @tmp
			Dir.mkdir(@tmp)
		end
		
		if @options.output.nil?
			@options.output = Pathname.new(@path).join('../build/Release/', File.basename(@options.input) + '.app').to_s
		end
		
		if @options.skeleton.nil?
			@options.skeleton = Pathname.new(@path).join(File.basename(@options.output)).to_s
		end
		
		@options.input = Pathname.new(@path).join(@options.input).to_s
		@options.output = Pathname.new(@path).join(@options.output).to_s
		@options.skeleton = Pathname.new(@path).join(@options.skeleton).to_s
		
		# Copy the skeleton
		Dir.glob(File.join(@options.skeleton, '**/*')).each do |path|
			rpath = File.join(@tmp, path[@options.skeleton.length, path.length - 1])
			
			if File.directory? path
				Dir.mkdir(rpath)
			else
				puts "Copying file #{path[@options.skeleton.length, path.length - 1]} to #{File.basename(@options.output)}" if @options.verbose
				FileUtils.cp(path, rpath)
			end
		end
		
		# Copy the executable
		puts "Copying executable #{File.basename(@options.input)} to #{File.basename(@options.output)}" if @options.verbose
		FileUtils.cp(@options.input, File.join(@tmp, 'Contents/MacOS', File.basename(@options.output, '.app')))
		
		# Copy the application from temporary destination to the final destination
		puts "Copying application #{File.basename(@options.output)} to #{@options.output}" if @options.verbose
		FileUtils.mkdir_p(File.dirname(@options.output)) unless File.exists? File.dirname(@options.output)
		FileUtils.rm_rf(@options.output) if File.exists? @options.output
		FileUtils.cp_r(@tmp, @options.output)
		
		# Hide the file extension
		`setfile -a E #{@options.output}`
		
		puts "\n" if @options.verbose
	end
	
	def run_usage
		puts options.to_s
		puts ""
		puts "Example:"
		puts "\t./Make.rb -V -i qesteidutil -o qesteidutil.app"
		puts ""
		puts "#{File.basename(__FILE__)} is a script for creating applications"
	end
	
	def run_version
		puts "#{File.basename(__FILE__)} version #{VERSION}"
	end
	
	def parsed_options?		
		options.parse!(@arguments) rescue return false
		
		@options.verbose = false if @options.quiet
		
		true	
	end
	
	def options
		opts = OptionParser.new
		opts.banner = "Usage: #{File.basename(__FILE__)} [options]"
		opts.on('-v', '--version', 'Show version') { run_version; exit 0 }
		opts.on('-h', '--help', 'Show this message') { run_usage; exit 0 }
		opts.on('-V', '--verbose', 'Run verbosely') { @options.verbose = true }	
		opts.on('-q', '--quiet', 'Run quietly') { @options.quiet = true }
		opts.on('-s', '--skeleton [DIR]', 'The skeleton to use') { |dir| @options.skeleton = dir }
		opts.on('-i', '--input [FILE]', 'Use file for input') { |file| @options.input = file }
		opts.on('-o', '--output [DIR]', 'Use directory for output ') { |dir| @options.output = dir }
		
		return opts
	end
end

application = Application.new(ARGV, STDIN)
application.run
