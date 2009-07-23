/* Copyright (c) 2009 Janek Priimann */

#import <CommonCrypto/CommonDigest.h>
#import <Foundation/Foundation.h>
#import <Security/Authorization.h>
#import <signal.h>
#import <sys/stat.h>
#import <sys/wait.h>
#import <sys/types.h>
#import <sys/fcntl.h>
#import <sys/errno.h>
#import <unistd.h>
#import <dlfcn.h>

static inline NSString *EstEIDAgentGetString(const char *str)
{
	return [[[NSString alloc] initWithBytes:str length:strlen(str) encoding:NSUTF8StringEncoding] autorelease];
}

static NSString *EstEIDAgentGetSha1(char *path)
{
	int file = open(path, O_RDONLY);
	NSMutableString *result = nil;
	
	if(file != -1) {
		UInt8 buffer[8192];
		CC_SHA1_CTX sha1;
		UInt8 bytes[20];
		int i, length;
		
		CC_SHA1_Init(&sha1);
		
		while((length = read(file, &(buffer[0]), 8192)) > 0) {
			CC_SHA1_Update(&sha1, &(buffer[0]), length);
		}
		
		CC_SHA1_Final(&(bytes[0]), &sha1);
		
		result = [NSMutableString string];
		
		for(i = 0; i < 20; i++) {
			[result appendFormat:@"%02x", bytes[i]];
		}
		
		close(file);
	}
	
	return result;
}

static NSString *EstEIDAgentGetExecutablePath()
{
	int (*getExecutablePath)(char *buffer, size_t *size) = (int (*)(char *, size_t *))dlsym(RTLD_DEFAULT, "_NSGetExecutablePath");
	char *inPathBuffer = malloc(2048);
	size_t inOutBufferSize = 2048;
	NSString *path;
	
	if(getExecutablePath != nil) {
		if(getExecutablePath(inPathBuffer, &inOutBufferSize) == -1) {
			inPathBuffer = realloc(inPathBuffer, inOutBufferSize);
			
			if(getExecutablePath(inPathBuffer, &inOutBufferSize) <= 0) {
				free(inPathBuffer);
				
				return nil;
			}
		}
		
		path = [[NSString alloc] initWithBytes:inPathBuffer length:inOutBufferSize encoding:NSUTF8StringEncoding];
	}
	
	free(inPathBuffer);
	
	return [path autorelease];
}

static int EstEIDAgentExecuteCommand(AuthorizationRef authorization, char *command, ...)
{
	int result = EXIT_FAILURE;
	int argc = 0;
	va_list list;
	char **argv;
	char *arg;
	int cpid;
	
	va_start(list, command);
	while((arg = va_arg(list, char *)) != NULL) argc++;
	va_end(list);
	
	argv = malloc(sizeof(char *) * (argc + 2));
	
	if(argv != NULL) {
		argv[0] = command;
		argv[argc + 1] = NULL;
		argc = 1;
		
		va_start(list, command);
		while((arg = va_arg(list, char *)) != NULL) argv[argc++] = arg;
		va_end(list);
		
		if(authorization != NULL) {
			if(AuthorizationExecuteWithPrivileges(authorization, command, kAuthorizationFlagDefaults, argv + 1, NULL) == errAuthorizationSuccess && wait(&cpid) != -1 && WIFEXITED(cpid)) {
				result = WEXITSTATUS(cpid);
			}
		} else {
			cpid = fork();
			
			if(cpid == 0) {
				execvp(command, argv);
				exit(EXIT_FAILURE);
			} else {
				if(wait(&cpid) != -1 && WIFEXITED(cpid)) {
					result = WEXITSTATUS(cpid);
				}
			}
		}
		
		free(argv);
	}
	
	return result;
}

