#!/usr/bin/ruby

# Copyright (c) 2009, 2010 Janek Priimann

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
		@options.verbose = true
		@options.quiet = false
		@options.name = 'ID-Installer'
		@options.volname = nil
		@options.build = 'build'
		@options.target = 'Release'
		@options.firebreath = 'build/firebreath'
		@options.binaries = 'build/Release'
		@options.repository = 'build/Repository'
		@options.digidoc = 'build/Digidoc'
		@options.qt = 'build/Qt'
		@options.opensc = 'build/OpenSC'
		@options.drivers = 'build/Drivers'
		@options.packages = 'build/Packages'
		@options.pkgapp = '/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS'
		@options.mozappid = '{aa84ce40-4253-11da-8cd6-0800200c9a66}'
		@options.setsdkenv = '~/OpenOffice.org3.2_SDK/' + `hostname`.strip + '/setsdkenv_unix.sh'
		@options.sign = nil
	end
	
	def run
		if parsed_options?
			begin
				if @arguments.last == 'digidoc'
					run_digidoc
				elsif @arguments.last == 'opensc'
					run_opensc
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
					run_opensc
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
	
	def run_cmake_build(source_dir, cmake_extra_args = '', skip_install = false)
		puts "run_cmake_build: #{source_dir}, #{cmake_extra_args}"
		
		FileUtils.cd(Pathname.new(@path).join("#{source_dir}").to_s) do
			FileUtils.rm_rf('build') if File.exists? 'build'
			FileUtils.mkdir_p('build')
			FileUtils.cd('build') do
				run_command "cmake -DCMAKE_OSX_ARCHITECTURES='i386;x86_64' -DCMAKE_OSX_SYSROOT=/Developer/SDKs/MacOSX10.5.sdk -DCMAKE_OSX_DEPLOYMENT_TARGET=10.5 -DCMAKE-BUILD-TYPE=#{@options.target} -DOPENSSLCRYPTO_LIBRARY=/usr/local/lib/libcrypto.dylib -DOPENSSLCRYPTO_INCLUDE_DIR=/usr/local/include -DOPENSSL_LIBRARIES=/usr/local/lib/libssl.dylib -DOPENSSL_INCLUDE_DIR=/usr/local/include/ #{cmake_extra_args} .."
				run_command "make -j2 VERBOSE=1"
				unless skip_install
					run_command "sudo make install"
				end
			end
		end
	end

	def run_opensc
		FileUtils.cd(Pathname.new(@path).join(@options.build).to_s) do
			_run_opensc "10.5"
			_run_opensc "10.6"
		end
	end

	def _run_opensc(osx_target)
		puts "Creating opensc..." if @options.verbose

		stagedir = Pathname.new(@path).join(@options.opensc + "." + osx_target).to_s
		builddir = 'opensc-build-' + osx_target
		prefix = '/Library/OpenSC'

		# Get latest tarball
		latest_tarball = `ssh idkopeerija@pontu.smartlink ls -1 "/home/ftp/pub/id/nightly/snapshots/opensc-0.12.0-svn-r*.tar.gz" | tail -n 1 | xargs basename`.strip
		run_command "scp \"idkopeerija@pontu.smartlink:/home/ftp/pub/id/nightly/snapshots/#{latest_tarball}\" ." unless File.exists? latest_tarball
		opensc_rev = `echo "#{latest_tarball}" | sed "s/.*opensc-0\.12\.0-svn-r\(.*\)\.tar\.gz/\1/"`.strip

		run_command 'rm -rf ' + stagedir
		run_command 'sudo rm -rf ' + builddir
		run_command 'rm -rf opensc-0.12.0'

		run_command "tar -xzf " + latest_tarball
		run_command "mv opensc-0.12.0 " + builddir

		FileUtils.cd(builddir) do
			if osx_target == "10.5"
				cflags = "-isysroot /Developer/SDKs/MacOSX10.5.sdk -arch i386 -arch ppc7400 -mmacosx-version-min=10.5 -O2"
				ltlib_libs = "/Developer/SDKs/MacOSX10.5.sdk/usr/lib/libltdl.a"
			elsif osx_target == "10.6"
				cflags = "-isysroot /Developer/SDKs/MacOSX10.6.sdk -arch i386 -arch x86_64 -mmacosx-version-min=10.6 -O2"
				ltlib_libs = "/Developer/SDKs/MacOSX10.6.sdk/usr/lib/libltdl.a"
			end

			run_command "CFLAGS=\"#{cflags}\" LTLIB_LIBS=\"#{ltlib_libs}\" PKG_CONFIG_PATH=/usr/local/lib/pkgconfig ./configure --prefix=#{prefix} --sysconfdir=#{prefix}/etc --disable-dependency-tracking --disable-doc --enable-man --enable-shared --disable-static --enable-iconv --enable-strict --disable-assert"
			run_command "make -j2"

			run_command "make install DESTDIR=" + stagedir
			run_command "rm " + stagedir + prefix + "/lib/*.la"

			run_command "sudo make install"
			run_command "sudo rm " + prefix + "/lib/*.la"

			# An ugly hack to help people who have broken CCID
			# driver that is linked against wrong libusb path
			run_command "cp " + @options.drivers + "/ifd-ccid.bundle/Contents/MacOS/libusb-0.1.4.dylib " + prefix + "/lib/"

			_run_tokend osx_target
		end
		
		puts "\n" if @options.verbose
	end

	def _run_tokend(osx_target)
		puts "Creating tokend..." if @options.verbose

		stagedir = Pathname.new(@path).join(@options.binaries + "/" + osx_target).to_s
		git = "/usr/local/git/bin/git"

		run_command git + " clone git://github.com/martinpaljak/OpenSC.tokend.git"
		run_command git + " --git-dir OpenSC.tokend/.git --work-tree OpenSC.tokend checkout origin/" + osx_target
		run_command "rm -rf OpenSC.tokend/build"

		run_command "curl http://martinpaljak.net/download/build-" + osx_target + ".tar.gz -o build-" + osx_target + ".tar.gz"

		# Unpack the binary building components
		run_command "tar -C OpenSC.tokend -xzvf build-" + osx_target + ".tar.gz"

		# Create the symlink to OpenSC sources
		run_command "ln -sf `pwd`/src OpenSC.tokend/build/opensc-src"

		# build and copy OpenSC.tokend
		run_command "xcodebuild -configuration Deployment -project OpenSC.tokend/Tokend.xcodeproj"
		run_command "rm -rf " + stagedir
		run_command "mkdir -p " + stagedir
		run_command "mv OpenSC.tokend/build/OpenSC.tokend " + stagedir + "/OpenSC.tokend"
	end

	def run_digidoc
		puts "Creating libdigidoc..." if @options.verbose
		
		stagedir = Pathname.new(@path).join(@options.digidoc).to_s
		etcdir = File.join(stagedir, 'etc')
		libdir = File.join(stagedir, 'lib')
		sharedir = File.join(stagedir, 'share')
		digidoc2 = File.join(libdir, 'libdigidoc.2.dylib')
		digidoc = File.join(libdir, 'libdigidocpp.0.dylib')
		libp11 = File.join(libdir, 'libp11.1.dylib')
		libcrypto = File.join(libdir, 'libcrypto.1.0.0.dylib')
		libssl = File.join(libdir, 'libssl.1.0.0.dylib')
		
		FileUtils.rm_rf(stagedir) if File.exists? stagedir
		FileUtils.mkdir_p(etcdir)
		FileUtils.mkdir_p(libdir)
		FileUtils.mkdir_p(sharedir)

		puts "Copying file libp11.dylib" if @options.verbose
		FileUtils.cp('/usr/local/lib/libp11.dylib', libp11)

		puts "Copying openssl" if @options.verbose
		FileUtils.cp('/usr/local/lib/libcrypto.1.0.0.dylib', libcrypto)
		FileUtils.cp('/usr/local/lib/libssl.1.0.0.dylib', libssl)
		
		puts "Creating libdigidoc..." if @options.verbose
		FileUtils.cd(Pathname.new(@path).join('../../libdigidoc/trunk').to_s) do	
			run_cmake_build('../../libdigidoc/trunk')

			puts "Copying file libdigidoc.dylib" if @options.verbose
			FileUtils.cp_r("/usr/local/lib/libdigidoc.dylib", digidoc2)
			
			FileUtils.cp('/usr/local/etc/digidoc.conf', etcdir)
			FileUtils.cp_r('/usr/local/share/libdigidoc/', sharedir)
		end
		
		puts "Creating libdigidocpp..." if @options.verbose
		FileUtils.cd(Pathname.new(@path).join('../../libdigidocpp/trunk').to_s) do
			run_cmake_build('../../libdigidocpp/trunk', '-DENABLE_BINDINGS=NO')

			puts "Copying file libdigidocpp.dylib" if @options.verbose
			FileUtils.cp_r("/usr/local/lib/libdigidocpp.dylib", digidoc)
			
			puts "Copying digidocpp config dir" if @options.verbose
			FileUtils.cp_r('/usr/local/etc/digidocpp/', etcdir)
		end
		
		puts "\n" if @options.verbose
	end
	
	def run_binaries
		puts "Creating installer binaries..." if @options.verbose
		
		binaries = Pathname.new(@path).join(@options.binaries).to_s
		
		# Cleanup
		FileUtils.mkdir_p(binaries)
		FileUtils.mkdir_p(File.join(binaries, '10.5'))
		FileUtils.mkdir_p(File.join(binaries, '10.6'))
		
		puts "Creating smartcardpp..." if @options.verbose
		FileUtils.cd(Pathname.new(@path).join('../../smartcardpp/trunk').to_s) do
			run_cmake_build('../../smartcardpp/trunk')
		end
		
		puts "Creating browser plugin..." if @options.verbose
		FileUtils.cd(Pathname.new(@path).join(@options.build).to_s) do
			FileUtils.rm_rf('firebreath') if File.exists? 'firebreath'
			run_command '/opt/local/bin/hg clone -r a85df5ce249e https://firebreath.googlecode.com/hg/ firebreath'
			FileUtils.cd('firebreath') do
				FileUtils.mkdir_p('projects')
				FileUtils.cd('projects') do
					FileUtils.cp_r('/Users/kalev/nightly_build/google-svn/esteid-browser-plugin/trunk', 'esteid-browser-plugin')
				end

				run_command './prepmac.sh projects/ build/ -DWITH_SYSTEM_BOOST=YES -DCMAKE_OSX_SYSROOT=/Developer/SDKs/MacOSX10.5.sdk -DCMAKE_OSX_ARCHITECTURES="i386;x86_64"'
				FileUtils.cd('build') do
					run_command "cmake --build . --config #{@options.target}"

					# Copy the resulting plugin
					proot = Pathname.new(@path).join('projects', 'esteid-browser-plugin', @options.target, 'esteid.plugin').to_s
					FileUtils.cp_r("projects/esteid-browser-plugin/#{@options.target}/esteid.plugin", File.join(binaries, 'esteid.plugin'))

					run_command "ditto -xk *.xpi #{@options.mozappid}"
			
					puts "Copying Mozilla extension..." if @options.verbose
					FileUtils.cp_r(@options.mozappid, binaries)
				end
			end
		end
		
		# Build cross-platform Qt-based components
		puts "Creating qesteidutil..." if @options.verbose
		FileUtils.cd(Pathname.new(@path).join('../../qesteidutil/trunk').to_s) do
			run_cmake_build('../../qesteidutil/trunk', "-DCMAKE_OSX_ARCHITECTURES='i386'", true)
			
			puts "Copying qesteidutil.app..." if @options.verbose
			FileUtils.cp_r("build/qesteidutil.app", binaries)
		end
		
		puts "Creating qdigidoc..." if @options.verbose
		FileUtils.cd(Pathname.new(@path).join('../../qdigidoc/trunk').to_s) do
			run_cmake_build('../../qdigidoc/trunk', "-DCMAKE_OSX_ARCHITECTURES='i386' -DENABLE_KDE=NO -DENABLE_NAUTILUS_EXTENSION=NO", true)
			
			puts "Copying qdigidocclient.app..." if @options.verbose
			FileUtils.cp_r("build/client/qdigidocclient.app", binaries)
			
			puts "Copying qdigidoccrypto.app..." if @options.verbose
			FileUtils.cp_r("build/crypto/qdigidoccrypto.app", binaries)
		end
		
		project_build = Pathname.new(@path).join(@options.build, 'Project.build').to_s
		FileUtils.rm_rf(project_build) if File.exists? project_build
		
		# Build all xcode targets
		puts "Building xcode projects..." if @options.verbose
		run_command "xcodebuild -project Project.xcodeproj -configuration Release -target Main"
		
		puts "\n" if @options.verbose
	end
	
	def run_clean
		puts "Cleaning installer..." if @options.verbose
		
		if File.exists? File.join(@path, @options.packages)
			puts "Cleaning #{File.join(@path, @options.packages)}" if @options.verbose
			FileUtils.rm_rf(File.join(@path, @options.packages))
		end
		
		if File.exists? File.join(@path, @options.binaries)
			puts "Cleaning #{File.join(@path, @options.binaries)}" if @options.verbose
			FileUtils.rm_rf(File.join(@path, @options.binaries))
		end
		
		if File.exists? File.join(@path, @options.repository)
			puts "Cleaning #{File.join(@path, @options.repository)}" if @options.verbose
			FileUtils.rm_rf(File.join(@path, @options.repository))
		end
		
		puts "\n" if @options.verbose
	end
	
	def run_repository
		puts "Creating installer repository..." if @options.verbose
		
		pkgs = packages
		
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
					
					unless File.exists? path
						puts "Compressing package #{identifier}..." if @options.verbose
						run_command "ditto -c -k -X #{pkgroot} #{path}"
					end
					
					sha1 = `openssl dgst -sha1 #{path}`
					puts sha1 if @options.verbose
					
					opsystem = nil
					
					pkgs.each do |pkg|
						if pkg[:system] != nil and pkg[:identifier] == identifier
							opsystem = pkg[:system]
							break;
						end
					end
					
					file.puts "\t\t<package id=\"#{identifier}\" version=\"#{version}\" length=\"#{File.stat(path).size}\" sha1=\"#{sha1[-41,40]}\"#{(restart) ? ' restart=\'1\'' : ''}#{(opsystem.nil?) ? '' : ' macos=\'' + opsystem + '\'' }>Packages/#{name}</package>"
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
			
			run_command "openssl dgst -sha1 -binary < #{manifest} | openssl dgst -dss1 -sign #{key} | openssl enc > #{signature}"
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
		
		pkgs = packages
		
		pkgs.each do |package|
			# Load package configuration
			name = package[:name]
			identifier = package[:identifier]
			version = package[:version]
			svnroot = package[:svnroot]
			location = package[:location]
			patterns = package[:files]
			restart = package[:restart]
			root = Pathname.new(@path).join(package[:froot]).to_s unless package[:froot].nil?
			extension = '.mpkg'
			files = []
			
			if !version.nil? and !svnroot.nil?
				FileUtils.cd(Pathname.new(@path).join('../..' + svnroot).to_s) do
					svnversion = `svn info | grep 'Last Changed Rev' | cut -d ' ' -f 4`.strip
					version = version.gsub("$", svnversion)
				end
			end
			
			if patterns.instance_of? Array
				patterns.each { |pattern| files.concat(Dir.glob(Pathname.new(@path).join(pattern).to_s)) }
			else
				if File.exists? Pathname.new(@path).join(patterns).to_s
				puts patterns
					files.push(Pathname.new(@path).join(patterns).to_s)
				else
					files.concat(Dir.glob(Pathname.new(@path).join(patterns).to_s))
				end
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
				
				file = Pathname.new(files.first).join('install.rdf').to_s
				
				if File.exists? file
					version = `grep -s "<em:version>" #{file} | tr -s " " | cut -c 14-21`.strip
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
						run_command "ditto -arch #{@options.arch} #{file} #{File.join(dir, File.basename(file))}"
					elsif @options.arch != 'universal' and File.exists? File.join(file, 'Contents/MacOS')
						FileUtils.cp_r(file, dir)
						
						exe_in = File.join(file, 'Contents/MacOS', File.basename(file, File.extname(file)))
						exe_out = File.join(dir, File.basename(file), 'Contents/MacOS', File.basename(file, File.extname(file)))
						
						if File.exists? exe_in
							puts "Thinning #{File.basename(file)}..." if @options.verbose;
							FileUtils.rm_rf(exe_out) if File.exists? exe_out
							run_command "ditto -arch #{@options.arch} #{exe_in} #{exe_out}"
						end	
					elsif @options.arch != 'universal' and File.exists? File.join(file, File.basename(file, '.framework'))
						FileUtils.cp_r(file, dir)
						
						exe_in = Pathname.new(@path).join(file, File.readlink(File.join(file, File.basename(file, '.framework')))).to_s
						exe_out = Pathname.new(dir).join(File.basename(file), File.readlink(File.join(file, File.basename(file, '.framework')))).to_s
						
						if File.exists? exe_in
							puts "Thinning #{File.basename(file)}..." if @options.verbose;
							FileUtils.rm_rf(exe_out) if File.exists? exe_out
							run_command "ditto -arch #{@options.arch} #{exe_in} #{exe_out}"
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
				script = ""
				scounter = 0
				
				# Copy packages
				files.each do |file|
					puts "Copying #{file} to package #{name + extension}" if @options.verbose
					
					FileUtils.cp_r(file, subpackages)
					
					plist = Plist.parse_xml(File.join(file, 'Contents', 'Info.plist'))
					pidentifier = plist['CFBundleIdentifier']
					pversion = plist['CFBundleShortVersionString']
					psize = plist['IFPkgFlagInstalledSize']
					installCheck = nil
					
					pkgs.each do |pkg|
						if pkg[:script] != nil and pkg[:identifier] == pidentifier
							installCheck = "InstallCheck#{scounter}()"
							script += "function " + installCheck + " {" + pkg[:script] + "}\n"
							scounter += 1
							break;
						end
					end
					
					outlines += "<line choice=\"#{pidentifier}.choice\" />\n"
					choices += "<choice id=\"#{pidentifier}.choice\" "
					choices += "selected=\"#{installCheck}\" enabled=\"#{installCheck}\" " unless installCheck.nil?
					choices += ">\n"
					choices += "<pkg-ref id=\"#{pidentifier}\" />\n"
					choices += "</choice>\n"
					choices += "<pkg-ref id=\"#{pidentifier}\" version=\"#{pversion}\" installKBytes=\"#{psize}\" auth=\"Root\">file:./Contents/Packages/#{File.basename(file)}</pkg-ref>\n"
				end
				
				# Create the distribution.dist file
				file = File.new(File.join(contents, 'distribution.dist'), 'w')
				file.puts "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
				file.puts "<installer-script minSpecVersion=\"1.000000\" authoringTool=\"org.esteid.make\" authoringToolVersion=\"#{VERSION}\" authoringToolBuild=\"1\">"
				file.puts "<title>#{package[:title]}</title>" unless package[:title].nil?
				file.puts "<options customize=\"never\" allow-external-scripts=\"no\" rootVolumeOnly=\"true\" />"
				# FIXME: Uncomment to build complete installer
				#file.puts "<background file=\"Background.tiff\" alignment=\"center\" scaling=\"none\" />"
				#file.puts "<welcome file=\"Welcome.html\" />"
				#file.puts "<readme file=\"ReadMe.html\" />"
				#file.puts "<license file=\"License.html\" />"
				#file.puts "<conclusion file=\"Conclusion.html\" />"
				
				if script.length > 0
					file.puts "<script>"
					file.puts script
					file.puts "</script>"
				end
				
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
			dstname = (identifier == 'org.esteid.installer' and !@options.name.nil?) ? @options.name : identifier
			dstname = package[:dstname] unless package[:dstname].nil?
			dst = Pathname.new(@path).join(@options.packages, dstname + extension).to_s
			FileUtils.mkdir_p(File.dirname(dst)) unless File.exists? File.dirname(dst)
			FileUtils.rm_rf(dst) if File.exists? dst
			FileUtils.cp_r(pkgroot, dst)
			
			# Hide the file extension
			run_command "setfile -a E #{dst}"
			
			# Finally create .dmg and .tar.gz files
			if identifier == 'org.esteid.installer'
				FileUtils.cd(Pathname.new(@path).join(@options.packages).to_s) do
					dmg = dstname + '.dmg'
					
					#`tar -czf #{dstname + '.tar.gz'} #{dstname + extension}`
					FileUtils.rm_rf(dmg) if File.exists? dmg
					run_command "hdiutil create -fs HFS+ -srcfolder \"#{dstname + extension}\" -volname \"#{(@options.volname.nil?) ? dstname : @options.volname}\" #{dmg}"
					run_command "hdiutil internet-enable -yes #{dmg}"
					run_command "setfile -a E #{dmg}"
				end
			end
			
			FileUtils.rm_rf(pkgroot)
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
		puts "\trepository"
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
	
	def run_command(cmd)
		puts "run_command: #{cmd}" if @options.verbose
		out = `#{cmd}`
		puts out if @options.verbose
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
		opts.on('-a', '--arch [ppc|x86|universal]', 'Use architecture. The default is universsal') { |arch| @options.arch = arch }
		opts.on('-n', '--name [NAME]', 'File name for the installer') { |name| @options.name = name }
		opts.on('-N', '--volname [VOLNAME]', 'Volume name for the installer') { |volname| @options.volname = volname }
		opts.on('-b', '--binaries [DIR]', 'Use directory for the binaries') { |dir| @options.binaries = dir }
		opts.on('-c', '--repository [DIR]', 'Use directory for the repository') { |dir| @options.repository = dir }
		opts.on('-d', '--digidoc [DIR]', 'Look for libdigidoc stuff at this path') { |dir| @options.digidoc = dir }
		opts.on('-D', '--debug', 'Build in debug mode') { |dir| @options.target = 'Debug' }
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
				:packages => [ 'org.esteid.installer.digidoc',
							   'org.esteid.installer.opensc.10.6',
							   'org.esteid.installer.opensc',
							   'org.esteid.installer.qt',
							   'org.esteid.installer.su',
							   'org.esteid.installer.pp',
							   'org.esteid.installer.tokend.10.6',
							   'org.esteid.installer.tokend.10.5',
							   'org.esteid.installer.drivers.10.5' ]
			}, {
				:title => 'Internet Plug-ins',
				:description => 'Internet Plug-in description',
				:priority => 1,
				:packages => [ 'org.esteid.webplugin', 'org.esteid.installer.mozilla' ]
			}, {
				:title => 'Finder Plug-in',
				:description => 'Finder Plug-in description',
				:priority => 1,
				:packages => [ 'org.esteid.installer.cmi' ]
			}, {
				:title => 'ID-card Utility',
				:description => 'ID-card Utility description',
				:priority => 1,
				:packages => [ 'org.esteid.qesteidutil' ]
			}, {
				:title => 'DigiDoc3 Client',
				:description => 'DigiDoc3 Client description',
				:priority => 1,
				:packages => [ 'org.esteid.qdigidocclient' ]
			}, {
				:title => 'Digidoc3 Crypto',
				:description => 'Digidoc3 Crypto description',
				:priority => 1,
				:packages => [ 'org.esteid.qdigidoccrypto' ]
			} ]
	end
	
	def packages
		return [
			{
				:name => 'esteid-opensc',
				:files => [ File.join(@options.opensc + '.10.6', '*/**') ],
				:froot => File.join(@options.opensc + '.10.6'),
				:location => '/',
				:identifier => 'org.esteid.installer.opensc.10.6',
				:version => '1.0.5',
				:script => "return system.version.ProductVersion >= '10.6';",
				:system => '10.6>='
			}, {
				:name => 'esteid-opensc',
				:files => [ File.join(@options.opensc + '.10.5', '*/**') ],
				:froot => File.join(@options.opensc + '.10.5'),
				:location => '/',
				:identifier => 'org.esteid.installer.opensc',
				:version => '1.0.5',
				:script => "return system.version.ProductVersion &lt; '10.6';",
				:system => '10.6&lt;'
			}, {
				:name => 'esteid-digidoc',
				:files => [ File.join(@options.digidoc, '*/**') ],
				:froot => @options.digidoc,
				:location => '/usr/local',
				:identifier => 'org.esteid.installer.digidoc',
				:version => '1.0.$',
				:svnroot => '/libdigidocpp/trunk'
			}, {
				:name => 'esteid-qt',
				:files => [
					File.join(@options.qt, 'QtDBus.framework'),
					File.join(@options.qt, 'QtCore.framework'),
					File.join(@options.qt, 'QtGui.framework'),
					File.join(@options.qt, 'QtNetwork.framework'),
					File.join(@options.qt, 'QtWebKit.framework'),
					File.join(@options.qt, 'QtXml.framework'),
					File.join(@options.qt, 'QtXmlPatterns.framework'),
					File.join(@options.qt, 'phonon.framework') ],
				:froot => @options.qt,
				:location => '/Library/Frameworks',
				:identifier => 'org.esteid.installer.qt',
				:version => '4.6.3'
			}, {
				:name => 'esteid-qesteidutil',
				:files => File.join(@options.binaries, 'qesteidutil.app'),
				:helpers => [ 'pkmksendae', 'pkmkpidforapp' ],
				:froot => @options.binaries,
				:location => '/Applications'
			}, {
				:name => 'esteid-qdigidoccrypto',
				:files => File.join(@options.binaries, 'qdigidoccrypto.app'),
				:helpers => [ 'pkmksendae', 'pkmkpidforapp' ],
				:froot => @options.binaries,
				:location => '/Applications/Utilities'
			}, {
				:name => 'esteid-webplugin',
				:files => File.join(@options.binaries, 'esteid.plugin'),
				:helpers => [ 'pkmksendae', 'pkmkpidforapp' ],
				:froot => @options.binaries,
				:location => '/Library/Internet Plug-Ins'
			}, {
				:name => 'esteid-mozilla',
				:files => File.join(@options.binaries, @options.mozappid),
				:helpers => [ 'pkmksendae', 'pkmkpidforapp' ],
				:froot => @options.binaries,
				:identifier => 'org.esteid.installer.mozilla',
				:version => '1.0.$',
				:svnroot => '/esteid-browser-plugin/trunk',
				:location => '/Library/Application Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}'
			}, {
				:name => 'esteid-qdigidocclient',
				:files => File.join(@options.binaries, 'qdigidocclient.app'),
				:helpers => [ 'pkmksendae', 'pkmkpidforapp' ],
				:froot => @options.binaries,
				:location => '/Applications'
			}, {
				:name => 'esteid-contextmenu',
				:files => File.join(@options.binaries, 'EstEIDCM.plugin'),
				:froot => @options.binaries,
				:identifier => 'org.esteid.installer.cmi',
				:location => '/Library/Contextual Menu Items'
			}, {
				:name => 'esteid-updater',
				:files => File.join(@options.binaries, 'EstEIDSU.app'),
				:froot => @options.binaries,
				:identifier => 'org.esteid.installer.su',
				:location => '/Applications/Utilities'
			}, {
				:name => 'esteid-preferences',
				:files => File.join(@options.binaries, 'EstEIDPP.prefPane'),
				:helpers => [ 'pkmksendae', 'pkmkpidforapp' ],
				:froot => @options.binaries,
				:identifier => 'org.esteid.installer.pp',
				:location => '/Library/PreferencePanes'
			}, {
				:name => 'esteid-tokend-snowleopard',
				:files => File.join(@options.binaries, '10.6', 'OpenSC.tokend'),
				:froot => File.join(@options.binaries, '10.6'),
				:identifier => 'org.esteid.installer.tokend.10.6',
				:location => '/System/Library/Security/tokend',
				:version => '1.0.2',
				:script => "return system.version.ProductVersion >= '10.6';",
				:system => '10.6>='
			}, {
				:name => 'esteid-tokend-leopard',
				:files => File.join(@options.binaries, '10.5', 'OpenSC.tokend'),
				:froot => File.join(@options.binaries, '10.5'),
				:identifier => 'org.esteid.installer.tokend.10.5',
				:location => '/System/Library/Security/tokend',
				:version => '1.0.2',
				:script => "if(system.version.ProductVersion >= '10.6') { return false; } return system.version.ProductVersion >= '10.5';",
				:system => '10.5='
			}, {
				:name => 'esteid-drivers-leopard',
				:files => [ File.join(@options.drivers, '*/**') ],
				:froot => @options.drivers,
				:identifier => 'org.esteid.installer.drivers.10.5',
				:location => '/usr/libexec/SmartCardServices/drivers',
				:version => '1.1',
				:script => "if(system.version.ProductVersion >= '10.6') { return false; } return system.version.ProductVersion >= '10.5';",
				:system => '10.5='
			}, {
				:name => 'esteid',
				:title => 'ID-card',
				:files => File.join(@options.packages, '*.pkg'),
				:identifier => 'org.esteid.installer',
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

# don't buffer stdout and stderr
$stdout.sync=true
$stderr.sync=true
# redirect stderr to stdout
$stderr.reopen($stdout)

application = Application.new(ARGV, STDIN)
application.run
