#include "cv.h"
#include "cxcore.h"
#include "beauty_gray.h"
#include "stdio.h"
#include "ModelBuilding.h"
#include "su_alloc.h"

#define IMG_W 1024
#define IMG_H 768

int main()
{
	int Nlayer = 7;
	short MIN_CONTRAST = 40;
	int MAX_PT_NUMBER[7] = {500 , 400 , 400 , 300 , 300 , 200 , 32};
	short derivative_threshold_for_cc = 20;
	CvMemStorage * storage = cvCreateMemStorage(32000000);
	
	IplImage * imgSrc = cvCreateImage( cvSize( IMG_W , IMG_H ) , IPL_DEPTH_8U , 1 );
	
	memcpy( imgSrc->imageData , beauty_gray , IMG_W*IMG_H*sizeof( uchar ) );
		
	LhTemplatePyramid * TempPyr = lhBuildingTemplatePyramidFromImage(\
							imgSrc ,			/* the template */\
							storage ,	/* storage to store the rotated template pyramid */\
							Nlayer ,				/* number of layers of the pyramid */\
							MIN_CONTRAST ,		/* minimum contrast set by the users */\
							MAX_PT_NUMBER 		/* number of EdgePoints for each layer */);
	
	LhImagePyramid * ImgPyr = lhBuildingImagePyramidFromImage(	imgSrc ,
													derivative_threshold_for_cc ,
													Nlayer);
													
	Lh3DCor MaxPt = lhFindCoordinateBasedOnUpPyramid(  TempPyr ,\
											ImgPyr ,\
											0.7  ,/*minimum score*/\
											0.3 ,/* breakpoint*/\
											3 /* srange */);
	
	
	printf( "XLocation = %d\t YLocation = %d\t TLocation = %d\n" , MaxPt.XLocation , MaxPt.YLocation , MaxPt.TLocation );
																							
	cvReleaseImage( &imgSrc );
	cvReleaseMemStorage( &storage );
	lhFreeTemplatePyramid( TempPyr );
	lhFreeImagePyramid( ImgPyr );
	
	return(0);
}


