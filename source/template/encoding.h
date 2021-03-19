/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

template<class SourceCodec, class DestCodec>
size_t ir::Encoding::recode(typename DestCodec::Char *dest, const typename SourceCodec::Char *source, const typename DestCodec::Char *errstring) noexcept
{
	size_t isource = 0;
	size_t idest = 0;
	
	while (true)
	{
		//Decoding
		size_t nsource;
		uint32 code = SourceCodec::decode(&source[isource], &nsource);
		isource += nsource;
		
		//Encoding
		size_t ndest;
		if (DestCodec::encode(code, (dest == nullptr) ? nullptr : &dest[idest], &ndest))
		{
			if (code == 0) return idest;
			idest += ndest;
		}
		else
		{
			if (errstring == nullptr)
			{
				DestCodec::encode(code, (dest == nullptr) ? nullptr : &dest[idest], &ndest);
				return idest;
			}
			else
			{
				size_t ierr = 0;
				while (errstring[ierr] != '\0')
				{
					if (dest != nullptr) dest[idest] = errstring[ierr];
					idest++;
					ierr++;
				}
			}
		}
	}
}

template<class SourceCodec, class DestCodec>
typename DestCodec::Char *ir::Encoding::alloc_recode(const typename SourceCodec::Char *source, const typename DestCodec::Char *errstring) noexcept
{
	size_t size = (recode<SourceCodec, DestCodec>(nullptr, source, errstring) + 1) * sizeof(typename DestCodec::Char);
	typename DestCodec::Char *result = (typename DestCodec::Char*)malloc(size);
	if (result == nullptr) return nullptr;
	recode<SourceCodec, DestCodec>(result, source, errstring);
	return result;
}

template<class SourceCodec, class DestCodec>
typename DestCodec::Char *ir::Encoding::buffer_recode(const typename SourceCodec::Char *source, const typename DestCodec::Char *errstring) noexcept
{
	size_t size = (recode<SourceCodec, DestCodec>(nullptr, source, errstring) + 1) * sizeof(typename DestCodec::Char);
	if (!_buffer.resize(size)) return nullptr;
	recode<SourceCodec, DestCodec>((typename DestCodec::Char*)&_buffer[0], source, errstring);
	return (typename DestCodec::Char*)&_buffer[0];
}