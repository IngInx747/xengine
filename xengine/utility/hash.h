#pragma once
#ifndef XE_HASH_H
#define XE_HASH_H

#include <string>

namespace xengine
{
	namespace hash
	{
		inline unsigned int hash(const std::string& str)
		{
			unsigned int code = 0;

			for (char c : str)
			{
				code = 37 * code + 17 * c;
			}

			return code;
		}
	}
}

#endif // !XE_HASH_H
