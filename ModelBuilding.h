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
		
}LhSeq;

typedef struct LhRotatedTemplate
{
	LhSeq ** rotTemp;	
	int	NTheta;	
	float dTheta;
	
}LhRotatedTemplate;

typedef struct LhTemplatePyramid
{
	LhRotatedTemplate ** TempLayer;	
	int Nlayer;	
	int UPPER_NTHETA;			/* NTheta of the top layer of the pyramid */	
	int LOWER_NTHETA;			/* NTheta of the lower layer of the pyramid */	
	float UPPER_DTHETA;	
	float LOWER_DTHETA;	
	CvMemStorage * storage;
	
}LhTemplatePyramid;

typedef struct LhImagePyramid
{
	CvMat ** SobelX;
	CvMat ** SobelY;
	int Nlayer;
}LhImagePyramid;

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

inline void lhIsCalculateScore(	CvMat * SobelX ,					/* Derivative of Search Image in X direction */
								CvMat * SobelY , 					/* Derivative of Search Image in Y direction */
								LhSeq * model , 					/* The template in a sequence */
								short xshift ,						/* The x coordinate the heart locates */
								short yshift , 						/* The y coordinate the heart locates */
								long long MIN_SCORE_SINGLE , 		/* minimum score should achieved, the higher it's been set, the pro is faster */
								long long MIN_SCORE_MULTIPLE,		/**/
								long long MAX_SCORE_SINGLE ,		/**/
								long long MAX_SCORE_MULTIPLE,		/**/				
								short BREAK_POINT ,					/* the separation points between safe stopping criterion and hard stopping criterion */
								long long *score , 					/* Final score , if it's been interrupted , then it's zero */
								short quadrant						/* Specify the quadrant of the angle lies */)
{
	*score = 0 ;
	
	switch( quadrant )
	{
		case 1 :
		{
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
			break;
		}
		case 2 :
		{
			for ( int ipt=0 ; ipt < BREAK_POINT ; ipt++ )
			{
				EdgePt * CharPt = CV_GET_SEQ_ELEM( EdgePt , model->pts , ipt );
		
				short col	   = -(CharPt->ycor) + xshift ;
				short row	   =  (CharPt->xcor) + yshift ;
		
				if ( row <0 || row >SobelX->height || col <0 || col >SobelX->width )
					continue;
		
				short xsob	   = *((short *)(SobelX->data.ptr + row*SobelX->step) + col );
				short ysob	   = *((short *)(SobelY->data.ptr + row*SobelY->step) + col );
		
				(*score)+= -xsob * CharPt->yder + ysob * CharPt->xder ;
		
				if ( *score < MIN_SCORE_MULTIPLE - MAX_SCORE_MULTIPLE + ipt * MAX_SCORE_SINGLE )
					return;
			}
	
			for ( int ipt = BREAK_POINT ; ipt < model->pts->total ; ipt++ )
			{
				EdgePt * CharPt = CV_GET_SEQ_ELEM( EdgePt , model->pts , ipt );
		
				short col	   = -(CharPt->ycor) + xshift ;
				short row	   =  (CharPt->xcor) + yshift ;
		
				if ( row <0 || row >SobelX->height || col <0 || col >SobelX->width )
					continue;
		
				short xsob	   = *((short *)(SobelX->data.ptr + row*SobelX->step) + col );
				short ysob	   = *((short *)(SobelY->data.ptr + row*SobelY->step) + col );
		
				(*score)+= -xsob * CharPt->yder + ysob * CharPt->xder  ;
		
				if ( *score < MIN_SCORE_SINGLE * ipt )
					return;
			}
			break;
		}
		case 3 :
		{
			for ( int ipt=0 ; ipt < BREAK_POINT ; ipt++ )
			{
				EdgePt * CharPt = CV_GET_SEQ_ELEM( EdgePt , model->pts , ipt );
		
				short col	   = -(CharPt->xcor) + xshift ;
				short row	   = -(CharPt->ycor) + yshift ;
		
				if ( row <0 || row >SobelX->height || col <0 || col >SobelX->width )
					continue;
		
				short xsob	   = *((short *)(SobelX->data.ptr + row*SobelX->step) + col );
				short ysob	   = *((short *)(SobelY->data.ptr + row*SobelY->step) + col );
		
				(*score)+= -xsob * CharPt->xder - ysob * CharPt->yder ;
		
				if ( *score < MIN_SCORE_MULTIPLE - MAX_SCORE_MULTIPLE + ipt * MAX_SCORE_SINGLE )
					return;
			}
	
			for ( int ipt = BREAK_POINT ; ipt < model->pts->total ; ipt++ )
			{
				EdgePt * CharPt = CV_GET_SEQ_ELEM( EdgePt , model->pts , ipt );
		
				short col	   = -(CharPt->xcor) + xshift ;
				short row	   = -(CharPt->ycor) + yshift ;
		
				if ( row <0 || row >SobelX->height || col <0 || col >SobelX->width )
					continue;
		
				short xsob	   = *((short *)(SobelX->data.ptr + row*SobelX->step) + col );
				short ysob	   = *((short *)(SobelY->data.ptr + row*SobelY->step) + col );
		
				(*score)+= -xsob * CharPt->xder - ysob * CharPt->yder ;
		
				if ( *score < MIN_SCORE_SINGLE * ipt )
					return;
			}
			break;
		}
		case 4 :
		{
			for ( int ipt=0 ; ipt < BREAK_POINT ; ipt++ )
			{
				EdgePt * CharPt = CV_GET_SEQ_ELEM( EdgePt , model->pts , ipt );
		
				short col	   =  (CharPt->ycor) + xshift ;
				short row	   = -(CharPt->xcor) + yshift ;
		
				if ( row <0 || row >SobelX->height || col <0 || col >SobelX->width )
					continue;
		
				short xsob	   = *((short *)(SobelX->data.ptr + row*SobelX->step) + col );
				short ysob	   = *((short *)(SobelY->data.ptr + row*SobelY->step) + col );
		
				(*score)+= xsob * CharPt->yder - ysob * CharPt->xder ;
		
				if ( *score < MIN_SCORE_MULTIPLE - MAX_SCORE_MULTIPLE + ipt * MAX_SCORE_SINGLE )
					return;
			}
	
			for ( int ipt = BREAK_POINT ; ipt < model->pts->total ; ipt++ )
			{
				EdgePt * CharPt = CV_GET_SEQ_ELEM( EdgePt , model->pts , ipt );
		
				short col	   =  (CharPt->ycor) + xshift ;
				short row	   = -(CharPt->xcor) + yshift ;
		
				if ( row <0 || row >SobelX->height || col <0 || col >SobelX->width )
					continue;
		
				short xsob	   = *((short *)(SobelX->data.ptr + row*SobelX->step) + col );
				short ysob	   = *((short *)(SobelY->data.ptr + row*SobelY->step) + col );
		
				(*score)+= xsob * CharPt->yder - ysob * CharPt->xder ;
		
				if ( *score < MIN_SCORE_SINGLE * ipt )
					return;
			}
			break;
		}
		default:
			break;
	}
	
	return;

}

