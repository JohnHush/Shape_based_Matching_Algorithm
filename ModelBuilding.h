#ifndef MODELBUILDING_H_
#define MODELBUILDING_H_

typedef struct Lh3DCor
{
	int XLocation;
	int YLocation;
	int TLocation;//The theta location
	
}Lh3DCor;

typedef struct LhSeq
{
	CvSeq * pts;
	/* 
	 * The x and y coordinates are compared to the heart points which
	 * is faciliated to the forecoming processing
	 */
	CvPoint RefPt;	// The Reference Point for calculating the cross-correlation.
	
	int AveDis;		// Average Distance of the pts from RefPt.
		
}LhSeq;

typedef struct LhRotatedTemplate
{
	LhSeq ** rotTemp;
	
	int	NTheta;
	
	float dTheta;
	
}LhRotatedTemplate;

typedef struct EdgePt
{
	short xcor;
	short ycor;	
	short xder;
	short yder;
		
}EdgePt;

inline EdgePt* edgePt( short xcor , short ycor , short xder , short yder )
{
	EdgePt *pt = (EdgePt *)malloc(sizeof( EdgePt ));
	
	pt->xcor  = xcor;
	pt->ycor  = ycor;
	pt->xder  = xder;
	pt->yder  = yder;
	
	return pt;
}
void lhFreeTemplate( LhRotatedTemplate * RotTemp );

Lh3DCor lhFindCoordinateBasedOnUpPyramid(	LhRotatedTemplate * Template ,		/**/
											IplImage * imgSearch ,				/**/
											short xshift ,						/**/
											short yshift ,						/**/
											short xrange ,						/**/
											short yrange ,						/**/
											short tshift ,						/**/
											short trange ,						/**/
											float min_score ,					/**/
											float break_point ,					/**/
											short threshold_cross_correlation);

LhRotatedTemplate * lhBuildingRotatedTemplateFromImage(	IplImage * imgSrc ,		/**/
											int NPoint , 			/**/
											int NTheta , 			/**/
											float dTheta , 			/**/
											CvPoint RefPt,			/**/
											int flag,				/**/
											double threshold1 ,		/**/
											double threshold2 ,		/**/
											int aperture_size		/**/);

LhSeq* lhCreateSingleSeqFromImage(	IplImage *imgSrc , 		/* Source image */
									CvPoint RefPt,			/**/
									int flag ,				/**/
									double threshold1 , 	/* threshold 1 for canny detector */
									double threshold2 , 	/* threshold 2 for canny detector */
									int aperture_size )		/* parameter for both canny and Sobel */;
/*< Get a single sequence to characterize the feature pts of a image >*/

LhSeq* lhRotateSingleSeq(	const LhSeq *srcSeq	/*source sequence */,
							float theta			/*rotating angle*/,
							float scale=1.0		/*rotating scale*/);

LhSeq* lhRefineSeqUsingRandomDelete(	LhSeq * srcSeq ,	/* Source Sequence */ 
										float ratio 		/* the retain ratio , the smaller , the quicker of the pro.*/);

void lhSetReferencePoint( LhSeq * srcSeq , CvPoint newValue );

Lh3DCor lhFindMaxPoint3D(	long long ***score,	/* score volume */
							int NX	,		/* fastest dimension */
							int NY	,		/* second dimension */
							int NT			/* slowest dimension */);

void lhSeqRegularization( LhSeq * srcSeq );
/*< Regularize the derivative of srcSeq points to the one with Amplitude SHRT_MAX =32767 >*/

void lhSearchImageRegularization(	CvMat * SobelX ,	/* Derivative in X direction */
									CvMat * SobelY ,	/* Derivative in Y direction */
									short threshold		/* Threshold to eleminate low value direvatives */);

inline void lhIsCalculateScore(	CvMat * SobelX ,			/* Derivative of Search Image in X direction */
								CvMat * SobelY , 			/* Derivative of Search Image in Y direction */
								LhSeq * model , 			/* The template in a sequence */
								short xshift ,				/* The x coordinate the heart locates */
								short yshift , 				/* The y coordinate the heart locates */
								long long MIN_SCORE_SINGLE , 		/* minimum score should achieved, the higher it's been set, the pro is faster */
								long long MIN_SCORE_MULTIPLE,		/**/
								long long MAX_SCORE_SINGLE ,		/**/
								long long MAX_SCORE_MULTIPLE,		/**/				
								short BREAK_POINT ,			/* the separation points between safe stopping criterion and hard stopping criterion */
								long long *score 					/* Final score , if it's been interrupted , then it's zero */)
{
	*score = 0 ;
	
	for ( int ipt=0 ; ipt < BREAK_POINT ; ipt++ )
	{
		EdgePt * CharPt = CV_GET_SEQ_ELEM( EdgePt , model->pts , ipt );
		
		short col	   = (CharPt->xcor) + xshift ;
		short row	   = (CharPt->ycor) + yshift ;
		
		if ( row <0 || row >SobelX->height || col <0 || col >SobelX->width )
			continue;
		
		short xsob	   = *((short *)(SobelX->data.ptr + row*SobelX->step) + col );
		short ysob	   = *((short *)(SobelY->data.ptr + row*SobelY->step) + col );
		
		(*score)+= xsob * CharPt->xder + ysob * CharPt->yder ;
		
		if ( *score < MIN_SCORE_MULTIPLE - MAX_SCORE_MULTIPLE + ipt * MAX_SCORE_SINGLE )
			return;
	}
	
	for ( int ipt = BREAK_POINT ; ipt < model->pts->total ; ipt++ )
	{
		EdgePt * CharPt = CV_GET_SEQ_ELEM( EdgePt , model->pts , ipt );
		
		short col	   = (CharPt->xcor) + xshift ;
		short row	   = (CharPt->ycor) + yshift ;
		
		if ( row <0 || row >SobelX->height || col <0 || col >SobelX->width )
			continue;
		
		short xsob	   = *((short *)(SobelX->data.ptr + row*SobelX->step) + col );
		short ysob	   = *((short *)(SobelY->data.ptr + row*SobelY->step) + col );
		
		(*score)+= xsob * CharPt->xder + ysob * CharPt->yder ;
		
		if ( *score < MIN_SCORE_SINGLE * ipt )
			return;
	}
	return;
}

#endif /*MODELBUILDING_H_*/
