#pragma once

#include <stdint.h>

namespace BrunoEngine
{
	class BitCopier
	{
	public:
		template <typename TSource, typename TDestination>
		static void Copy(const TSource* &sourceArray, int sourceOffset, int sourceStride, TDestination* &destinationArray, int destinationOffset, int destinationStride, int valueSize, int valueCount, int sourceArraySize, int destinationArraySize);
	};

	template <typename TSource, typename TDestination>
	void BitCopier::Copy(const TSource* &sourceArray, int sourceOffset, int sourceStride, TDestination* &destinationArray, int destinationOffset, int destinationStride, int valueSize, int valueCount, int sourceArraySize, int destinationArraySize)
	{
        int num3 = sizeof(TSource);
        int num2 = sizeof(TDestination);
		if ((((valueSize < 0) || (valueCount < 0)) || ((sourceOffset < 0) || (destinationOffset < 0))) || (((sourceStride < valueSize) || (destinationStride < valueSize)) || ((((((valueCount - 1) * sourceStride) + sourceOffset) + valueSize) > (sourceArraySize * num3)) || (((((valueCount - 1) * destinationStride) + destinationOffset) + valueSize) > (destinationArraySize * num2)))))
        {
            throw std::exception("ArgumentOutOfRangeException");
        }

		if (valueCount != 0)
        {
			uint8_t* numPtr3 = (uint8_t*)sourceArray;
			numPtr3 += sourceOffset;
			uint8_t* numPtr2 = (uint8_t*)destinationArray;
			numPtr2 += destinationOffset;

			if (valueCount > 0)
			{
				do
				{
					//TO-DO: usar memcpy
					if (valueSize > 0)
					{
						memcpy(numPtr2, numPtr3, valueSize);
					}
					numPtr3 += sourceStride;
					numPtr2 += destinationStride;
					valueCount--;
				} while (valueCount > 0);
			}
		}
	}
}