static int EstEIDAgentLaunchdSetEnabled(AuthorizationRef authorization, NSString *path, BOOL flag)
{
	NSDictionary *plist = [[NSDictionary alloc] initWithContentsOfFile:path];
	BOOL launchd = NO;
	int result = 0;
	
	if(plist) {
		id obj = [plist objectForKey:@"Disabled"];
		
		if([obj isKindOfClass:[NSNumber class]] && [(NSNumber *)obj boolValue] == flag) {
			NSMutableDictionary *plist_1 = [[NSMutableDictionary alloc] init];
			
			[plist_1 addEntriesFromDictionary:plist];
			[plist_1 setObject:[NSNumber numberWithBool:!flag] forKey:@"Disabled"];
			launchd = [plist_1 writeToFile:path atomically:YES];
			[plist_1 release];
			[plist release];
		}
	} else if(flag) {
		time_t ctime = time(NULL);
		struct tm *ltime = localtime(&ctime);
		const char *identifier = "org.esteid.updater";
		NSString *updater = @"/Applications/Utilities/EstEIDSU.app/Contents/MacOS/EstEIDSU";
		NSString *plist_1 = [[NSString alloc] initWithFormat:
							 @"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
							 @"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
							 @"<plist version=\"1.0\">\n"
							 @"\t<dict>\n"
							 @"\t\t<key>Disabled</key>\n"
							 @"\t\t<false />\n"
							 @"\t\t<key>Label</key>\n"
							 @"\t\t<string>%s</string>\n"
							 @"\t\t<key>Program</key>\n"
							 @"\t\t<string>%@</string>\n"
							 @"\t\t<key>ProgramArguments</key>\n"
							 @"\t\t<array>\n"
							 @"\t\t\t<string>%@</string>\n"
							 @"\t\t\t<string>--silent</string>\n"
							 @"\t\t</array>\n"
#ifdef DEBUG
							 @"\t\t<key>StartInterval</key>\n"
							 @"\t\t<integer>60</integer>\n"
#else
							 // Randomize the time when the requests are made in order to avoid mass-synchronizations.
							 @"\t\t<key>StartCalendarInterval</key>\n"
							 @"\t\t<dict>\n"
							 @"\t\t\t<key>Hour</key>\n"
							 @"\t\t\t<integer>%d</integer>\n"
							 @"\t\t\t<key>Minute</key>\n"
							 @"\t\t\t<integer>%d</integer>\n"
							 @"\t\t</dict>\n"
#endif
							 @"\t\t<key>LimitLoadToSessionType</key>\n"
							 @"\t\t<string>Aqua</string>\n"
							 @"\t</dict>\n"
							 @"</plist>\n", identifier, updater, updater, ltime->tm_hour, ltime->tm_min];
		
		launchd = [plist_1 writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
		[plist_1 release];
	}
		
	if(launchd) {
		if(plist) EstEIDAgentExecuteCommand(authorization, "/bin/launchctl", "unload", [path fileSystemRepresentation], NULL);
		result = EstEIDAgentExecuteCommand(authorization, "/bin/launchctl", "load", [path fileSystemRepresentation], NULL);
	}
	
	return result;
}

int main(int argc, char *argv[])
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	AuthorizationExternalForm externalAuthorization;
	AuthorizationRef authorization = NULL;
	int result = EXIT_FAILURE;
	int extauth = 0;
	
	if(argc > 1) {
		if(strcmp(argv[argc - 1], "--extauth") == 0) {
			extauth = 1;
			argc--;
		}
	}
	
	if(!extauth || read(0, &externalAuthorization, sizeof(externalAuthorization)) == sizeof(externalAuthorization)) {
		if(!extauth || AuthorizationCreateFromExternalForm(&externalAuthorization, &authorization) == errAuthorizationSuccess) {
			if(extauth && geteuid() != 0 && !(argc > 1 && strcmp(argv[argc - 1], "--repair") == 0)) {
				char *args[] = { "--repair", "--extauth", NULL };
				FILE *pipe = NULL;
				
				if(AuthorizationExecuteWithPrivileges(authorization, [EstEIDAgentGetExecutablePath() UTF8String], kAuthorizationFlagDefaults, args, &pipe) == errAuthorizationSuccess) {
					int cpid;
					
					fwrite(&externalAuthorization, 1, sizeof(externalAuthorization), pipe);
					fflush(pipe);
					fclose(pipe);
					
					if(wait(&cpid) != -1 && WIFEXITED(cpid)) {
						result = WEXITSTATUS(cpid);
					}
				}
				
				if(result == 0) {
					char *args[argc + 1];
					
					args[argc - 0] = "--extauth";
					args[argc - 1] = NULL;
					for(int i = 1; i < argc; i++) args[i - 1] = argv[i];
					
					if(AuthorizationExecuteWithPrivileges(authorization, [EstEIDAgentGetExecutablePath() UTF8String], kAuthorizationFlagDefaults, args, &pipe) == errAuthorizationSuccess) {
						int cpid;
						
						fwrite(&externalAuthorization, 1, sizeof(externalAuthorization), pipe);
						fflush(pipe);
						fclose(pipe);
						
						result = (wait(&cpid) != -1 && WIFEXITED(cpid)) ? WEXITSTATUS(cpid) : EXIT_FAILURE;
					} else {
						result = EXIT_FAILURE;
					}
				}
			} else if(argc == 2 && strcmp(argv[1], "--repair") == 0) {
				int tool = open([EstEIDAgentGetExecutablePath() fileSystemRepresentation], O_NONBLOCK | O_RDONLY | O_EXLOCK, 0);
				struct stat st;
				
				if((tool != -1) && (fstat(tool, &st) == 0)) {
					int chownerr = (st.st_uid != 0) ? fchown(tool, 0, st.st_gid) : 0;
					int chmoderr = fchmod(tool, (st.st_mode & (~(S_IWGRP | S_IWOTH))) | S_ISUID);
					
					close(tool);
					
					if(chownerr == 0 && chmoderr == 0) {
						result = 0;
					}
				}
				
				if(result == 0) {
					fprintf(stderr, "EstEIDAgent: Repair succeeded!\n");
				} else {
					fprintf(stderr, "EstEIDAgent: Repair failed.\n");
				}
			// [enable|disable] [file]
			} else if(argc == 4 && strcmp(argv[1], "--launchd") == 0) {
				if(strcmp(argv[2], "enable") == 0) {
					result = EstEIDAgentLaunchdSetEnabled(authorization, EstEIDAgentGetString(argv[3]), YES);
				} else if(strcmp(argv[2], "disable") == 0) {
					result = EstEIDAgentLaunchdSetEnabled(authorization, EstEIDAgentGetString(argv[3]), NO);
				}
			// [enable|disable] [user] [entry]
			} else if(argc == 5 && strcmp(argv[1], "--idlogin") == 0) {
				if(strcmp(argv[2], "enable") == 0) {
					if((result = EstEIDAgentExecuteCommand(authorization, "/usr/bin/dscl", ".", "-append", argv[3], "AuthenticationAuthority", argv[4], NULL)) != 0) {
						fprintf(stderr, "EstEIDAgent: Couldn't enable id-login for %s.\n", argv[3]);
					}
				} else if(strcmp(argv[2], "disable") == 0) {
					if((result = EstEIDAgentExecuteCommand(authorization, "/usr/bin/dscl", ".", "-delete", argv[3], "AuthenticationAuthority", argv[4], NULL)) != 0) {
						fprintf(stderr, "EstEIDAgent: Couldn't disable id-login for %s.\n", argv[3]);
					}
				}
			// [checksum] [file] [identifier] [target]
			} else if((argc == 5 || argc == 6) && strcmp(argv[1], "--install") == 0) {
				char *tmp1 = strdup("/tmp/esteid-agent/XXXXXX");
				struct stat st;
				
				if(stat("/tmp/esteid-agent", &st) == 0) {
					EstEIDAgentExecuteCommand(authorization, "/bin/rm", "-R", "/tmp/esteid-agent", NULL);
				}
				
				mkdir("/tmp/esteid-agent", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				tmp1 = mktemp(tmp1);
				
				if(stat("/tmp/esteid-agent", &st) == 0 && st.st_uid == 0 && EstEIDAgentExecuteCommand(authorization, "/bin/cp", argv[3], tmp1, NULL) == 0) {
					char *tmp2 = (char *)[[NSString stringWithFormat:@"/tmp/esteid-agent/%s.pkg", argv[4]] UTF8String];
					
					if(stat(tmp1, &st) == 0 && st.st_uid == 0 && [EstEIDAgentGetString(argv[2]) isEqualToString:EstEIDAgentGetSha1(tmp1)]) {
						if(EstEIDAgentExecuteCommand(authorization, "/usr/bin/ditto", "-x", "-k", tmp1, tmp2, NULL) == 0) {
							if((result = EstEIDAgentExecuteCommand(authorization, "/usr/sbin/installer", "-pkg", tmp2, "-target", (argc == 6) ? argv[5] : "/", NULL)) == 0) {
								fprintf(stderr, "EstEIDAgent: Installed %s successfully\n", argv[4]);
							} else {
								fprintf(stderr, "EstEIDAgent: Installing %s failed.\n", argv[4]);
							}
						} else {
							fprintf(stderr, "EstEIDAgent: Couldn't uncompress %s.\n", argv[4]);
						}
					} else {
						fprintf(stderr, "EstEIDAgent: Checksum/permissions don't match for %s\n", argv[4]);
					}
				} else {
					fprintf(stderr, "EstEIDAgent: Couldn't copy package %s.\n", argv[4]);
				}
				
				EstEIDAgentExecuteCommand(authorization, "/bin/rm", "-R", "/tmp/esteid-agent", NULL);
				free(tmp1);
			} else if(argc == 2 && strcmp(argv[1], "--preflight") == 0) {
				// Do nothing, reserved for future usage.
				result = 0;
			} else if(argc == 2 && strcmp(argv[1], "--postflight") == 0) {
				// Do nothing, reserved for future usage.
				result = 0;
			}
		}
	}
	
	[pool release];
	
	return result;
}
