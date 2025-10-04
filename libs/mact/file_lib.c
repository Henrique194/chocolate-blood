#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include "compat.h"
#include "types.h"
#include "develop.h"
#include "file_lib.h"
#include "_filelib.h"
#include "util_lib.h"

static char* FileNames[MAXFILEHANDLES];

int SafeOpen(const char *filename, int b, int c)
{
	int h;
	if (b & O_CREAT)
		h = open(filename, b, c);
	else
		h = open(filename, b);

	if (h == -1)
		Error("Error opening %s: %s\n", filename, strerror(errno));
	if (h < MAXFILEHANDLES)
	{
		FileNames[h] = SafeMalloc(strlen(filename) + 1);
		strcpy(FileNames[h], filename);
	}
	return h;
}

int32 SafeOpenWrite(const char* filename, int32 filetype)
{
	int h;
	if (filetype == filetype_binary)
	{
		h = SafeOpen(filename, O_BINARY | O_TRUNC | O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
		return h;
	}
	else if (filetype == filetype_text)
	{
		h = SafeOpen(filename, O_TEXT | O_TRUNC | O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
		return h;
	}
	Error("SafeOpenWrite: Illegal filetype specified");

	return -1;
}

int32 SafeOpenRead(const char* filename, int32 filetype)
{
	int h;
	if (filetype == filetype_binary)
	{
		h = SafeOpen(filename, O_BINARY, 0);
		return h;
	}
	else if (filetype == filetype_text)
	{
		h = SafeOpen(filename, O_TEXT, 0);
		return h;
	}
	Error("SafeOpenRead: Illegal filetype specified");

	return -1;
}

int32 SafeOpenAppend(const char* filename, int32 filetype)
{
	int h;
	if (filetype == filetype_binary)
	{
		h = SafeOpen(filename, O_BINARY | O_APPEND | O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
		return h;
	}
	else if (filetype == filetype_text)
	{
		h = SafeOpen(filename, O_TEXT | O_APPEND | O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
		return h;
	}
	Error("SafeOpenAppend: Illegal filetype specified");

	return -1;
}

void SafeClose(int32 handle)
{
	if (close(handle))
	{
		if (handle < MAXFILEHANDLES)
			Error("Unable to close file %s\n", FileNames[handle]);
		else
			Error("Unable to close file\n");
	}
	if (handle < MAXFILEHANDLES)
	{
		SafeFree(FileNames[handle]);
		FileNames[handle] = NULL;
	}
}

boolean SafeFileExists(const char* filename)
{
	return access(filename, 0) == 0;
}

int32 SafeFileLength(int32 handle)
{
	return filelength(handle);
}

int32 SafeTell(int32 handle)
{
	return lseek(handle, 0, SEEK_CUR);
}

void SafeSeek(int32 handle, int32 position)
{
	lseek(handle, position, SEEK_SET);
}

void SafeRead(int32 handle, void* buffer, int32 count)
{
	int chunk;
	while (count)
	{
		chunk = count > 0x7fff ? 0x7fff : count;
		if (read(handle, buffer, chunk) != chunk)
			Error("File read failure %s reading %ld bytes from file %s", strerror(errno), count, FileNames[handle]);
		buffer = (char*)buffer + chunk;
		count -= chunk;
	}
}

void SafeWrite(int32 handle, void* buffer, int32 count)
{
	int chunk;
	while (count)
	{
		chunk = count > 0x7fff ? 0x7fff : count;
		if (write(handle, buffer, chunk) != chunk)
			Error("File write failure %s writing %ld bytes to file %s", strerror(errno), count, FileNames[handle]);
		buffer = (char*)buffer + chunk;
		count -= chunk;
	}
}

int32 LoadFile(const char* filename, void** bufferptr)
{
	int h;
	int l;
	h = SafeOpenRead(filename, filetype_binary);
	l = SafeFileLength(h);
	*bufferptr = SafeMalloc(l);
	SafeRead(h, *bufferptr, l);
	SafeClose(h);
	return l;
}

void SaveFile(const char* filename, void* bufferptr, int32 count)
{
	int h;

	h = SafeOpenWrite(filename, filetype_binary);
	SafeWrite(h, bufferptr, count);
	SafeClose(h);
}

void GetPathFromEnvironment(char* fullname, const char* envname, const char* filename)
{
	char* s;

	s = getenv(envname);

	if (s)
	{
		strcpy(fullname, s);
		if (fullname[strlen(fullname)] != '\\')
			strcat(fullname, "\\");
		strcat(fullname, filename);
	}
	else
		strcpy(fullname, filename);
}

void DefaultExtension(char* path, const char* extension)
{
	char* s;

	s = path + strlen(path) - 1;

	while (*s != '\\' && s != path)
	{
		if (*s == '.')
			return;
		s--;
	}
	strcat(s, extension);
}

void DefaultPath(char* path, const char* basepath)
{
	char buf[128];
	if (*path == '\\')
		return;

	strcpy(buf, path);
	strcpy(path, basepath);
	strcat(path, buf);
}

void ExtractFileBase(char* path, char* dest)
{
	char* s;
	int l;

	s = path + strlen(path) - 1;
	while (s != path && *(s - 1) != '\\')
		s--;

	memset(dest, 0, 8);
	l = 0;
	while (*s != 0 && *s != '.')
	{
		if (++l == 9)
			Error("Filename base of %s > 8 chars", path);
		*dest++ = tolower(*s++);
	}
}

boolean GetExtension(char* filename, char* extension)
{
	char* s;

	s = filename + strlen(filename) - 1;
	while (s != filename && *(s - 1) != '.')
		s--;

	if (s == filename)
		return false;
	strcpy(extension, s);
	return true;
}

void SetExtension(char* filename, const char* extension)
{
	char* s;

	s = filename + strlen(filename) - 1;
	while (s != filename && *(s - 1) != '.')
		s--;

	s--;
	*s = 0;
	strcat(s, extension);
}

#if 0

char* GetPath(char* path, char* dir)
{
	boolean f = false;
	char* s = dir;
	int c = 0;
	if (*path == SLASHES)
		path++;

	while (!f)
	{
		*s = *path;

		c++;

		if (c > MAXCHARS)
			Error("ERROR : Directory name can only be %d characters long.", MAXCHARS);

		path++;
		c++;

		if (*path == SLASHES || *path == '\0')
			f = true;
	}

	*s = '\0';

	return path;
}

boolean ChangeDirectory(char* path)
{
	char buf[9];
	char* s;
	char* p;

	s = buf;
	p = path;
	memset(buf, 0, sizeof(buf));

	if (*(p + 1) == ':')
	{
		*s++ = *p++;
		*s++ = *p++;
		if (!ChangeDrive(buf))
			return false;
	}

	if (*p == SLASHES)
	{
		chdir("\\");
		p++;
	}

	s = buf;
	while (*p)
	{
		p = GetPath(p, s);
		if (chdir(p) == -1)
			return false;
	}

	return true;
}

boolean ChangeDrive(char* drive)
{
	int d, v1, v2;

	d = toupper(*drive);

	d = d - 'A' + 1;

	_dos_setdrive(d, &v1);
	_dos_getdrive(d, &v2);
	if (d != v2)
		return false;
	return true;
}

#endif
