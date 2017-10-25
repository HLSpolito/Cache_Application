#pragma once
#include "cache_associative.h"

#ifndef WAY
#define WAY 1
#endif

typedef CACHE_ASSOC::Cache<int, 0, CACHE, WAY> CacheA;

template<typename T, int ARRAY_SIZE_BITS, int WORD_BITS> class BitonicSortCache{
	public:
		static const int ARRAY_SIZE = 1 << ARRAY_SIZE_BITS;
	public:

		BitonicSortCache(CacheA::DataType* array):array(array){
		}

		void sort(bool dir){
#pragma HLS INLINE
			CacheA _cache(array);
			for(int bits = 1; bits <= ARRAY_SIZE_BITS; bits ++){
				for(int stride = bits - 1; stride >= 0; stride--){
					for(int i=0; i< ARRAY_SIZE/2; i++){
						bool l_dir = ((i >> (bits - 1)) & 0x01);
						l_dir = l_dir ^ dir;
						int step = 1 << stride;
						int POS = i *2 - (i & (step - 1));
						T p0 = _cache[POS], p1 = _cache[POS + step];
						compare(p0, p1, l_dir);
						_cache.modify(POS, p0);
						_cache.modify(POS + step, p1);
					}
				}

			}
		}
	private:

		bool compare(T* pos0, T* pos1, bool dir){
#pragma HLS INLINE
			if((*pos0 > *pos1) != dir){
				T tmp = *pos0;
				*pos0 = *pos1;
				*pos1 = tmp;
				return true;
			}
			return false;
		}

		bool compare(T &pos0, T &pos1, bool dir){
#pragma HLS INLINE
			if((pos0 > pos1) != dir){
				T tmp = pos0;
				pos0 = pos1;
				pos1 = tmp;
				return true;
			}
			return false;
		}

		CacheA::DataType* const array;

};
