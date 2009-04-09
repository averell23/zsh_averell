#include <sys/param.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include <CoreServices/CoreServices.h>

static Boolean isFinderAlias(const char *pathname) {
  FSRef ref;
  Boolean aliasFileFlag, folderFlag;
  
  if (noErr == FSPathMakeRef((const UInt8 *)pathname, &ref, NULL)) {
    if (noErr == FSIsAliasFile(&ref, &aliasFileFlag, &folderFlag)) {
      return aliasFileFlag;
    }
  }
  
  return FALSE;
}

static char *resolveFinderAlias(const char *pathname) {
  FSRef ref;
  Boolean targetIsFolder, wasAliased;
  char *resolvedAlias;
  
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
  
  if (!errno)
    errno = ENOENT;
  
  return NULL;
}

int main(int argc, char* argv[]) {
  char buffer[PATH_MAX];
  char *pathname;
  int err = EINVAL;
  
  if (argc > 1) {
    if (isFinderAlias(argv[1])) {
      if (NULL != (pathname = resolveFinderAlias(argv[1]))) {  
        fprintf(stdout, "%s\n", pathname);
        free(pathname);
        err = 0;
      }
    }
    
    if (err) {
      if (NULL != (pathname = realpath(argv[1], buffer))) {
        fprintf(stdout, "%s\n", pathname);
        err = 0;
      }
    }
    
    if (err) {
      fprintf(stderr, "failed: \"%s\" %s\n", strerror(errno), argv[1]);
      err = errno;
    }
  }
  
  return err;
}