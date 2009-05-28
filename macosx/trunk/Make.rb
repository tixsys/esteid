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
		@options.force = false
		@options.verbose = false
		@options.quiet = false
		@options.name = 'Installer'
		@options.volname = nil
		@options.binaries = 'build/Release'
		@options.repository = 'build/Repository'
		@options.digidoc = '/usr/local'
		@options.qt = '/Library/Frameworks'
		@options.opensc = '/Library/OpenSC'
		@options.packages = 'build/Packages'
		@options.pkgapp = '/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS'
		@options.sign = nil
	end
	
	def run
		if parsed_options?
			begin
				if @arguments.last == 'digidoc'
					run_digidoc
				elsif @arguments.last == 'binaries'
					run_binaries
				elsif @arguments.last == 'clean'
					run_clean
				elsif @arguments.last == 'repository'
					run_repository
				elsif @arguments.last == 'packages'
					run_packages
				elsif @arguments.last == 'installer'
					run_clean
					run_digidoc
					run_binaries
					run_packages
					run_repository
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
	
	def run_digidoc
		puts "Creating digidoc..." if @options.verbose
		
		puts "Creating libdigidoc2..." if @options.verbose
		
		FileUtils.cd(Pathname.new(@path).join('../../libdigidoc2/tags/2.6.0.17').to_s) do
			
		end
		
		puts "Creating libdigidoc..."if @options.verbose
		
		FileUtils.cd(Pathname.new(@path).join('../../libdigidoc/cpp/branches/libdigidoc_ddoc').to_s) do
			cmd = `rm CMakeCache.txt`
			puts cmd if @options.verbose
			cmd = `rm -R CMakeFiles`
			puts cmd if @options.verbose
			cmd = `cmake -G "Unix Makefiles" -DOPENSSLCRYPTO_LIBRARY=/usr/local/lib/libcrypto.a -DOPENSSLCRYPTO_INCLUDE_DIR=/usr/local/include -DOPENSSL_LIBRARY=/usr/local/lib/libssl.a -DOPENSSL_INCLUDE_DIR=/usr/local/include/`
			puts cmd if @options.verbose
			cmd = `make clean`
			puts cmd if @options.verbose
			cmd = `make`
			puts cmd if @options.verbose
			cmd = `sudo make install`
			puts cmd if @options.verbose
		end
		
		puts "\n" if @options.verbose
	end
	
	def run_binaries
		puts "Creating installer binaries..." if @options.verbose
		
		# Build cross-platform Qt-based components
		puts "Creating qesteidutil..." if @options.verbose
		
		FileUtils.cd(Pathname.new(@path).join('../../qesteidutil/trunk').to_s) do
			cmd = `rm CMakeCache.txt`
			puts cmd if @options.verbose
			cmd = `rm -R CMakeFiles`
			puts cmd if @options.verbose
			cmd = `cmake -G "Unix Makefiles" -DOPENSSLCRYPTO_LIBRARY=/usr/local/lib/libcrypto.a -DOPENSSLCRYPTO_INCLUDE_DIR=/usr/local/include -DOPENSSL_LIBRARY=/usr/local/lib/libssl.a -DOPENSSL_INCLUDE_DIR=/usr/local/include/`
			puts cmd if @options.verbose
			cmd = `make clean`
			puts cmd if @options.verbose
			cmd = `make`
			puts cmd if @options.verbose
		end
		
		cmd = `Skeleton/Make.rb -V -i ../../../qesteidutil/trunk/qesteidutil`
		puts cmd if @options.verbose
		
		puts "Creating qdigidocclient..." if @options.verbose
		
		FileUtils.cd(Pathname.new(@path).join('../../qdigidocclient/trunk').to_s) do
			cmd = `rm CMakeCache.txt`
			puts cmd if @options.verbose
			cmd = `rm -R CMakeFiles`
			puts cmd if @options.verbose
			cmd = `cmake -G "Unix Makefiles" -DOPENSSLCRYPTO_LIBRARY=/usr/local/lib/libcrypto.a -DOPENSSLCRYPTO_INCLUDE_DIR=/usr/local/include -DOPENSSL_LIBRARY=/usr/local/lib/libssl.a -DOPENSSL_INCLUDE_DIR=/usr/local/include/`
			puts cmd if @options.verbose
			cmd = `make clean`
			puts cmd if @options.verbose
			cmd = `make`
			puts cmd if @options.verbose
		end
		
		cmd = `Skeleton/Make.rb -V -i ../../../qdigidocclient/trunk/qdigidocclient`
		puts cmd if @options.verbose
		
		puts "Creating qdigidoccrypto..." if @options.verbose
		
		FileUtils.cd(Pathname.new(@path).join('../../qdigidoccrypto/trunk').to_s) do
			cmd = `rm CMakeCache.txt`
			puts cmd if @options.verbose
			cmd = `rm -R CMakeFiles`
			puts cmd if @options.verbose
			cmd = `cmake -G "Unix Makefiles" -DOPENSSLCRYPTO_LIBRARY=/usr/local/lib/libcrypto.a -DOPENSSLCRYPTO_INCLUDE_DIR=/usr/local/include -DOPENSSL_LIBRARY=/usr/local/lib/libssl.a -DOPENSSL_INCLUDE_DIR=/usr/local/include/`
			puts cmd if @options.verbose
			cmd = `make clean`
			puts cmd if @options.verbose
			cmd = `make`
			puts cmd if @options.verbose
		end
		
		cmd = `Skeleton/Make.rb -V -i ../../../qdigidoccrypto/trunk/qdigidoccrypto`
		puts cmd if @options.verbose
		
		# Build all xcode targets
		puts "Building xcode projects..." if @options.verbose
		cmd = `xcodebuild -project Project.xcodeproj -configuration Release -target Main`
		puts cmd if @options.verbose
		
		puts "Building tokend..." if @options.verbose
		cmd = `xcodebuild -project tokend/Tokend.xcodeproj -configuration Deployment -target world`
		puts cmd if @options.verbose
		FileUtils.cp_r(File.join(@path, 'tokend', 'build', 'OpenSC.tokend'), File.join(@path, @options.binaries, 'OpenSC.tokend'))
		
		puts "\n" if @options.verbose
	end
	
	def run_clean
		puts "Cleaning installer..." if @options.verbose
		
		if File.exists? File.join(@path, @options.packages)
			puts "Cleaning #{File.join(@path, @options.packages)}" if @options.verbose
			FileUtils.rm_rf(File.join(@path, @options.packages))
		end
		
		if File.exists? File.join(@path, @options.binaries) and @options.force
			puts "Cleaning #{File.join(@path, @options.binaries)}" if @options.verbose
			FileUtils.rm_rf(File.join(@path, @options.binaries))
		end
		
		if File.exists? File.join(@path, @options.repository) and @options.force
			puts "Cleaning #{File.join(@path, @options.repository)}" if @options.verbose
			FileUtils.rm_rf(File.join(@path, @options.repository))
		end
		
		puts "\n" if @options.verbose
	end
	
	def run_repository
		puts "Creating installer repository..." if @options.verbose
		
		cpkgroot = Pathname.new(@path).join(@options.repository, 'Packages').to_s
		FileUtils.mkdir_p(cpkgroot) unless File.exists? cpkgroot
		
		puts "Creating Manifest.xml..." if @options.verbose
		
		manifest = Pathname.new(@path).join(@options.repository, 'Manifest.xml').to_s
		file = File.new(manifest, 'w')
		file.puts "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
		file.puts "<manifest#{(@options.sign.nil?) ? '' : ' signature=\'Manifest.sig\'' }>"
			
		components.each do |component|
			# Load component configuration
			version = component[:version]
			packages = component[:packages]
			priority = component[:priority]
			
			file.puts "\t<component#{(version.nil?) ? '' : ' version=\'' + version + '\''} priority=\"#{(priority.nil?) ? 1 : priority}\">"
			
			file.puts "\t\t<title xml:lang=\"en\">#{component[:title]}</title>"
			file.puts "\t\t<description xml:lang=\"en\"><![CDATA[#{component[:description]}]]></description>"
			
			packages.each do |package|
				pkgroot = Pathname.new(@path).join(@options.packages, package + ".pkg").to_s
				
				if File.exists? pkgroot
					plist = Plist.parse_xml(File.join(pkgroot, 'Contents', 'Info.plist'))
					identifier = plist['CFBundleIdentifier'] 
					version = plist['CFBundleShortVersionString']
					restart = plist['IFPkgFlagRestartAction'] == 'RequiredRestart'
					name = identifier + '.' + version + ((@options.arch == 'universal') ? '' : '.' + @options.arch) + '.zip'
					path = File.join(cpkgroot, name)
					
					unless File.exists? path and !@options.force
						puts "Compressing package #{identifier}..." if @options.verbose
						`ditto -c -k -X #{pkgroot} #{path}`
					end
					
					sha1 = `openssl dgst -sha1 #{path}`
					puts sha1 if @options.verbose
					
					file.puts "\t\t<package id=\"#{identifier}\" version=\"#{version}\" length=\"#{File.stat(path).size}\" sha1=\"#{sha1[-41,40]}\"#{(restart) ? ' restart=\'1\'' : ''}>Packages/#{name}</package>"
				else
					puts "No package #{package} was found. Stopped." if !@options.quiet
					raise 'No package'
				end
			end
			
			file.puts "\t</component>"
		end
		
		file.puts "</manifest>"
		file.close
		
		unless @options.sign.nil?
			puts "Signing Manifest.xml..." if @options.verbose
			
			signature = Pathname.new(@path).join(@options.repository, 'Manifest.sig').to_s
			key = Pathname.new(@path).join(@options.sign).to_s
			FileUtils.rm(signature) if File.exists? signature
			
			`openssl dgst -sha1 -binary < #{manifest} | openssl dgst -dss1 -sign #{key} | openssl enc >> #{signature}`
		end
		
		puts "\n" if @options.verbose
	end
	
	def run_packages
		puts "Creating installer packages..." if @options.verbose
		
		if @tmp.nil?
			@tmp = File.join(Dir.tmpdir, 'esteidmake')
			FileUtils.rm_rf(@tmp) if File.exists? @tmp
			Dir.mkdir(@tmp)
		end
		
		packages.each do |package|
			# Load package configuration
			name = package[:name]
			identifier = package[:identifier]
			version = package[:version]
			location = package[:location]
			patterns = package[:files]
			restart = package[:restart]
			root = Pathname.new(@path).join(package[:froot]).to_s unless package[:froot].nil?
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
				puts "No files match pattern '#{package[:files]}' for package #{name + extension}. Stopped." if !@options.quiet
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
			pkgroot = File.join(@tmp, name + extension)
			Dir.mkdir(pkgroot)
			contents = File.join(pkgroot, 'Contents')
			Dir.mkdir(contents)
			
			# Normal package
			if extension == '.pkg'
				# Copy files to pax archive root directory
				archive = File.join(@tmp, 'Archive')
				FileUtils.rm_rf(archive) if File.exists? archive
				Dir.mkdir(archive)
				
				files.each do |file|
					puts "Archiving file #{file} for package #{name + extension}" if @options.verbose
					
					dir = (root.nil?) ? archive : File.join(archive, File.dirname(file[root.length, file.length - 1]))	
					FileUtils.mkdir_p dir unless File.exists? dir
					
					if @options.arch != 'universal' and (File.extname(file) == '.dylib' or File.extname(file) == '.so')
						puts "Thinning #{File.basename(file)}..." if @options.verbose;
						cmd = `ditto -arch #{@options.arch} #{file} #{File.join(dir, File.basename(file))}`
						puts cmd if @options.verbose
					elsif @options.arch != 'universal' and File.exists? File.join(file, 'Contents/MacOS')
						FileUtils.cp_r(file, dir)
						
						exe_in = File.join(file, 'Contents/MacOS', File.basename(file, File.extname(file)))
						exe_out = File.join(dir, File.basename(file), 'Contents/MacOS', File.basename(file, File.extname(file)))
						
						if File.exists? exe_in
							puts "Thinning #{File.basename(file)}..." if @options.verbose;
							FileUtils.rm_rf(exe_out) if File.exists? exe_out
							cmd = `ditto -arch #{@options.arch} #{exe_in} #{exe_out}`
							puts cmd if @options.verbose
						end	
					elsif File.exists? File.join(file, File.basename(file, '.framework'))
						FileUtils.cp_r(file, dir)
						
						exe_in = Pathname.new(@path).join(file, File.readlink(File.join(file, File.basename(file, '.framework')))).to_s
						exe_out = Pathname.new(dir).join(File.basename(file), File.readlink(File.join(file, File.basename(file, '.framework')))).to_s
						
						if File.exists? exe_in
							puts "Thinning #{File.basename(file)}..." if @options.verbose;
							FileUtils.rm_rf(exe_out) if File.exists? exe_out
							cmd = `ditto -arch #{@options.arch} #{exe_in} #{exe_out}`
							puts cmd if @options.verbose
						end	
					else 
						FileUtils.cp_r(file, dir) unless File.exists? File.join(dir, File.basename(file))
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
				subpackages = File.join(contents, 'Packages')
				Dir.mkdir(subpackages)
				
				outlines = ""
				choices = ""
				
				# Copy packages
				files.each do |file|
					puts "Copying #{file} to package #{name + extension}" if @options.verbose
					
					FileUtils.cp_r(file, subpackages)
					
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
			helpers = package[:helpers]
			
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
			dstname = (identifier == 'org.esteid.installer' && !@options.name.nil?) ? @options.name : name
			dst = Pathname.new(@path).join(@options.packages, dstname + extension).to_s
			FileUtils.mkdir_p(File.dirname(dst)) unless File.exists? File.dirname(dst)
			FileUtils.rm_rf(dst) if File.exists? dst
			FileUtils.cp_r(pkgroot, dst)
			
			# Hide the file extension
			`setfile -a E #{dst}`
			
			# Finally create .dmg and .tar.gz files
			if extension == '.mpkg'
				FileUtils.cd(Pathname.new(@path).join(@options.packages).to_s) do
					dmg = dstname + '.dmg'
					
					`tar -czf #{dstname + '.tar.gz'} #{dstname + extension}`
					`rm #{dmg}` if File.exists? dmg
					`hdiutil create -fs HFS+ -srcfolder "#{dstname + extension}" -volname "#{(@options.volname.nil?) ? dstname : @options.volname}" #{dmg}`
					`setfile -a E #{dmg}`
				end
			end
		end
		
		puts "\n" if @options.verbose
	end
	
	def run_usage
		puts options.to_s
		puts ""
		puts "Available commands:"
		puts "\tdigidoc"
		puts "\tbinaries"
		puts "\tclean"
		puts "\tcomponents"
		puts "\tinstaller"
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
		options.parse!(@arguments) rescue return false
		
		@options.verbose = false if @options.quiet
		
		true		
	end
	
	def options
		opts = OptionParser.new
		opts.banner = "Usage: #{File.basename(__FILE__)} [options] [command]"
		opts.on('-v', '--version', 'Show version') { run_version; exit 0 }
		opts.on('-h', '--help', 'Show this message') { run_usage; exit 0 }
		opts.on('-V', '--verbose', 'Run verbosely') { @options.verbose = true }	
		opts.on('-q', '--quiet', 'Run quietly') { @options.quiet = true }
		opts.on('-f', '--force', 'Perform additional actions that are normally skipped') { @options.force = true }
		opts.on('-a', '--arch [ppc|x86|universal]', 'Use architecture. The default is universsal') { |arch| @options.arch = arch }
		opts.on('-n', '--name [NAME]', 'File name for the installer') { |name| @options.name = name }
		opts.on('-N', '--volname [VOLNAME]', 'Volume name for the installer') { |volname| @options.volname = volname }
		opts.on('-b', '--binaries [DIR]', 'Use directory for the binaries') { |dir| @options.binaries = dir }
		opts.on('-c', '--repository [DIR]', 'Use directory for the repository') { |dir| @options.repository = dir }
		opts.on('-d', '--digidoc [DIR]', 'Look for libdigidoc stuff at this path') { |dir| @options.digidoc = dir }
		opts.on('-q', '--qt [DIR]', 'Look for Qt frameworks at this path') { |dir| @options.qt = dir }
		opts.on('-o', '--opensc [DIR]', 'Look for OpenSC files at this path') { |dir| @options.opensc = dir }
		opts.on('-p', '--packages [DIR]', 'Use directory for the packages') { |dir| @options.packages = dir }
		opts.on('-s', '--sign [KEY]', 'Use key for signing the components') { |key| @options.sign = key }
		
		return opts
	end
	
	def components
		return [
			{
				:title => 'Core components',
				:description => 'Core components description',
				:version => '1.0',
				:priority => 2,
				:packages => [ 'esteid-digidoc',
							   'esteid-opensc',
							   'esteid-qt',
							   'esteid-updater',
							   'esteid-preferences' ]
			}, {
				:title => 'Internet Plug-in',
				:description => 'Internet Plug-in description',
				:priority => 1,
				:packages => [ 'esteid-webplugin', 'esteid-tokend' ]
			}, {
				:title => 'Finder Plug-in',
				:description => 'Finder Plug-in description',
				:priority => 1,
				:packages => [ 'esteid-contextmenu' ]
			}, {
				:title => 'ID-card Utility',
				:description => 'ID-card Utility description',
				:priority => 1,
				:packages => [ 'esteid-qesteidutil' ]
			}, {
				:title => 'ID-card DigiDoc',
				:description => 'ID-card DigiDoc description',
				:priority => 1,
				:packages => [ 'esteid-qdigidocclient' ]
			}, {
				:title => 'ID-card Crypto',
				:description => 'ID-card Crypto description',
				:priority => 1,
				:packages => [ 'esteid-qdigidoccrypto' ]
			} ]
	end
	
	def packages
		return [
			{
				:name => 'esteid-digidoc',
				:files => [
					File.join(@options.digidoc, 'lib/libp11*.dylib'),
					File.join(@options.digidoc, 'lib/libxerces-c*.dylib'),
					File.join(@options.digidoc, 'lib/libxml-security-c*.dylib'),
					File.join(@options.digidoc, 'lib/libdigidoc*.dylib'),
					File.join(@options.digidoc, 'etc/libdigidoc') ],
				:froot => '/',
				:location => '/',
				:identifier => 'org.esteid.digidoc',
				:version => '1.0'
			}, {
				:name => 'esteid-opensc',
				:files => [ File.join(@options.opensc, '*/**') ],
				:location => '/Library/OpenSC',
				:identifier => 'org.esteid.opensc',
				:version => '1.0'
			}, {
				:name => 'esteid-qt',
				:files => [
					File.join(@options.qt, 'QtCore.framework'),
					File.join(@options.qt, 'QtGui.framework'),
					File.join(@options.qt, 'QtNetwork.framework'),
					File.join(@options.qt, 'QtWebKit.framework') ],
				:location => '/Library/Frameworks',
				:identifier => 'org.esteid.qt',
				:version => '1.0'
			}, {
				:name => 'esteid-updater',
				:files => File.join(@options.binaries, 'EstEIDSU.app'),
				:location => '/Applications/Utilities'
			}, {
				:name => 'esteid-preferences',
				:files => File.join(@options.binaries, 'EstEIDPP.prefPane'),
				:location => '/Library/PreferencePanes'
			}, {
				:name => 'esteid-qesteidutil',
				:files => File.join(@options.binaries, 'qesteidutil.app'),
				:location => '/Applications'
			}, {
				:name => 'esteid-qdigidocclient',
				:files => File.join(@options.binaries, 'qdigidocclient.app'),
				:location => '/Applications'
			}, {
				:name => 'esteid-qdigidoccrypto',
				:files => File.join(@options.binaries, 'qdigidoccrypto.app'),
				:location => '/Applications'
			}, {
				:name => 'esteid-contextmenu',
				:files => File.join(@options.binaries, 'EstEIDCM.plugin'),
				:location => '/Library/Contextual Menu Items'
			}, {
				:name => 'esteid-webplugin',
				:files => File.join(@options.binaries, 'EstEIDWP.bundle'),
				:location => '/Library/Internet Plug-Ins'
			}, {
				:name => 'esteid-tokend',
				:files => File.join(@options.binaries, 'OpenSC.tokend'),
				:location => '/System/Library/Security/tokend'
			}, {
				:name => 'esteid',
				:files => File.join(@options.packages, '*.pkg'),
				:helpers => [ 'pkmksendae', 'pkmkpidforapp' ],
				:identifier => 'org.esteid.installer',
				#:restart => 'RequiredRestart',
				:version => '1.0'
			} ]
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
