#include "_cv.h"
#include "ModelBuilding.h"
#include "su_alloc.h"

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
											short threshold_cross_correlation)
{
	long long ***score = alloc3longlong( 2 *xrange +1, 2 *yrange +1 , 2 *trange +1 );
	
	CvMat * SobelX = cvCreateMat  ( imgSearch->height , imgSearch->width , CV_16SC1 );
	CvMat * SobelY = cvCreateMat  ( imgSearch->height , imgSearch->width , CV_16SC1 );
	
	cvSobel( imgSearch , SobelX , 1 , 0 );
	cvSobel( imgSearch , SobelY , 0 , 1 );
	
	lhSearchImageRegularization( SobelX , SobelY , threshold_cross_correlation );
	
	short BREAK_POINT = (short)( break_point * Template->rotTemp[0]->pts->total );
	
	long long MIN_SCORE_SINGLE   = (long long )( min_score * SHRT_MAX * SHRT_MAX / 1.28 );
	long long MIN_SCORE_MULTIPLE = (long long )( min_score * SHRT_MAX * SHRT_MAX / 1.28 ) * Template->rotTemp[0]->pts->total ;
	long long MAX_SCORE_SINGLE   = (long long )SHRT_MAX * SHRT_MAX;
	long long MAX_SCORE_MULTIPLE = (long long )SHRT_MAX * SHRT_MAX * Template->rotTemp[0]->pts->total ;
	
	for ( int ithe =0 ; ithe < 2 *trange +1 ; ithe++ )
	for ( int irow =0 ; irow < 2 *yrange +1 ; irow++ )
	for ( int icol =0 ; icol < 2 *xrange +1 ; icol++ )
	{
		short xcor = xshift - xrange + icol;
		short ycor = yshift - yrange + irow;
		short tcor = tshift - trange + ithe;
		
		if ( tcor < 0 )
			tcor = tcor + Template->NTheta;
		
		lhIsCalculateScore(	SobelX , SobelY , Template->rotTemp[tcor] , xcor , ycor , MIN_SCORE_SINGLE, 
							MIN_SCORE_MULTIPLE , MAX_SCORE_SINGLE, MAX_SCORE_MULTIPLE , BREAK_POINT ,&score[ithe][irow][icol] );	
	}
	
	Lh3DCor MaxPts = lhFindMaxPoint3D(	score , 2 *xrange +1 , 2 *yrange +1	, 2 *trange +1 );
	
	MaxPts.XLocation += xshift - xrange;
	MaxPts.YLocation += yshift - yrange;
	MaxPts.TLocation += tshift - trange;
	
	if ( MaxPts.TLocation < 0 )
		MaxPts.TLocation += Template->NTheta;
	if ( MaxPts.TLocation > Template->NTheta )
		MaxPts.TLocation -= Template->NTheta;
	
	free3longlong( score );
	cvReleaseMat( &SobelX );
	cvReleaseMat( &SobelY );
	
	return MaxPts;
}

void lhFreeTemplate( LhRotatedTemplate * RotTemp )
{
	for ( int iSeq=0 ; iSeq < RotTemp->NTheta ; iSeq++ )
	{
		cvReleaseMemStorage( &( RotTemp->rotTemp[iSeq]->pts->storage ) );
		cvFree( &(RotTemp->rotTemp[iSeq]) );
	}
	cvFree( &(RotTemp->rotTemp) );
	cvFree( &(RotTemp) );
}

