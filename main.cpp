#include "cv.h"
#include "cxcore.h"
//#include "beauty_gray.h"
#include "w_3662_01.h"
#include "w_3662_01r2.h"
#include "stdio.h"
#include "ModelBuilding.h"
#include "su_alloc.h"

#define IMG_W 640
#define IMG_H 480

int main()
{
	int Nlayer = 5 ;
	short MIN_CONTRAST = 40 ;
	int MAX_PT_NUMBER[7] = {5000 , 400 , 400 , 300 , 200 };
	short derivative_threshold_for_cc = 20;
	CvMemStorage * storage = cvCreateMemStorage(32000000);
	
	IplImage * imgSrc    = cvCreateImage( cvSize( IMG_W , IMG_H ) , IPL_DEPTH_8U , 1 );
	IplImage * imgSearch = cvCreateImage( cvSize( IMG_W , IMG_H ) , IPL_DEPTH_8U , 1 );
	
	memcpy( imgSrc->imageData    , w_3662_01   , IMG_W*IMG_H*sizeof( uchar ) );
	memcpy( imgSearch->imageData , w_3662_01r2 , IMG_W*IMG_H*sizeof( uchar ) );
	
	
	LhImagePyramid * ImgPyr = lhBuildingImagePyramidFromImage(	imgSearch ,
													derivative_threshold_for_cc ,
													 Nlayer);
													 
													 	
	LhTemplatePyramid * TempPyr = lhBuildingTemplatePyramidFromImage(\
							imgSrc ,			/* the template */\
							storage ,	/* storage to store the rotated template pyramid */\
							Nlayer ,				/* number of layers of the pyramid */\
							MIN_CONTRAST ,		/* minimum contrast set by the users */\
							MAX_PT_NUMBER 		/* number of EdgePoints for each layer */);
	
	
													
	Lh3DCor MaxPt = lhFindCoordinateBasedOnUpPyramid(  TempPyr ,\
											ImgPyr ,\
											0.7  ,/*minimum score*/\
											0.3 ,/* breakpoint*/\
											3 /* srange */);
	
	
	printf( "XLocation = %d\t YLocation = %d\t TLocation = %d\n" , MaxPt.XLocation , MaxPt.YLocation , MaxPt.TLocation );
	
	LhSeq* drawingSeq= lhRotateSingleSeq(	TempPyr->TempLayer[0]->rotTemp[0]		/* source sequence */,
							storage,	/* storage */
							MaxPt.TLocation * TempPyr->TempLayer[0]->dTheta				/* rotating angle */
							);	
	lhDrawModelOnImage( 	imgSearch ,
							drawingSeq ,
							MaxPt.XLocation , 
							MaxPt.YLocation );
																												
	cvReleaseImage( &imgSrc );
	cvReleaseMemStorage( &storage );
	lhFreeTemplatePyramid( TempPyr );
	lhFreeImagePyramid( ImgPyr );
	
	return(0);
}


