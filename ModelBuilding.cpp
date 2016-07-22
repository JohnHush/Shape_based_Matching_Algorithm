#include "_cv.h"
#include "ModelBuilding.h"
#include "su_alloc.h"

int lhComputeAveDistanceFromReferencePt( const LhSeq * srcSeq )
{
	int AveDis = 0 ;
	
	for ( int iPt=0 ; iPt < srcSeq->pts->total ; iPt++ )
	{
		EdgePt * pt = (EdgePt *)cvGetSeqElem( srcSeq->pts , iPt );
		
		AveDis += abs( pt->xcor );
		AveDis += abs( pt->ycor );		
		/* 
		 * compute the distance of points using approximate formulae
		 * the sqrt root is approximated by absolute adding
		 */
	}	
	AveDis /= srcSeq->pts->total;
	
	return AveDis;
}

void lhComputeTopLayerDeltaTheta(	const LhSeq * srcSeq ,	/* image for computing the parameters */
									int Nlayer ,		/* the number of pyramid layer */
									float * dTheta ,	/* the computed angle interval in the top layer */
									int * NTheta )		/* the number of angles in the top layer */
{
	int AveDis = lhComputeAveDistanceFromReferencePt( srcSeq );
	
	if ( AveDis <= 0 )
	{
		printf( " Average Distance in wrong value!\n " );
		exit(1);
	}
	
	float dTheta_lower_layer = (1./AveDis)*180./3.1415926535898;
	
	*dTheta = dTheta_lower_layer * pow( 2 , Nlayer -1 );
	
	*NTheta = (int)( 0.5 + 90. / (*dTheta));
	
	if ( *NTheta <= 0 )
	{
		printf( " Number of Theta in wrong value!\n" );
		exit(1);
	}
	
	*dTheta = 90./( *NTheta );
}

void lhComputeTheresholdsBasedOnMinContract(	short min_contrast ,	/* the minimum contrast the user input */
												double * threshold1 ,	/* the first parameter of cvCanny */
												double * threshold2 	/* the second threshold of cvCanny */)
{
	if ( min_contrast <0 || min_contrast > 255 )
	{
		printf( " minimum contrast is set out of range!\n " );
		exit(1);
	}
	/*
	 * the minimu contrast is assumed to be in the range [0,255]
	 * the higher the value , the thresholds for cvCanny should 
	 * be higher results of the derivative is comparably lager
	 */
	
	*threshold1 = 1.2 * min_contrast;
	*threshold2 = 2.4 * min_contrast;
}

LhSeq* lhRefineSeqUsingRandomDelete(	const LhSeq * srcSeq ,			/* Source Sequence */ 
										CvMemStorage * storage ,	/* storage */
										float ratio 				/* the retain ratio , the smaller , the quicker of the pro.*/)
{
	LhSeq *RefinedSeq           = (LhSeq *)cvAlloc( sizeof( LhSeq ) );
	
	RefinedSeq->RefPt.x			= srcSeq->RefPt.x;
	RefinedSeq->RefPt.y			= srcSeq->RefPt.y;
	
	RefinedSeq->pts = cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_16SC(4) , sizeof(CvSeq) , sizeof(EdgePt) , storage );
	
	for ( int isrc=0 ; isrc < srcSeq->pts->total ; isrc++ )
	{
		if( rand()%10000 <= (int)(ratio*10000.) )
		{
			EdgePt * srcPt = (EdgePt *)cvGetSeqElem( srcSeq->pts , isrc );
			
			cvSeqPush( RefinedSeq->pts , srcPt );
		}
	}
	
	return RefinedSeq;
}

LhSeq* lhCreateSingleSeqFromImage(	IplImage *imgSrc , 		/* Source image */
									CvMemStorage * storage,	/* the storage the sequence will be preserved */
									short MIN_CONTRAST,		/* the minimum contrast to specify the parameters in cvCanny */
									int MAX_PT_NUMBER)		/* Max number of EdgePt in the sequence */