LhRotatedTemplate * lhBuildingRotatedTemplateFromImage(	IplImage * imgSrc ,		/**/
											int NPoint , 			/**/
											int NTheta , 			/**/
											float dTheta , 			/**/
											CvPoint RefPt,			/**/
											int flag,				/**/
											double threshold1 ,		/**/
											double threshold2 ,		/**/
											int aperture_size		/**/)
{
	LhSeq* srcSeq = lhCreateSingleSeqFromImage( imgSrc , RefPt, flag , threshold1 , threshold2 , aperture_size );
	
	LhRotatedTemplate * Templates = ( LhRotatedTemplate *)cvAlloc( sizeof( LhRotatedTemplate ) );
	
	Templates->NTheta  = NTheta;
	Templates->dTheta  = dTheta;
	Templates->rotTemp = ( LhSeq ** )cvAlloc( NTheta*sizeof( LhSeq *) );
	
	float ratio = (float)NPoint / (float)srcSeq->pts->total;
	
	LhSeq * refinedSeq = lhRefineSeqUsingRandomDelete( srcSeq , ratio );
	
	lhSeqRegularization( refinedSeq );
	
	float theta = 0.;
	for ( int iTheta = 0 ; iTheta < NTheta ; iTheta++ , theta+= dTheta )	
		Templates->rotTemp[iTheta] = lhRotateSingleSeq( refinedSeq , theta , 1 );
	
	cvReleaseMemStorage( &(srcSeq->pts->storage) );
	cvReleaseMemStorage( &(refinedSeq->pts->storage) );
	cvFree( &srcSeq );
	cvFree( &refinedSeq );
	
	return Templates;
}

void lhSearchImageRegularization(	CvMat * SobelX ,	/* Derivative in X direction */
									CvMat * SobelY ,	/* Derivative in Y direction */
									short threshold		/* Threshold to eleminate low value direvatives */)
/*< regularize the amplitude of the derivatives of search image, approximately to speed up the process >*/
{
	for ( int irow =0 ; irow < SobelX->rows ; irow++ )
	{	
		short * prowX= (short *)( SobelX->data.ptr + irow*SobelX->step );
		short * prowY= (short *)( SobelY->data.ptr + irow*SobelY->step );
		
		for ( int icol =0 ; icol < SobelX->cols ; icol++ )
		{
			short Amp = abs( prowX[icol] ) + abs( prowY[icol] );
			
			if( Amp > threshold )
			{
				short ratio = SHRT_MAX/Amp;
				
				prowX[icol] = (short)( ratio * prowX[icol] );
				prowY[icol] = (short)( ratio * prowY[icol] );
			}
			else
			{
				prowX[icol] = 0;
				prowY[icol] = 0;
			}
		}
	}
}

void lhSeqRegularization( LhSeq * srcSeq )
/*< Regularize the derivative of srcSeq points to the one with Amplitude SHRT_MAX =32767 >*/
{
	for ( int isrc=0 ; isrc < srcSeq->pts->total ; isrc++ )
	{
		EdgePt * EdgePoint = CV_GET_SEQ_ELEM( EdgePt , srcSeq->pts , isrc );
		
		if ( EdgePoint->xder!=0 || EdgePoint->yder!=0 )
		{
			float ratio = 1.*SHRT_MAX/sqrt( EdgePoint->xder*EdgePoint->xder + EdgePoint->yder*EdgePoint->yder );
			
			if ( ratio * EdgePoint->xder - 0.5 > SHRT_MAX )
				EdgePoint->xder = SHRT_MAX;
			else if( ratio * EdgePoint->xder + 0.5 < SHRT_MIN )
				EdgePoint->xder = SHRT_MIN;
			else
				EdgePoint->xder = (short)(EdgePoint->xder * ratio + 0.5 * (EdgePoint->xder?1:-1) );
			
			if ( ratio * EdgePoint->yder - 0.5 > SHRT_MAX )
				EdgePoint->yder = SHRT_MAX;
			else if( ratio * EdgePoint->yder + 0.5 < SHRT_MIN )
				EdgePoint->yder = SHRT_MIN;
			else
				EdgePoint->yder = (short)(EdgePoint->yder * ratio + 0.5 * (EdgePoint->yder?1:-1));
		}
	}
}

