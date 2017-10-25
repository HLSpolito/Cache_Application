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
			const int factor = 1 << CACHE;
			for(int bits = 1; bits <= ARRAY_SIZE_BITS; bits ++){
				for(int stride = bits - 1; stride >= 0; stride--){
					for(int i=0; i< ARRAY_SIZE/2; ){
						int step = 1 << stride;
						int p_tmp0 = i *2 - (i & (step - 1));
						int p_tmp1 = step + (i+factor-1) *2 - ((i+factor-1) & (step - 1));
						T tmp0=_cache[p_tmp0], tmp1=_cache[p_tmp1];
						for(int f=0;f<factor;i++,f++){
#pragma HLS PIPELINE
							bool l_dir = ((i >> (bits - 1)) & 0x01);
							l_dir = l_dir ^ dir;
							int POS = i *2 - (i & (step - 1));
							T p0 = _cache.retrieve(POS), p1 = _cache.retrieve(POS + step);
							compare(p0, p1, l_dir);
							_cache.modify(POS, p0);
							_cache.modify(POS + step, p1);
						}
					}
				}

			}
#ifndef __SYNTHESIS__
#include<iostream>
			std::cout<<"The hit ratio is "<< _cache.getHitRatio()<<std::endl;
#endif
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
