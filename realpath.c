//
// realpath.c
//
// Copyright (C) 2001-2015 Cucurbita. All rights reserved.
//

#include <sys/param.h>
#include <sys/stat.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

// http://stackoverflow.com/users/44065/jmucchiello
static char * str_replace(char * orig, char * rep, char * with)
{
	char * result;
	char * ins;
	char * tmp;
	int len_rep;
	int len_with;
	int len_front;
	int count;

	if (!orig) {
		return NULL;
	}
	if (!rep) {
		rep = "";
	}
	len_rep = strlen(rep);
	if (!with) {
		with = "";
	}
	len_with = strlen(with);

	ins = orig;
	for (count = 0; (tmp = strstr(ins, rep)); ++count) {
		ins = tmp + len_rep;
	}
	tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);
	if (!result) {
		return NULL;
	}
	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; // move to next "end of rep"
	}
	strcpy(tmp, orig);
	return result;
}

// cc -Wall realpath.c -o realpath -framework CoreServices -framework CoreFoundation
// cc -Wall realpath.c -o realpath if not apple

#ifdef __APPLE__
	
	#include <AvailabilityMacros.h>
	
	#if (MAC_OS_X_VERSION_MAX_ALLOWED > 1070)
		
		#include <stdbool.h>
		#include <CoreFoundation/CoreFoundation.h>
		
		static CFURLRef createCFURLFromPosixPath(const char * pathname)
		{
			CFURLRef url = NULL;
			struct stat sb;
			Boolean isdir = FALSE;
			if (stat(pathname, &sb) != -1) {
				isdir = (sb.st_mode & S_IFDIR);
			}
			url = CFURLCreateFromFileSystemRepresentation
			(
				kCFAllocatorDefault,
				(const UInt8 *)pathname,
				strlen(pathname),
				isdir
			);
			return url;
		}
	
		static bool isFinderAlias(const char * pathname)
		{
			bool result = false;
			CFURLRef url = NULL;
			CFBooleanRef isAlias = kCFBooleanFalse;
			if (NULL != (url = createCFURLFromPosixPath(pathname))) {
				if(CFURLCopyResourcePropertyForKey(url, kCFURLIsAliasFileKey, &isAlias, NULL)) {
					result = (CFBooleanGetValue(isAlias));
				}
				CFRelease(url);
				url = NULL;
			}
			return result;
		}
		
		static char * resolveFinderAlias(const char  * pathname)
		{
			CFURLRef url = NULL;
			CFErrorRef * err = NULL;
			CFDataRef data = NULL;
			CFURLRef resolvedURL = NULL;
			char * resolvedAlias = NULL;
			
			if (NULL != (url = createCFURLFromPosixPath(pathname))) {
				if ((data = CFURLCreateBookmarkDataFromFile(kCFAllocatorDefault, url, err))) {
					if (!err) {
						if (NULL != (resolvedURL = CFURLCreateByResolvingBookmarkData(
							kCFAllocatorDefault,
							data,
							kCFBookmarkResolutionWithoutUIMask,
							NULL,
							NULL,
							FALSE,
							err
						)))
						{
							if (!err) {
								if (NULL != (resolvedAlias = malloc(PATH_MAX + 1))) {
									if(CFURLGetFileSystemRepresentation(resolvedURL, TRUE, (UInt8 *)resolvedAlias, PATH_MAX)) {
										errno = 0;
										CFRelease(resolvedURL);
										CFRelease(data);
										CFRelease(url);
										return resolvedAlias;
									}
									free(resolvedAlias);
								}
							}
							CFRelease(resolvedURL);
						}
					}
					CFRelease(data);
				}
				CFRelease(url);
			}
			if (!errno) {
				errno = ENOENT;
			}
			return NULL;
		}
		
	#else
	
		#include <CoreServices/CoreServices.h>

		static Boolean isFinderAlias(const char * pathname)
		{
			FSRef ref;
			Boolean aliasFileFlag, folderFlag;
	
			if (noErr == FSPathMakeRef((const UInt8 *)pathname, &ref, NULL)) {
				if (noErr == FSIsAliasFile(&ref, &aliasFileFlag, &folderFlag)) {
					return aliasFileFlag;
				}
			}	
			return FALSE;
		}

		static char * resolveFinderAlias(const char  * pathname)
		{
			FSRef ref;
			Boolean targetIsFolder, wasAliased;
			char * resolvedAlias = NULL;
	
			if (noErr == FSPathMakeRef((const UInt8 *)pathname, &ref, NULL)) {
				if (noErr == FSResolveAliasFile(&ref, TRUE, &targetIsFolder, &wasAliased)) {
					if (TRUE == wasAliased) {
						if (NULL != (resolvedAlias = malloc(PATH_MAX + 1))) {
							if (noErr == FSRefMakePath(&ref, (UInt8 *)resolvedAlias, PATH_MAX)) {
								errno = 0;
								return resolvedAlias;
							}
							free(resolvedAlias);
						}
					}
				}
			}
	
			if (!errno) {
				errno = ENOENT;
			}
			return NULL;
		}
	#endif
#endif

int main(int argc, char * argv[])
{
	static char buffer[PATH_MAX + 1];
	char * pathname, * tmp;
	int err = EINVAL;
	
	if (argc > 1) {
	
#ifdef __APPLE__

		if (isFinderAlias(argv[1])) {
			if (NULL != (pathname = resolveFinderAlias(argv[1]))) {	
				tmp = str_replace(pathname, " ", "\\ ");
				if (tmp) {
					fprintf(stdout, "%s\n", tmp);
					free(tmp);
				} else {
					fprintf(stdout, "%s\n", pathname);
				}
				free(pathname);
				pathname = NULL;
				err = 0;
			}
		}

#endif
		if (err) {
			if (NULL != (pathname = realpath(argv[1], buffer))) {
				tmp = str_replace(pathname, " ", "\\ ");
				if (tmp) {
					fprintf(stdout, "%s\n", tmp);
					free(tmp);
				} else {
					fprintf(stdout, "%s\n", pathname);
				}
				err = 0;
			}
		}
		
		if (err) {
			tmp = str_replace(argv[1], " ", "\\ ");
			if (tmp) {
				fprintf(stderr, "failed: \"%s\" %s\n", strerror(errno), tmp);
				free(tmp);
			} else {
				fprintf(stderr, "failed: \"%s\" %s\n", strerror(errno), argv[1]);
			}
			err = errno;
		}
	}
	
	return err;
}

/* EOF */