Lh3DCor lhFindMaxPoint3D(	long long ***score,	/* score volume */
							int NX	,		/* fastest dimension */
							int NY	,		/* second dimension */
							int NT			/* slowest dimension */)
{
	Lh3DCor MaxPts;
	
	long long tmp_score = 0 ;
	
	for ( int ix=0 ; ix < NX ; ix++ )
	for ( int iy=0 ; iy < NY ; iy++ )
	for ( int it=0 ; it < NT ; it++ )
	{
		if ( tmp_score < score[it][iy][ix] )
		{
			tmp_score = score[it][iy][ix];
			
			MaxPts.XLocation = ix;
			MaxPts.YLocation = iy;
			MaxPts.TLocation = it;
		}
	}
	
	return MaxPts;
}

LhSeq* lhRotateSingleSeq(	const LhSeq *srcSeq	/*source sequence */,
							float theta			/*rotating angle*/,
							float scale		/*rotating scale*/)
/*< Rotate the sequence w.r.t the reference point >*/
{
	LhSeq * dstSeq = ( LhSeq *)cvAlloc( sizeof(LhSeq) );
	CvSeqWriter writer;
	CvMemStorage * storage = cvCreateMemStorage();
	cvStartWriteSeq( CV_SEQ_KIND_GENERIC|CV_16SC(4) , sizeof( CvSeq ) , sizeof(EdgePt) , storage , &writer );
	
	dstSeq->RefPt.x = srcSeq->RefPt.x;
	dstSeq->RefPt.y = srcSeq->RefPt.y;
	
	dstSeq->AveDis	= srcSeq->AveDis;
	
	// construct the matrix;
	float A00 = scale * cos(theta*3.1415926535/180.);
	float A01 = scale * sin(theta*3.1415926535/180.);
	float A10 = -A01;
	float A11 = A00;
	
	for ( int isrc=0 ; isrc < srcSeq->pts->total ; isrc++ )
	{
		EdgePt *srcPt   = (EdgePt *)cvGetSeqElem( srcSeq->pts , isrc );
		
		short Xcor = (short)( (srcPt->xcor * A00 + srcPt->ycor * A01) + 0.5 * ((srcPt->xcor * A00 + srcPt->ycor * A01)>=0?1:-1) );
		short Ycor = (short)( (srcPt->xcor * A10 + srcPt->ycor * A11) + 0.5 * ((srcPt->xcor * A10 + srcPt->ycor * A11)>=0?1:-1) );
		short Xder = (short)( (srcPt->xder * A00 + srcPt->yder * A01) + 0.5 * ((srcPt->xder * A00 + srcPt->yder * A01)>=0?1:-1) );
		short Yder = (short)( (srcPt->xder * A10 + srcPt->yder * A11) + 0.5 * ((srcPt->xder * A10 + srcPt->yder * A11)>=0?1:-1) );
			
		CV_WRITE_SEQ_ELEM( *(edgePt( Xcor , Ycor , Xder , Yder )) , writer );
		// The Amplitude is considered unchanged to speed up the program.
	}
	
	dstSeq->pts    = cvEndWriteSeq( &writer );
	
	return dstSeq;
}

LhSeq* lhCreateSingleSeqFromImage(	IplImage *imgSrc , 		/* Source image */
									CvPoint RefPt,			/**/
									int flag ,				/**/
									double threshold1 , 	/* threshold 1 for canny detector */
									double threshold2 , 	/* threshold 2 for canny detector */
									int aperture_size )		/* parameter for both canny and Sobel */
