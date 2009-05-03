#!/usr/bin/ruby

# Copyright (c) 2009 Janek Priimann

require 'rubygems'
require 'date'
require 'fileutils'
require 'optparse' 
require 'ostruct'
require 'pathname'
require 'plist'
require 'tmpdir'

class Application
	VERSION = '1.0'
	PKGMAJOR = 1
	PKGMINOR = 0
	
	attr_reader :options
	
	def initialize(arguments, stdin)
		@arguments = arguments
		@stdin = stdin
		@path = Pathname.new(__FILE__).realpath.dirname
		@error = false
		@tmp = nil
		
		@options = OpenStruct.new
		@options.arch = 'universal'
		@options.verbose = false
		@options.quiet = false
		@options.nocp = false
		@options.binaries = 'build/Release'
		@options.prerequisites = '/usr/local'
		@options.packages = 'build/Release'
		@options.pkgapp = '/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS'
	end
	
	def run
		if parsed_options?
			begin
				if @arguments.last == 'prerequisites'
					run_prerequisites
				elsif @arguments.last == 'binaries'
					run_binaries
				elsif @arguments.last == 'clean'
					run_clean
				elsif @arguments.last == 'manifest'
					run_manifest
				elsif @arguments.last == 'packages'
					run_packages
				elsif @arguments.last == 'installer'
					run_clean
					run_prerequisites
					run_binaries
					run_packages
					run_manifest
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
	
	def run_binaries
		puts "Creating installer binaries..." if @options.verbose
		
		if !@options.nocp
			
		else
			puts "Skipping building cross-platform binaries" if @options.verbose
		end
		
		# Build all xcode targets
		puts "Building xcode projects..." if @options.verbose
		`xcodebuild -project Project.xcodeproj -configuration Release -target Main`
		
		puts "Building tokend..." if @options.verbose
		`xcodebuild -project tokend/Tokend.xcodeproj -configuration Deployment -target world`
		
		puts "\n" if @options.verbose
	end
	
	def run_clean
		puts "Cleaning installer..." if @options.verbose
		
		if File.exists? File.join(@path, @options.binaries)
			puts "Cleaning #{File.join(@path, @options.binaries)}" if @options.verbose
			FileUtils.rm_rf(File.join(@path, @options.binaries))
		end
		
		if @options.binaries != @options.packages
			if File.exists? File.join(@path, @options.packages)
				puts "Cleaning #{File.join(@path, @options.packages)}" if @options.verbose
				FileUtils.rm_rf(File.join(@path, @options.packages))
			end
		end
		
		puts "\n" if @options.verbose
	end
	
	def run_prerequisites
		puts "Creating installer prerequisites..." if @options.verbose
		
		# TODO: Should !!!CORRECTLY!!! build libraries for the specified architecture(s).
		# TODO: Move stuff from Build.sh here, some prerequisites compile on OS X only after some tweaking.
		
		puts "\n" if @options.verbose
	end
	
	def run_manifest
		puts "Creating installer manifest..." if @options.verbose
		
		# TODO: For automatic software updates?
		
		puts "\n" if @options.verbose
	end
	
	def run_packages
		puts "Creating installer packages..." if @options.verbose
		
		if @tmp.nil?
			@tmp = File.join(Dir.tmpdir, 'esteidmake')
			FileUtils.rm_rf(@tmp) if File.exists? @tmp
			Dir.mkdir(@tmp)
		end
		
		all_packages.each do |options|
			# Load package configuration
			name = options[:name]
			identifier = options[:identifier]
			version = options[:version]
			location = options[:location]
			patterns = options[:files]
			restart = options[:restart]
			root = Pathname.new(@path).join(options[:root]).to_s unless options[:root].nil?
			extension = '.mpkg'
			files = []
			
			if patterns.instance_of? Array
				patterns.each { |pattern| files.concat(Dir.glob(Pathname.new(@path).join(pattern).to_s)) }
			else
				files.concat(Dir.glob(Pathname.new(@path).join(patterns).to_s))
			end
			
			# Check if we're not dealing with a meta-package ie a package that contains other packages
			files.each { |file| extension = '.pkg' unless file.include? '.pkg' }
			
			puts "Creating package #{name + extension}..." if @options.verbose
			
			# Basic error checking
			if name.nil?
				puts "Name is missing for package #{name + extension}. Stopped." if !@options.quiet
				raise 'No name'
			end
			
			if files.empty?
				puts "No files match pattern '#{options[:files]}' for package #{name + extension}. Stopped." if !@options.quiet
				raise 'No files'
			end
			
			if identifier.nil? || version.nil?
				file = Pathname.new(files.first).join('Contents', 'Info.plist').to_s
				
				if File.exists? file
					plist = Plist.parse_xml(file)
					identifier = plist['CFBundleIdentifier'] if identifier.nil?
					version = plist['CFBundleShortVersionString'] if version.nil?
				end
				
				if identifier.nil?
					puts "Identifier is missing for package #{name + extension}. Stopped." if !@options.quiet
					raise 'No identifier'
				end
				
				if version.nil?
					puts "Version is missing for package #{name + extension}. Stopped." if !@options.quiet
					raise 'No version'
				end
			end
			
			# Create the package skeleton
			package = File.join(@tmp, name + extension)
			Dir.mkdir(package)
			contents = File.join(package, 'Contents')
			Dir.mkdir(contents)
			
			# Normal package
			if extension == '.pkg'
				# Copy files to pax archive root directory
				archive = File.join(@tmp, 'Archive')
				FileUtils.rm_rf(archive) if File.exists? archive
				Dir.mkdir(archive)
				
				files.each do |file|
					puts "Archiving file #{file} for package #{name + extension}" if @options.verbose
					
					if root.nil?
						FileUtils.cp_r(file, archive)
					else
						dir = File.join(archive, File.dirname(file[root.length, file.length - 1]))
						
						FileUtils.mkdir_p dir
						FileUtils.cp_r(file, dir)
					end
				end
				
				# Create the gzipped pax archive
				puts "Creating file Archive.pax.gz for package #{name + extension}" if @options.verbose
				FileUtils.cd(archive) { `pax -w . | gzip -c > #{File.join(contents, 'Archive.pax.gz')}` }
				
				# Create the so-called 'Bill Of Materials' file (bom)
				puts "Creating file Archive.bom for package #{name + extension}" if @options.verbose
				`mkbom #{archive} #{File.join(contents, 'Archive.bom')}`
				
				# Create the Info.plist file
				puts "Creating file Info.plist for package #{name + extension}" if @options.verbose
				{ 
					'CFBundleIdentifier' => identifier,
					'CFBundleShortVersionString' => version,
					'IFMajorVersion' => PKGMAJOR,
					'IFMinorVersion' => PKGMINOR,
					'IFPkgFlagAllowBackRev' => false,
					'IFPkgFlagAuthorizationAction' => 'RootAuthorization',
					'IFPkgFlagDefaultLocation' => (location.nil?) ? '/' : location,
					'IFPkgFlagFollowLinks' => true,
					'IFPkgFlagInstallFat' => false,
					'IFPkgFlagInstalledSize' => `du -sk #{archive} | cut -f 1`.to_i,
					'IFPkgFlagIsRequired' => false,
					'IFPkgFlagOverwritePermissions' => false,
					'IFPkgFlagRelocatable' => false,
					'IFPkgFlagRestartAction' => (restart.nil?) ? 'None' : restart,
					'IFPkgFlagRootVolumeOnly' => true,
					'IFPkgFlagUpdateInstalledLanguages' => false,
					'IFPkgFormatVersion' => HugeNum.new('0.10000000149011612') #Ugh!
				}.save_plist(File.join(contents, 'Info.plist'))
				
				# Create the PkgInfo file
				puts "Creating file PkgInfo for package #{name + extension}" if @options.verbose
				file = File.new(File.join(contents, 'PkgInfo'), 'w')
				file.puts "pkmkrpkg1"
				file.close
			# Meta-package
			else
				packages = File.join(contents, 'Packages')
				Dir.mkdir(packages)
				
				outlines = ""
				choices = ""
				
				# Copy packages
				files.each do |file|
					puts "Copying #{file} to package #{name + extension}" if @options.verbose
					
					FileUtils.cp_r(file, packages)
					
					plist = Plist.parse_xml(File.join(file, 'Contents', 'Info.plist'))
					pidentifier = plist['CFBundleIdentifier']
					pversion = plist['CFBundleShortVersionString']
					psize = plist['IFPkgFlagInstalledSize']
					
					outlines += "<line choice=\"#{pidentifier}.choice\" />\n"
					choices += "<choice id=\"#{pidentifier}.choice\">\n"
					choices += "<pkg-ref id=\"#{pidentifier}\" />\n"
					choices += "</choice>\n"
					choices += "<pkg-ref id=\"#{pidentifier}\" version=\"#{pversion}\" installKBytes=\"#{psize}\" auth=\"Root\">file:./Contents/Packages/#{File.basename(file)}</pkg-ref>\n"
				end
				
				# Create the distribution.dist file
				file = File.new(File.join(contents, 'distribution.dist'), 'w')
				file.puts "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
				file.puts "<installer-script minSpecVersion=\"1.000000\" authoringTool=\"org.esteid.make\" authoringToolVersion=\"#{VERSION}\" authoringToolBuild=\"1\">"
				file.puts "<options customize=\"never\" allow-external-scripts=\"yes\" rootVolumeOnly=\"true\" />"
				file.puts "<background file=\"Background.tiff\" alignment=\"center\" scaling=\"none\" />"
				file.puts "<welcome file=\"Welcome.html\" />"
				file.puts "<readme file=\"ReadMe.html\" />"
				file.puts "<license file=\"License.html\" />"
				file.puts "<conclusion file=\"Conclusion.html\" />"
				file.puts "<choices-outline>"
				file.puts outlines
				file.puts "</choices-outline>"
				file.puts choices
				file.puts "</installer-script>"
				file.close
			end
			
			# Copy additional resources
			extras = Pathname.new(@path).join('Installer', name).to_s
			
			resources = File.join(contents, 'Resources')
			Dir.mkdir(resources)
			
			if File.exists? extras
				Dir.glob(File.join(extras, '**/*')).each do |path|
					rpath = File.join(resources, path[extras.length, path.length - 1])
					
					if File.directory? path
						Dir.mkdir(rpath)
					else
						puts "Copying file #{path[extras.length, path.length - 1]} to package #{name + extension}" if @options.verbose
						FileUtils.cp(path, rpath)
					end
				end
				
				# Create the package_version file
				if extension == '.pkg'
					puts "Creating file package_version for package #{name + extension}" if @options.verbose
					file = File.new(File.join(resources, 'package_version'), 'w')
					file.print "major: #{PKGMAJOR}\n"
					file.print "minor: #{PKGMINOR}"
					file.close
				end
			end
			
			# Copy helper tools
			helpers = options[:helpers]
			
			unless helpers.nil?
				helpers.each do |helper|
					path = File.join(@options.pkgapp, helper)
					
					if File.exists? path
						puts "Copying #{helper} to #{name + extension}" if @options.verbose
						FileUtils.cp(path, File.join(resources, helper))
					else
						puts "#{path} is missing for package #{name + extension}. Stopped." if !@options.quiet
						raise 'No helper'
					end
					
				end
			end
			
			# Copy the package from temporary destination to the final destination
			puts "Copying package #{name + extension} to #{@options.packages}" if @options.verbose
			dst = Pathname.new(@path).join(@options.packages, name + extension).to_s
			FileUtils.rm_rf(dst) if File.exists? dst
			FileUtils.cp_r(package, dst)
			
			# Hide the file extension
			`setfile -a E #{dst}`
			
			# Finally create the .tar.gz file
			if extension == '.mpkg'
				FileUtils.cd(Pathname.new(@path).join(@options.packages).to_s) { `tar -czf #{name + '.tar.gz'} #{name + extension}` }
			end
		end
		
		puts "\n" if @options.verbose
	end
	
	def run_usage
		puts all_options.to_s
		puts ""
		puts "Available commands:"
		puts "\tbinaries"
		puts "\tclean"
		puts "\tinstaller"
		puts "\tprerequisites"
		puts "\tmanifest"
		puts "\tpackages"
		puts ""
		puts "Example:"
		puts "\t./Make.rb -V installer"
		puts ""
		puts "#{File.basename(__FILE__)} is a script for creating installer packages"
	end
	
	def run_version
		puts "#{File.basename(__FILE__)} version #{VERSION}"
	end
	
	def parsed_options?		
		all_options.parse!(@arguments) rescue return false
		
		@options.verbose = false if @options.quiet
		
		true		
	end
	
	def all_options
		opts = OptionParser.new
		opts.banner = "Usage: #{File.basename(__FILE__)} [options] [command]"
		opts.on('-v', '--version', 'Show version') { run_version; exit 0 }
		opts.on('-h', '--help', 'Show this message') { run_usage; exit 0 }
		opts.on('-V', '--verbose', 'Run verbosely') { @options.verbose = true }	
		opts.on('-q', '--quiet', 'Run quietly') { @options.quiet = true }
		opts.on('-a', '--arch [ppc|x86|universal]', 'Use architecture. Default is universsal') { |arch| @options.arch = arch }
		opts.on('-n', '--no-crossplatform', 'Skip building cross-plaform binaries') { @options.nocp = true }
		opts.on('-b', '--binaries [DIR]', 'Use directory for the binaries') { |dir| @options.binaries = dir }
		opts.on('-l', '--prerequisites [DIR]', 'Use directory for the prerequisites') { |dir| @options.prerequisites = dir }
		opts.on('-p', '--packages [DIR]', 'Use directory for the packages') { |dir| @options.packages = dir }
		
		return opts
	end
	
	def all_packages
		pkgs = []
		
		pkgs << {
			:name => 'EstEIDCore',
			:files => [
				File.join(@options.prerequisites, 'lib', '*.dylib'),
				File.join(@options.prerequisites, 'include'),
				File.join(@options.prerequisites, 'etc', 'libdigidoc') ],
			:root => @options.prerequisites,
			:location => '/usr/local',
			:identifier => 'org.esteid.core',
			:version => '1.0' }
		
		pkgs << {
			:name => 'EstEIDSU',
			:files => File.join(@options.binaries, 'EstEIDSU.app'),
			:location => '/Applications/Utilities'
		}
		
		pkgs << {
			:name => 'EstEIDPP',
			:files => File.join(@options.binaries, 'EstEIDPP.prefPane'),
			:location => '/Library/PreferencePanes' }
		
		pkgs << {
			:name => 'EstEIDCM',
			:files => File.join(@options.binaries, 'EstEIDCM.plugin'),
			:location => '/Library/Contextual Menu Items' }
		
		
		pkgs << {
			:name => 'EstEIDWP',
			:files => File.join(@options.binaries, 'EstEIDWP.bundle'),
			:location => '/Library/Internet Plug-Ins'
		}
		
		pkgs << {
			:name => 'EstEIDInstaller',
			:files => File.join(@options.packages, '*.pkg'),
			:helpers => [ 'pkmksendae', 'pkmkpidforapp' ],
			:identifier => 'org.esteid.installer',
			#:restart => 'RequiredRestart',
			:version => '1.0' }
		
		return pkgs
	end
end

class HugeNum
	def initialize(value)
		@value = value
	end
	
	def to_plist_node
		return "<real>#{@value}</real>"
	end
end

application = Application.new(ARGV, STDIN)
application.run
