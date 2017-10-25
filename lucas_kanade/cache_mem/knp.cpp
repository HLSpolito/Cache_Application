#include "knp.h"

int PO(int x, int y){
	return x + y * WIDTH;
}

int Px(int x){
#pragma HLS INLINE
	if(x >=WIDTH)
		x =  WIDTH -1;
	else if (x < 0)
		x = 0;
	return x;
}
int Py(int y){
#pragma HLS INLINE
	if(y >=HEIGHT)
		y =  HEIGHT - 1;
	else if (y < 0)
		y = 0;
	return y;
}
int P(int x, int y){
#pragma HLS INLINE
	return Py(y) * WIDTH + Px(x);
}


int get_matrix_inv(mType* G, float* G_inv)
{
	float detG = (float)G[0] * G[3] - (float)G[1] * G[2];
	if (detG <= 1.0f) { return 0; }
	float detG_inv = 1.0f / detG;
	G_inv[0] = G[3] * detG_inv;		G_inv[1] = -G[1] * detG_inv;
	G_inv[2] = -G[2] * detG_inv;	G_inv[3] = G[0] * detG_inv;
	return 1;
}
extern "C"
#ifdef CACHE
void knp(CacheM::DataType *im1p,
		CacheN::DataType *im2p,
		CacheF::DataType  *  outp
		)
{

#pragma HLS INTERFACE m_axi port=im1p bundle=gmem0
#pragma HLS INTERFACE m_axi port=im2p bundle=gmem1
#pragma HLS INTERFACE m_axi port=outp bundle=gmem2

#pragma HLS INTERFACE s_axilite port=im1p bundle=control
#pragma HLS INTERFACE s_axilite port=im2p bundle=control
#pragma HLS INTERFACE s_axilite port=outp bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

	CacheM im1(im1p);
	CacheN im2(im2p);
	CacheF out(outp);
#else
void knp(
		unsigned char * im1,
		unsigned char * im2,
		float * out
		)
{

#pragma HLS INTERFACE m_axi port=im1 offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=im2 offset=slave bundle=gmem1
#pragma HLS INTERFACE m_axi port=out offset=slave bundle=gmem2

#pragma HLS INTERFACE s_axilite port=im1 bundle=control
#pragma HLS INTERFACE s_axilite port=im2 bundle=control
#pragma HLS INTERFACE s_axilite port=out bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#endif
	for(int j=0;j<HEIGHT;j++)
		for(int i = 0; i<WIDTH;i++){
			float G_inv[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
#pragma HLS ARRAY_PARTITION variable=G_inv complete dim=1
			mType G[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
#pragma HLS ARRAY_PARTITION variable=G complete dim=1
			mType b_k[2] = { 0.0f, 0.0f };
#pragma HLS ARRAY_PARTITION variable=b_k complete dim=1
			int x;

			for (int wj = -WINDOW_SIZE; wj<=WINDOW_SIZE;wj++)
			{
				for (int wi = -WINDOW_SIZE; wi<=WINDOW_SIZE;wi++)
				{
#pragma HLS PIPELINE
					int px = Px(i+wi), py = Py(j+wj);
					int im2_val = im2[px + py * WIDTH];

					int deltaIk = im1[px + py * WIDTH] - im2_val;
					int cx=Px(i + wi +1) + py * WIDTH, dx = Px(i + wi - 1) + py * WIDTH;
					int cIx=im1[cx];

					cIx-= im1[dx];
					cIx >>= 1;
					int cy=px + Py(j + wj+1) * WIDTH, dy=px + Py(j + wj-1)*WIDTH;

					int cIy =im1[cy];
					cIy-= im1[dy];
					cIy >>=1;

					G[0] += cIx * cIx;
					G[1] += cIx * cIy;
					G[2] += cIx * cIy;
					G[3] += cIy * cIy;
					b_k[0] += deltaIk * cIx;
					b_k[1] += deltaIk * cIy;
				}
			}

			get_matrix_inv(G, G_inv);

			float fx = 0.0f, fy = 0.0f;
			fx = G_inv[0] * b_k[0] + G_inv[1] * b_k[1];
			fy = G_inv[2] * b_k[0] + G_inv[3] * b_k[1];

			out[2*(j*WIDTH+i)]=fx;
			out[2*(j*WIDTH+i)+1]=fy;
		}
	printf("hit ratio of im1:%f\n",im1.getHitRatio());
	printf("hit ratio of im2:%f\n",im2.getHitRatio());
	printf("hit ratio of out:%f\n",out.getHitRatio());
}
