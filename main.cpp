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
	IplImage * imgSrc = cvCreateImage( cvSize( IMG_W   , IMG_H )    , IPL_DEPTH_8U , 1 );
	IplImage * imgPyr2= cvCreateImage( cvSize( IMG_W/2 , IMG_H/2 )  , IPL_DEPTH_8U , 1 );
	IplImage * imgPyr3= cvCreateImage( cvSize( IMG_W/4 , IMG_H/4 )  , IPL_DEPTH_8U , 1 );
	IplImage * imgPyr4= cvCreateImage( cvSize( IMG_W/8 , IMG_H/8 )  , IPL_DEPTH_8U , 1 ); //128*96
	IplImage * imgPyr5= cvCreateImage( cvSize( IMG_W/16, IMG_H/16 ) , IPL_DEPTH_8U , 1 ); //64*48
	IplImage * imgPyr6= cvCreateImage( cvSize( IMG_W/32, IMG_H/32 ) , IPL_DEPTH_8U , 1 ); //32*24
	IplImage * imgPyr7= cvCreateImage( cvSize( IMG_W/64, IMG_H/64 ) , IPL_DEPTH_8U , 1 );//16*12
	
	memcpy( imgSrc->imageData , beauty_gray , IMG_W*IMG_H*sizeof( uchar ) );
	
	cvPyrDown( imgSrc  , imgPyr2 );
	cvPyrDown( imgPyr2 , imgPyr3 );
	cvPyrDown( imgPyr3 , imgPyr4 );
	cvPyrDown( imgPyr4 , imgPyr5 );
	cvPyrDown( imgPyr5 , imgPyr6 );
	cvPyrDown( imgPyr6 , imgPyr7 );	
	
	CvPoint RefPt = cvPoint( 0 , 0 );
	LhRotatedTemplate * Templates  =lhBuildingRotatedTemplateFromImage(	imgPyr7 , 32  , 44   , 8.    , RefPt , -1 , 50 ,100 ,	3 );
	
	CvPoint RefPt2= cvPoint( 2 * Templates->rotTemp[0]->RefPt.x , 2 * Templates->rotTemp[0]->RefPt.y );
	CvPoint RefPt3= cvPoint( 4 * Templates->rotTemp[0]->RefPt.x , 4 * Templates->rotTemp[0]->RefPt.y );
	CvPoint RefPt4= cvPoint( 8 * Templates->rotTemp[0]->RefPt.x , 8 * Templates->rotTemp[0]->RefPt.y );
	CvPoint RefPt5= cvPoint( 16 * Templates->rotTemp[0]->RefPt.x , 16 * Templates->rotTemp[0]->RefPt.y );
	CvPoint RefPt6= cvPoint( 32 * Templates->rotTemp[0]->RefPt.x , 32 * Templates->rotTemp[0]->RefPt.y );
	CvPoint RefPt7= cvPoint( 64 * Templates->rotTemp[0]->RefPt.x , 64 * Templates->rotTemp[0]->RefPt.y );
	
	LhRotatedTemplate * Templates2 =lhBuildingRotatedTemplateFromImage(	imgPyr6 , 150 , 89   , 4.    , RefPt2 , 1 , 50 ,100 ,	3 );
//	LhRotatedTemplate * Templates3 =lhBuildingRotatedTemplateFromImage(	imgPyr5 , 300 , 179  , 2.    , RefPt3 , 1 , 50 ,100 ,	3 );
//	LhRotatedTemplate * Templates4 =lhBuildingRotatedTemplateFromImage(	imgPyr4 , 400 , 359  , 1.    , RefPt4 , 1 , 50 ,100 ,	3 );
//	LhRotatedTemplate * Templates5 =lhBuildingRotatedTemplateFromImage(	imgPyr3 , 400 , 719  , 0.5   , RefPt5 , 1 , 50 ,100 ,	3 );
//	LhRotatedTemplate * Templates6 =lhBuildingRotatedTemplateFromImage(	imgPyr2 , 400 , 1439 , 0.25  , RefPt6 , 1 , 50 ,100 ,	3 );
//	LhRotatedTemplate * Templates7 =lhBuildingRotatedTemplateFromImage(	imgSrc  , 400, 287  , 0.125 , RefPt7 , 1 , 50 ,100 ,	3 );

//	Lh3DCor MaxPts = lhFindCoordinateBasedOnUpPyramid(	Templates ,	imgPyr7 ,(imgPyr7->width)/2 ,(imgPyr7->height)/2 ,\
											(imgPyr7->width)/2 ,(imgPyr7->height)/2 ,	0 ,	20 ,0.7 ,0.3 , (short)(0.0002 * SHRT_MAX) );
											