/*< Get a single sequence to characterize the feature pts of an image >*/
{	
	LhSeq *PtSeq			= (LhSeq *)cvAlloc( sizeof( LhSeq ) );
	PtSeq->pts				= cvCreateSeq( CV_SEQ_KIND_GENERIC|CV_16SC(4) , sizeof(CvSeq) , sizeof(EdgePt) , storage );
	
	CvMat * canny  = cvCreateMat( imgSrc->height , imgSrc->width , CV_8UC1  );
	CvMat * SobelX = cvCreateMat( imgSrc->height , imgSrc->width , CV_16SC1 );
	CvMat * SobelY = cvCreateMat( imgSrc->height , imgSrc->width , CV_16SC1 );
	
	double threshold1 , threshold2;
	lhComputeTheresholdsBasedOnMinContract(	 MIN_CONTRAST ,	&threshold1 , &threshold2 );
	
	cvCanny( imgSrc , canny , threshold1 , threshold2 );

	cvSobel( imgSrc , SobelX , 1 , 0 );
	cvSobel( imgSrc , SobelY , 0 , 1 );
	/*
	 * Using Canny detector to determine the position of the feature pts.
	 * Using Sobel operator to determine the value of derivatives in x and y direction.
	 */
	
	CvPoint RefPoint = cvPoint( 0 , 0 );
	int count = 0;
	for ( int irow =0 ; irow < imgSrc->height ; irow++ )
	{
		uchar *prow = (uchar *)( canny->data.ptr  + irow*canny->step  );
		for ( int icol =0 ; icol < imgSrc->width ; icol++ )
		{
			if( prow[icol]!=0 )
			{
				RefPoint.x = RefPoint.x + icol ;
				RefPoint.y = RefPoint.y + irow ;
				
				count++;
			}
		}
	}
	RefPoint.x = RefPoint.x / count;
	RefPoint.y = RefPoint.y / count;
	/* Compute the reference point's corordinates 
	 * based on the average of all sequence points
	 */
	float ratio = (float)MAX_PT_NUMBER / count;
	
	for ( int irow =0 ; irow < imgSrc->height ; irow++ )
	{	
		uchar *prow = (uchar *)( canny->data.ptr  + irow*canny->step  );
		short *prowX= (short *)( SobelX->data.ptr + irow*SobelX->step );
		short *prowY= (short *)( SobelY->data.ptr + irow*SobelY->step );
		
		for ( int icol =0 ; icol < imgSrc->width ; icol++ )
			if( prow[icol]!=0 )
				if( rand()%10000 <= (int)(ratio*10000.) )
					cvSeqPush( PtSeq->pts , edgePt( icol-RefPoint.x , irow-RefPoint.y , prowX[icol] , prowY[icol] ) );
					/* 
					 * the number of the EdgePt being extracted is not a constant
					 * equals to MAX_PT_NUMBER, it's floating near this number
					 */

	}
		
	PtSeq->RefPt.x			= RefPoint.x;
	PtSeq->RefPt.y			= RefPoint.y;
	
	cvReleaseMat( &canny  );
	cvReleaseMat( &SobelX );
	cvReleaseMat( &SobelY );
	
	return PtSeq;
}

LhSeq* lhRotateSingleSeq(	const LhSeq *srcSeq		/* source sequence */,
							CvMemStorage * storage,	/* storage */
							float theta				/* rotating angle */,
							float scale				/* rotating scale */)