int lhComputeAveDistanceFromReferencePt( const LhSeq * srcSeq );

void lhComputeTopLayerDeltaTheta(	const LhSeq * srcSeq ,	/* image for computing the parameters */
									int Nlayer ,		/* the number of pyramid layer */
									float * dTheta ,	/* the computed angle interval in the top layer */
									int * NTheta )		/* the number of angles in the top layer */;

void lhComputeTheresholdsBasedOnMinContract(	short min_contrast ,	/* the minimum contrast the user input */
												double * threshold1 ,	/* the first parameter of cvCanny */
												double * threshold2 	/* the second threshold of cvCanny */);

LhSeq* lhCreateSingleSeqFromImage(	IplImage *imgSrc , 		/* Source image */
									CvMemStorage * storage,	/* the storage the sequence will be preserved */
									short MIN_CONTRAST,		/* the minimum contrast to specify the parameters in cvCanny */
									int MAX_PT_NUMBER)		/* Max number of EdgePt in the sequence */;
/*< Get a single sequence to characterize the feature pts of a image >*/

LhSeq* lhRotateSingleSeq(	const LhSeq *srcSeq		/* source sequence */,
							CvMemStorage * storage,	/* storage */
							float theta				/* rotating angle */,
							float scale=1.			/* rotating scale */);

LhSeq* lhRefineSeqUsingRandomDelete(	const LhSeq * srcSeq ,			/* Source Sequence */ 
										CvMemStorage * storage ,	/* storage */
										float ratio 				/* the retain ratio , the smaller , the quicker of the pro.*/);

void lhSetReferencePoint(	LhSeq * srcSeq ,	/* original search image */
							CvPoint newValue 	/* the new value of reference point */);
							
void lhSeqRegularization( LhSeq * srcSeq );
/*< Regularize the derivative of srcSeq points to the one with Amplitude SHRT_MAX =32767 >*/

void lhSearchImageRegularization(	CvMat * SobelX ,	/* Derivative in X direction */
									CvMat * SobelY ,	/* Derivative in Y direction */
									short threshold		/* Threshold to eleminate low value direvatives */);

Lh3DCor lhFindMaxPoint3D(	long long ***score,	/* score volume */
							int NX	,		/* fastest dimension */
							int NY	,		/* second dimension */
							int NT			/* slowest dimension */);
							
LhRotatedTemplate * lhBuildingRotatedTemplateFromImage
				(	const LhSeq * srcSeq ,		/* sequence for the original image */
					CvMemStorage * storage,	/* storage space for rotated template */
					float dTheta	)		/* search interval of the angle */;							
							
LhTemplatePyramid * lhBuildingTemplatePyramidFromImage(
							IplImage * srcImg ,			/* the template */
							CvMemStorage * storage ,	/* storage to store the rotated template pyramid */
							int Nlayer ,				/* number of layers of the pyramid */
							short MIN_CONTRAST ,		/* minimum contrast set by the users */
							int * MAX_PT_NUMBER 		/* number of EdgePoints for each layer */);

void lhFreeTemplatePyramid( LhTemplatePyramid * TempPyramid );

LhImagePyramid * lhBuildingImagePyramidFromImage(	IplImage * srcImg ,
													short derivative_threshold_for_cc ,	/* threshold for calculating the cross correlation between image and temp */
													int Nlayer);

							
Lh3DCor lhFindCoordinateBasedOnUpPyramid( 	LhTemplatePyramid * TempPyr ,
											LhImagePyramid * ImgPyr ,
											float min_score ,
											float break_point ,
											short sRange );



#endif /*MODELBUILDING_H_*/
