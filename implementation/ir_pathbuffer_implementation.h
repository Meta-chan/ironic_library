#ifndef IR_PATHBUFFER_IMPLEMENTATION
#define IR_PATHBUFFER_IMPLEMENTATION

#include <Windows.h>

struct Buffer
{
	wchar_t *data				= nullptr;
	unsigned int used			= 0;
	unsigned int allocated		= 0;
};

Buffer pathbuffer, directorybuffer;

bool bufferreserve(Buffer *buffer, unsigned int nchars)
{
	if (buffer->allocated < nchars)
	{
		if (buffer->data == nullptr) buffer->data = (wchar_t*)malloc((nchars + 1) * sizeof(wchar_t));
		else buffer->data = (wchar_t*)realloc(buffer->data, (nchars + 1) * sizeof(wchar_t));

		if (buffer->data == nullptr)
		{
			buffer->allocated = 0;
			return false;
		}
		buffer->allocated = nchars;
	}
	return true;
};

bool ir::pathinit(const wchar_t *directory, bool relative)
{
	if (directory == nullptr)
	{
		if (!bufferreserve(&directorybuffer, 512)) return false;
		directorybuffer.used = GetModuleFileNameW(NULL, directorybuffer.data, directorybuffer.allocated);
		while (directorybuffer.used == directorybuffer.allocated)
		{
			if (!bufferreserve(&directorybuffer, directorybuffer.allocated + 512)) return false;
			directorybuffer.used = GetModuleFileNameW(NULL, directorybuffer.data, directorybuffer.allocated);
		}

		while (directorybuffer.data[directorybuffer.used - 1] != '\\')
		{
			directorybuffer.used--;
		}
	}
	else
	{
		unsigned int length = wcslen(directory);
		if (!bufferreserve(&directorybuffer, length)) return false;
		memcpy(directorybuffer.data, directory, length * sizeof(wchar_t));
		directorybuffer.used = length;
	}
	return true;
};

wchar_t *ir::pathassembly(unsigned int nparts, const wchar_t *parts, ...)
{
	if (directorybuffer.data == nullptr) return false;

	const unsigned char *pread = (unsigned char*)&parts;
	unsigned int resultlen = 0;

	for (unsigned int i = 0; i < nparts; i++)
	{
		const wchar_t *part = *((const wchar_t**)pread);
		unsigned int partlen = wcslen(part);
		pread += sizeof(const wchar_t*);
		resultlen += partlen;
		if (resultlen > 2048) return nullptr;
	}

	bool wasinited = pathbuffer.data != nullptr;
	if (!bufferreserve(&pathbuffer, directorybuffer.used + resultlen)) return false;
	if (!wasinited) memcpy(pathbuffer.data, directorybuffer.data, directorybuffer.used * sizeof(wchar_t));
	pread = (unsigned char*)&parts;
	resultlen = 0;

	for (unsigned int i = 0; i < nparts; i++)
	{
		const wchar_t *part = *((const wchar_t**)pread);
		unsigned int partlen = wcslen(part);
		memcpy(pathbuffer.data + directorybuffer.used + resultlen, part, partlen * sizeof(wchar_t));
		pread += sizeof(const wchar_t*);
		resultlen += partlen;
		if (resultlen > 2048) return nullptr;
	}

	pathbuffer.used = directorybuffer.used + resultlen;
	pathbuffer.data[pathbuffer.used] = '\0';
	return pathbuffer.data;
};
#endif