//	Lh3DCor MaxPts2 = lhFindCoordinateBasedOnUpPyramid(Templates2 ,imgPyr6 , 2 * MaxPts.XLocation , 2 * MaxPts.YLocation ,\
//											3 , 3 ,	 2 * MaxPts.TLocation ,	3 ,0.7 , 0.3 , (short)(0.0002 * SHRT_MAX) );
//											
//	Lh3DCor MaxPts3 = lhFindCoordinateBasedOnUpPyramid(Templates3 ,imgPyr5 , 2 * MaxPts2.XLocation , 2 * MaxPts2.YLocation ,\
//											3 , 3 ,	 2 * MaxPts2.TLocation , 3 , 0.7 , 0.3 , (short)(0.0002 * SHRT_MAX) );
//	
//	Lh3DCor MaxPts4 = lhFindCoordinateBasedOnUpPyramid(Templates4 ,imgPyr4 , 2 * MaxPts3.XLocation , 2 * MaxPts3.YLocation ,\
//											3 , 3 ,	 2 * MaxPts3.TLocation , 3 , 0.7 , 0.3 , (short)(0.0002 * SHRT_MAX) );
//
//	Lh3DCor MaxPts5 = lhFindCoordinateBasedOnUpPyramid(Templates5 ,imgPyr3 , 2 * MaxPts4.XLocation , 2 * MaxPts4.YLocation ,\
//											3 , 3 ,	 2 * MaxPts4.TLocation , 3 , 0.7 , 0.3 , (short)(0.0002 * SHRT_MAX) );
//	
//	Lh3DCor MaxPts6 = lhFindCoordinateBasedOnUpPyramid(Templates6 ,imgPyr2 , 2 * MaxPts5.XLocation , 2 * MaxPts5.YLocation ,\
//											3 , 3 ,	 2 * MaxPts5.TLocation , 3 , 0.7 , 0.3 , (short)(0.0002 * SHRT_MAX) );
//	
//	Lh3DCor MaxPts7 = lhFindCoordinateBasedOnUpPyramid(Templates7 ,imgSrc , 2 * MaxPts6.XLocation , 2 * MaxPts6.YLocation ,\
//											3 , 3 ,	 2 * MaxPts6.TLocation , 3 , 0.7 , 0.3 , (short)(0.0002 * SHRT_MAX) );
//																														
//	printf( "Xlocation=%d\tYlocation=%d\tThetaLocation=%d\n" , MaxPts.XLocation , MaxPts.YLocation , MaxPts.TLocation );
//	printf( "2Xlocation=%d\tYlocation=%d\tThetaLocation=%d\n" , MaxPts2.XLocation , MaxPts2.YLocation , MaxPts2.TLocation );
//	printf( "3Xlocation=%d\tYlocation=%d\tThetaLocation=%d\n" , MaxPts3.XLocation , MaxPts3.YLocation , MaxPts3.TLocation );
//	printf( "4Xlocation=%d\tYlocation=%d\tThetaLocation=%d\n" , MaxPts4.XLocation , MaxPts4.YLocation , MaxPts4.TLocation );
//	printf( "5Xlocation=%d\tYlocation=%d\tThetaLocation=%d\n" , MaxPts5.XLocation , MaxPts5.YLocation , MaxPts5.TLocation );
//	printf( "6Xlocation=%d\tYlocation=%d\tThetaLocation=%d\n" , MaxPts6.XLocation , MaxPts6.YLocation , MaxPts6.TLocation );
//	printf( "7Xlocation=%d\tYlocation=%d\tThetaLocation=%d\n" , MaxPts7.XLocation , MaxPts7.YLocation , MaxPts7.TLocation );
//			
//	
	cvReleaseImage( &imgSrc );
//	cvReleaseImage( &imgPyr2 );
//	cvReleaseImage( &imgPyr3 );
//	cvReleaseImage( &imgPyr4 );
//	cvReleaseImage( &imgPyr5 );
//	cvReleaseImage( &imgPyr6 );
//	cvReleaseImage( &imgPyr7 );
//
//	lhFreeTemplate( Templates );
//	lhFreeTemplate( Templates2 );
//	lhFreeTemplate( Templates3 );
//	lhFreeTemplate( Templates4 );
//	lhFreeTemplate( Templates5 );
//	lhFreeTemplate( Templates6 );
//	lhFreeTemplate( Templates7 );
	
	return(0);
}