/*< Get a single sequence to characterize the feature pts of a image >*/
{	
	CvMemStorage * storage	= cvCreateMemStorage();
	LhSeq *PtSeq			= (LhSeq *)cvAlloc( sizeof( LhSeq ) );
	PtSeq->pts				= cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_16SC(4) , sizeof(CvSeq) , sizeof(EdgePt) , storage );
	
	CvMat * canny  = cvCreateMat( imgSrc->height , imgSrc->width , CV_8UC1  );
	CvMat * SobelX = cvCreateMat( imgSrc->height , imgSrc->width , CV_16SC1 );
	CvMat * SobelY = cvCreateMat( imgSrc->height , imgSrc->width , CV_16SC1 );
	
	cvCanny( imgSrc , canny , threshold1 , threshold2 , aperture_size );

	cvSobel( imgSrc , SobelX , 1 , 0 , aperture_size );
	cvSobel( imgSrc , SobelY , 0 , 1 , aperture_size );
	/*
	 * Using Canny detector to determine the position of the feature pts.
	 * Using Sobel operator to determine the value of derivatives in x and y direction.
	 */
	
	CvPoint RefPoint = cvPoint( 0 , 0 );
	
	for ( int irow =0 ; irow < imgSrc->height ; irow++ )
	{	
		uchar *prow = (uchar *)( canny->data.ptr  + irow*canny->step  );
		short *prowX= (short *)( SobelX->data.ptr + irow*SobelX->step );
		short *prowY= (short *)( SobelY->data.ptr + irow*SobelY->step );
		
		for ( int icol =0 ; icol < imgSrc->width ; icol++ )
		{
			if( prow[icol]!=0 )
			{
				RefPoint.x = RefPoint.x + icol ;
				RefPoint.y = RefPoint.y + irow ;
				
				cvSeqPush( PtSeq->pts , edgePt( icol , irow , prowX[icol] , prowY[icol] ) );
			}
		}
	}
	RefPoint.x = RefPoint.x / PtSeq->pts->total;
	RefPoint.y = RefPoint.y / PtSeq->pts->total;
	/* Compute the reference point's corordinates 
	 * based on the average of all sequence points
	 */
	if ( flag > 0 )
		RefPoint = RefPt;
	
	int AveDis =0 ;
	
	for ( int iEle = 0 ; iEle < PtSeq->pts->total ; iEle++ )
	{
		EdgePt *srcPt   = (EdgePt *)cvGetSeqElem( PtSeq->pts , iEle );
		
		srcPt->xcor		= srcPt->xcor - RefPoint.x;
		srcPt->ycor		= srcPt->ycor - RefPoint.y;
		
		AveDis			= AveDis + (int)(sqrt( srcPt->xcor * srcPt->xcor + srcPt->ycor * srcPt->ycor ) + 0.5 );
	}
	AveDis = AveDis / PtSeq->pts->total;
	
	PtSeq->RefPt.x			= RefPoint.x;
	PtSeq->RefPt.y			= RefPoint.y;
	
	PtSeq->AveDis			= AveDis;
	
	cvReleaseMat( &canny  );
	cvReleaseMat( &SobelX );
	cvReleaseMat( &SobelY );
	
	return PtSeq;
}

LhSeq* lhRefineSeqUsingRandomDelete(	LhSeq * srcSeq ,	/* Source Sequence */ 
										float ratio 		/* the retain ratio , the smaller , the quicker of the pro.*/)
{
	LhSeq *RefinedSeq           = (LhSeq *)cvAlloc( sizeof( LhSeq ) );
	CvMemStorage * storage		= cvCreateMemStorage();
	
	RefinedSeq->RefPt.x			= srcSeq->RefPt.x;
	RefinedSeq->RefPt.y			= srcSeq->RefPt.y;
	
	RefinedSeq->AveDis			= RefinedSeq->AveDis;
	
	RefinedSeq->pts = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_16SC(4) , sizeof(CvSeq) , sizeof(EdgePt) , storage );
	
	for ( int isrc=0 ; isrc < srcSeq->pts->total ; isrc++ )
	{
		if( rand()%100 < (int)(ratio*100.) )
		{
			EdgePt * srcPt = (EdgePt *)cvGetSeqElem( srcSeq->pts , isrc );
			
			cvSeqPush( RefinedSeq->pts , srcPt );
		}
	}
	
	return RefinedSeq;
}

void lhSetReferencePoint( LhSeq * srcSeq , CvPoint newValue )
{
	srcSeq->RefPt = newValue;
}



	
	