/*< Rotate the sequence w.r.t the reference point >*/
{
	LhSeq * dstSeq = ( LhSeq *)cvAlloc( sizeof(LhSeq) );
	CvSeqWriter writer;
	cvStartWriteSeq( CV_SEQ_KIND_GENERIC|CV_16SC(4) , sizeof( CvSeq ) , sizeof(EdgePt) , storage , &writer );
	
	dstSeq->RefPt.x = srcSeq->RefPt.x;
	dstSeq->RefPt.y = srcSeq->RefPt.y;
	
	// construct the matrix;
	float A00 = scale * cos(theta*3.1415926535898/180.);
	float A01 = scale * sin(theta*3.1415926535898/180.);
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

void lhSetReferencePoint(	LhSeq * srcSeq ,	/* original search image */
							CvPoint newValue 	/* the new value of reference point */)
{
	int xshift = newValue.x - srcSeq->RefPt.x;
	int yshift = newValue.y - srcSeq->RefPt.y;
	
	for ( int isrc=0 ; isrc < srcSeq->pts->total ; isrc++ )
	{
		EdgePt *srcPt   = (EdgePt *)cvGetSeqElem( srcSeq->pts , isrc );
		
		srcPt->xcor		= srcPt->xcor - xshift;
		srcPt->ycor		= srcPt->ycor - yshift;
	}
	
	srcSeq->RefPt = newValue;	
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

LhRotatedTemplate * lhBuildingRotatedTemplateFromImage
				(	const LhSeq * srcSeq ,		/* sequence for the original image */
					CvMemStorage * storage,	/* storage space for rotated template */
					float dTheta	)		/* search interval of the angle */

{
	int NTheta = (int)( 0.5 + 90./dTheta );
	
	LhRotatedTemplate * Templates = ( LhRotatedTemplate *)cvAlloc( sizeof( LhRotatedTemplate ) );
	
	Templates->NTheta  = NTheta;
	Templates->dTheta  = dTheta;
	Templates->rotTemp = ( LhSeq ** )cvAlloc( NTheta*sizeof( LhSeq *) );
	
	float theta = 0.;
	for ( int iTheta = 0 ; iTheta < NTheta ; iTheta++ , theta+= dTheta )	
		Templates->rotTemp[iTheta] = lhRotateSingleSeq(	srcSeq , storage , theta );
	
	return Templates;
}

LhTemplatePyramid * lhBuildingTemplatePyramidFromImage(
							IplImage * srcImg ,			/* the template */
							CvMemStorage * storage ,	/* storage to store the rotated template pyramid */
							int Nlayer ,				/* number of layers of the pyramid */
							short MIN_CONTRAST ,		/* minimum contrast set by the users */
							int * MAX_PT_NUMBER 		/* number of EdgePoints for each layer */)
{
	if ( (srcImg->width % (int)pow( 2 , Nlayer-1 ))!=0 || (srcImg->height % (int)pow( 2 , Nlayer-1 ))!=0 )
	{
		printf( "srcImg width or height are not a integer times power(2,Nlayer-1)" );
		exit(1);
	}
	CvMemStorage * storage_tmp	= cvCreateMemStorage();
	LhSeq ** unRotatedSeq		= ( LhSeq ** )cvAlloc( Nlayer*sizeof( LhSeq * ) );
	
	IplImage ** imgPyr = ( IplImage ** )cvAlloc( (Nlayer-1)*sizeof(IplImage *) );
	
	LhTemplatePyramid * pyramid = (LhTemplatePyramid *)cvAlloc(sizeof(LhTemplatePyramid));
	
	pyramid->TempLayer			= (LhRotatedTemplate **)cvAlloc( Nlayer*sizeof(LhRotatedTemplate *) );
	pyramid->Nlayer				= Nlayer;
	pyramid->storage			= storage;
	
	int count = 1;
	for ( int ilayer=0 ; ilayer < Nlayer ; ilayer++ )
	{			
		if ( ilayer == 0 )
		{
			unRotatedSeq[ilayer] = lhCreateSingleSeqFromImage( srcImg , storage_tmp ,MIN_CONTRAST ,MAX_PT_NUMBER[ilayer] );
			lhSeqRegularization( unRotatedSeq[ilayer] );
		}
		else if( ilayer ==1 )
		{
			imgPyr[ilayer-1] = cvCreateImage( cvSize( srcImg->width/2 , srcImg->height/2 )  , IPL_DEPTH_8U , 1 );
			cvPyrDown( srcImg , imgPyr[ilayer-1] );
			unRotatedSeq[ilayer] = lhCreateSingleSeqFromImage( imgPyr[ilayer-1] , storage_tmp ,MIN_CONTRAST ,MAX_PT_NUMBER[ilayer] );
			lhSeqRegularization( unRotatedSeq[ilayer] );
		}
		else
		{
			imgPyr[ilayer-1] = cvCreateImage( cvSize( srcImg->width/count , srcImg->height/count )  , IPL_DEPTH_8U , 1 );
			cvPyrDown( imgPyr[ilayer-2] , imgPyr[ilayer-1] );
			unRotatedSeq[ilayer] = lhCreateSingleSeqFromImage( imgPyr[ilayer-1] , storage_tmp ,MIN_CONTRAST ,MAX_PT_NUMBER[ilayer] );
			lhSeqRegularization( unRotatedSeq[ilayer] );
		}
		
		count = count *2;
	}
	/* 
	 * building the unrotated sequences from original image
	 * the sequences have been normalized
	 */
	if ( Nlayer > 1 )
	{
		for ( int iPyr = 0 ; iPyr < Nlayer -1 ; iPyr++ )
			cvReleaseImage( &imgPyr[iPyr] );
		cvFree( &imgPyr );
	}
	/*
	 * Free the imgPyramid
	 */
	
	float dTheta;
	int NTheta;
	
	for ( int iPyr = 0 ; iPyr < Nlayer ; iPyr++ )
	{
		count = count /2 ;
		
		lhSetReferencePoint( unRotatedSeq[iPyr] , cvPoint( count * unRotatedSeq[Nlayer-1]->RefPt.x , count * unRotatedSeq[Nlayer-1]->RefPt.y ) );	
		
		if ( iPyr == 0 )
			lhComputeTopLayerDeltaTheta( unRotatedSeq[0] , Nlayer , &dTheta , &NTheta );		
	
		pyramid->TempLayer[iPyr] = lhBuildingRotatedTemplateFromImage( unRotatedSeq[iPyr] , storage, dTheta/count );
	}
	/*
	 * set the reference point for all unrotated sequences
	 * and compute each rotated layer one by one.
	 */
	
	pyramid->UPPER_NTHETA = pyramid->TempLayer[Nlayer-1]->NTheta;
	pyramid->LOWER_NTHETA = pyramid->TempLayer[0]->NTheta;
	pyramid->UPPER_DTHETA = pyramid->TempLayer[Nlayer-1]->dTheta;
	pyramid->LOWER_DTHETA = pyramid->TempLayer[0]->dTheta;
	
	for ( int iPyr=0 ; iPyr < Nlayer ; iPyr++ )
	{
		cvFree( &unRotatedSeq[iPyr] );
	}
	cvFree( &unRotatedSeq );
	cvReleaseMemStorage( &storage_tmp );
	
	return pyramid;
}

void lhFreeTemplatePyramid( LhTemplatePyramid * TempPyramid )
{
	for ( int ilayer=0 ; ilayer < TempPyramid->Nlayer ; ilayer++ )
	{
		for ( int itheta=0 ; itheta < TempPyramid->TempLayer[ilayer]->NTheta ; itheta++ )
		{
			cvFree( &(TempPyramid->TempLayer[ilayer]->rotTemp[itheta]) );
		}
		cvFree( &(TempPyramid->TempLayer[ilayer]->rotTemp) );
		
		cvFree( &(TempPyramid->TempLayer[ilayer]) );
	}
	cvFree( &(TempPyramid->TempLayer) );
	cvReleaseMemStorage( &(TempPyramid->storage) );
}

void lhFreeImagePyramid( LhImagePyramid * imgPyramid )
{
	for ( int ilayer = 0 ; ilayer < imgPyramid->Nlayer ; ilayer++ )
	{
		cvReleaseMat( &(imgPyramid->SobelX[ilayer]) );
		cvReleaseMat( &(imgPyramid->SobelY[ilayer]) );
	}
	cvFree( &(imgPyramid->SobelX) );
	cvFree( &(imgPyramid->SobelY) );
}

LhImagePyramid * lhBuildingImagePyramidFromImage(	IplImage * srcImg ,
													short derivative_threshold_for_cc ,	/* threshold for calculating the cross correlation between image and temp */
													int Nlayer)
{
	
	if ( (srcImg->width % (int)pow( 2 , Nlayer-1 ))!=0 || (srcImg->height % (int)pow( 2 , Nlayer-1 ))!=0 )
	{
		printf( "srcImg width or height are not a integer times power(2,Nlayer-1)" );
		exit(1);
	}
	LhImagePyramid * SobelPyramid = (LhImagePyramid *)cvAlloc(sizeof(LhImagePyramid));
	
	CvMat ** SobelX = (CvMat **)cvAlloc( Nlayer*sizeof(CvMat *) );
	CvMat ** SobelY = (CvMat **)cvAlloc( Nlayer*sizeof(CvMat *) );
	
	
	IplImage ** imgPyr = ( IplImage ** )cvAlloc( (Nlayer-1)*sizeof(IplImage *) );
	
	SobelPyramid->Nlayer	= Nlayer;
	SobelPyramid->SobelX	= SobelX;
	SobelPyramid->SobelY	= SobelY;
	
	int count = 1;
	for ( int ilayer =0 ; ilayer < Nlayer ; ilayer++ )
	{
		SobelX[ilayer] = cvCreateMat  ( srcImg->height/count , srcImg->width/count , CV_16SC1 );
		SobelY[ilayer] = cvCreateMat  ( srcImg->height/count , srcImg->width/count , CV_16SC1 );
		
		if ( ilayer==0 )
		{
			cvSobel( srcImg , SobelX[0] , 1 , 0 );
			cvSobel( srcImg , SobelY[0] , 0 , 1 );
			lhSearchImageRegularization( SobelX[0] , SobelY[0] , derivative_threshold_for_cc );
		}
		else if ( ilayer==1 )
		{
			imgPyr[0] = cvCreateImage( cvSize( srcImg->width/count , srcImg->height/count )  , IPL_DEPTH_8U , 1 );
			cvPyrDown( srcImg  , imgPyr[0] );
			cvSobel( imgPyr[0] , SobelX[1] , 1 , 0 );
			cvSobel( imgPyr[0] , SobelY[1] , 0 , 1 );
			lhSearchImageRegularization( SobelX[1] , SobelY[1] , derivative_threshold_for_cc );
		}
		else
		{
			imgPyr[ilayer-1] = cvCreateImage( cvSize( srcImg->width/count , srcImg->height/count )  , IPL_DEPTH_8U , 1 );
			cvPyrDown( imgPyr[ilayer-2] , imgPyr[ilayer-1] );
			cvSobel( imgPyr[ilayer-1] , SobelX[ilayer] , 1 , 0 );
			cvSobel( imgPyr[ilayer-1] , SobelY[ilayer] , 0 , 1 );
			lhSearchImageRegularization( SobelX[ilayer] , SobelY[ilayer] , derivative_threshold_for_cc );
		}
		
		count *= 2;
	}
	
	if ( Nlayer > 1 )
	{
		for ( int ilayer =0 ; ilayer < Nlayer-1 ; ilayer++ )
			cvReleaseImage( &imgPyr[ilayer] );	
		cvFree( &imgPyr );
	}
	
	return SobelPyramid;
}

Lh3DCor lhFindCoordinateBasedOnUpPyramid( 	LhTemplatePyramid * TempPyr ,
											LhImagePyramid * ImgPyr ,
											float min_score ,
											float break_point ,
											short sRange )
{
	if ( TempPyr->Nlayer != ImgPyr->Nlayer )
	{
		printf( " The layer amount of TemplatePyramid is not Equal to the layer amount of ImagePyramid!\n " );
		exit(1);
	}
	
	int Nlayer = TempPyr->Nlayer;
	short xstart , xend , ystart , yend , tstart , tend;
	Lh3DCor MaxPts;
	
	for ( int ilayer =Nlayer-1 ; ilayer >= 0 ; ilayer-- )
	{			
		short BREAK_POINT 				= (short)( break_point * TempPyr->TempLayer[ilayer]->rotTemp[0]->pts->total );	
		long long MIN_SCORE_SINGLE		= (long long )( min_score * SHRT_MAX * SHRT_MAX / 1.28 );
		long long MIN_SCORE_MULTIPLE	= (long long )( min_score * SHRT_MAX * SHRT_MAX / 1.28 ) * TempPyr->TempLayer[ilayer]->rotTemp[0]->pts->total ;
		long long MAX_SCORE_SINGLE		= (long long ) SHRT_MAX * SHRT_MAX;
		long long MAX_SCORE_MULTIPLE	= (long long ) SHRT_MAX * SHRT_MAX * TempPyr->TempLayer[ilayer]->rotTemp[0]->pts->total ;
		
		if ( ilayer==Nlayer-1 )
		{
			xstart	= 0;
			ystart	= 0;
			tstart	= 0;
			xend	= ImgPyr->SobelX[ilayer]->cols -1 ;
			yend	= ImgPyr->SobelX[ilayer]->rows -1 ;
			tend	= 4 * TempPyr->TempLayer[ilayer]->NTheta -1 ;
		}
		else
		{
			xstart	= 2 * MaxPts.XLocation - sRange;
			ystart	= 2 * MaxPts.YLocation - sRange;
			tstart	= 2 * MaxPts.TLocation - sRange;
			xend	= 2 * MaxPts.XLocation + sRange;
			yend	= 2 * MaxPts.YLocation + sRange;
			tend	= 2 * MaxPts.TLocation + sRange;
		}
		/*
		 * compute the search range,
		 * the top layer is searched in the full range
		 */
		 
		long long ***score = alloc3longlong( xend-xstart +1 , yend-ystart +1 , tend-tstart +1 );
		zero3longlong( score , xend-xstart +1 , yend-ystart +1 , tend-tstart +1 );
		
		for ( int ithe =0 ; ithe < tend-tstart +1  ; ithe++ )
		{
			short quadrant;
			short theta_symbol;
			short theta_real = tstart + ithe;
			if ( theta_real < 0 )											theta_real += 4 * TempPyr->TempLayer[ilayer]->NTheta;
			if ( theta_real >= 4 * TempPyr->TempLayer[ilayer]->NTheta )		theta_real -= 4 * TempPyr->TempLayer[ilayer]->NTheta;
			
			if ( theta_real >=0 && theta_real < TempPyr->TempLayer[ilayer]->NTheta )
			{
				theta_symbol	= theta_real;
				quadrant 		= 1;
			}
			else if ( theta_real >= TempPyr->TempLayer[ilayer]->NTheta && theta_real < 2* TempPyr->TempLayer[ilayer]->NTheta )
			{
				theta_symbol	= theta_real - TempPyr->TempLayer[ilayer]->NTheta;
				quadrant		= 4;
			}
			else if ( theta_real >= 2 * TempPyr->TempLayer[ilayer]->NTheta && theta_real < 3* TempPyr->TempLayer[ilayer]->NTheta )
			{
				theta_symbol	= theta_real - 2* TempPyr->TempLayer[ilayer]->NTheta;
				quadrant		= 3;
			}
			else
			{
				theta_symbol	= theta_real - 3* TempPyr->TempLayer[ilayer]->NTheta;
				quadrant		= 2;
			}
			
			for ( int irow =0 ; irow < yend-ystart +1  ; irow++ )
			for ( int icol =0 ; icol < xend-xstart +1  ; icol++ )
				lhIsCalculateScore(	ImgPyr->SobelX[ilayer] , ImgPyr->SobelY[ilayer] , TempPyr->TempLayer[ilayer]->rotTemp[theta_symbol] ,\
										 xstart + icol  , ystart + irow , MIN_SCORE_SINGLE, MIN_SCORE_MULTIPLE , MAX_SCORE_SINGLE,\
			 									MAX_SCORE_MULTIPLE , BREAK_POINT ,&score[ithe][irow][icol] , quadrant );
		}
		
		MaxPts = lhFindMaxPoint3D(	score , xend-xstart +1 , yend-ystart +1 , tend-tstart +1 );
		
		MaxPts.XLocation += xstart;
		MaxPts.YLocation += ystart;
		MaxPts.TLocation += tstart;
	
		if ( MaxPts.TLocation < 0 )
			MaxPts.TLocation += 4 * TempPyr->TempLayer[ilayer]->NTheta;
		if ( MaxPts.TLocation > 4 * TempPyr->TempLayer[ilayer]->NTheta )
			MaxPts.TLocation -= 4 * TempPyr->TempLayer[ilayer]->NTheta;	
		
		free3longlong( score );
		
	}
	return MaxPts;	
}








	

