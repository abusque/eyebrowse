/*
Copyright (C) 2013 Tristan Hume

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "eyeTracker.h"

EyeTracker::EyeTracker(): m_faceCascadeName("haarcascade_frontalface_alt.xml"), m_rng(12345), m_faceDetected(false), m_noFaceDuration(0), 
  m_delay(0), m_posYAverage(0), m_posXAverage(0), m_pupilIndex(0), m_minY(0), m_minX(0), m_maxY(0), m_maxX(0), m_isPupilTableFilled(false)
{
    m_skinCrCbHist = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);

    // Load the cascades
    if( !m_faceCascade.load(m_faceCascadeName) ) {
        printf("--(!)Error loading haarcascasde_frontalface_alt.xml -- See README for details \n");
        m_capture = NULL;
        return;
    }

    createCornerKernels();
    ellipse(m_skinCrCbHist, cv::Point(113, 155.6), cv::Size(23.4, 15.2),
        43.0, 0.0, 360.0, cv::Scalar(255, 255, 255), -1);

    // Read the video stream
    m_capture = cvCaptureFromCAM( -1 );

    // Start no face timer for the first time
    m_timerStart = time(NULL);
}

EyeTracker::~EyeTracker()
{
  releaseCornerKernels();
}

void EyeTracker::setDelay(int delay)
{
  m_delay = delay;
}

bool EyeTracker::isAbsent()
{
  return !m_faceDetected && m_noFaceDuration >= m_delay;
}

bool EyeTracker::isWatchingBottom()
{
  if(!m_isPupilTableFilled)
    return false;

  int height = m_maxY - m_minY;
  //std::cout << height << " - " << m_posYAverage-m_minY << " result: " << (height - ((float)m_posYAverage-m_minY))/height << std::endl;
  return (height - ((float)m_posYAverage-m_minY))/height <= 0.2f;
}

bool EyeTracker::isWatchingLeft()
{
  if(!m_isPupilTableFilled)
    return false;

  int width = m_maxX - m_minX;

  return (width - ((float)m_posXAverage-m_minX))/width >= 0.7f;
}

bool EyeTracker::isWatchingRight()
{
  if(!m_isPupilTableFilled)
    return false;

  int width = m_maxX - m_minX;

  return (width - ((float)m_posXAverage-m_minX))/width <= 0.3f;
}

void EyeTracker::computePupilAverages()
{
  m_posYAverage = 0;
  m_posXAverage = 0;

  for(int i = 0; i < NB_PUPIL_VALUES; ++i)
  {
      m_posYAverage += m_pupilYValues[i];
      m_posXAverage += m_pupilXValues[i];
  }
    
  m_posYAverage /= NB_PUPIL_VALUES;
  m_posXAverage /= NB_PUPIL_VALUES;
}

bool EyeTracker::update()
{

  if(m_capture == NULL)
  {
      printf(" --(!) No video stream!");
      return false;
  }

  cv::Mat frame = cvQueryFrame(m_capture);
  // mirror it
  cv::flip(frame, frame, 1);
  frame.copyTo(m_debugImage);

  // Apply the classifier to the frame
  if( !frame.empty() ) {
      detectAndDisplay(frame);
  }
  else {
      printf(" --(!) No captured frame -- Break!");
      return false;
  }

  return true;
}

void EyeTracker::findEyes(cv::Mat frameGray, cv::Rect face) {
  cv::Mat faceROI = frameGray(face);
  cv::Mat debugFace = faceROI;

  if (kSmoothFaceImage) {
    double sigma = kSmoothFaceFactor * face.width;
    GaussianBlur( faceROI, faceROI, cv::Size( 0, 0 ), sigma);
  }
  //-- Find eye regions and draw them
  int eye_region_width = face.width * (kEyePercentWidth/100.0);
  int eye_region_height = face.width * (kEyePercentHeight/100.0);
  int eye_region_top = face.height * (kEyePercentTop/100.0);
  cv::Rect leftEyeRegion(face.width*(kEyePercentSide/100.0),
                         eye_region_top,eye_region_width,eye_region_height);
  cv::Rect rightEyeRegion(face.width - eye_region_width - face.width*(kEyePercentSide/100.0),
                          eye_region_top,eye_region_width,eye_region_height);

  //-- Find Eye Centers
  cv::Point leftPupil = findEyeCenter(faceROI,leftEyeRegion,"Left Eye");
  cv::Point rightPupil = findEyeCenter(faceROI,rightEyeRegion,"Right Eye");
  // get corner regions
  cv::Rect leftRightCornerRegion(leftEyeRegion);
  leftRightCornerRegion.width -= leftPupil.x;
  leftRightCornerRegion.x += leftPupil.x;
  leftRightCornerRegion.height /= 2;
  leftRightCornerRegion.y += leftRightCornerRegion.height / 2;
  cv::Rect leftLeftCornerRegion(leftEyeRegion);
  leftLeftCornerRegion.width = leftPupil.x;
  leftLeftCornerRegion.height /= 2;
  leftLeftCornerRegion.y += leftLeftCornerRegion.height / 2;
  cv::Rect rightLeftCornerRegion(rightEyeRegion);
  rightLeftCornerRegion.width = rightPupil.x;
  rightLeftCornerRegion.height /= 2;
  rightLeftCornerRegion.y += rightLeftCornerRegion.height / 2;
  cv::Rect rightRightCornerRegion(rightEyeRegion);
  rightRightCornerRegion.width -= rightPupil.x;
  rightRightCornerRegion.x += rightPupil.x;
  rightRightCornerRegion.height /= 2;
  rightRightCornerRegion.y += rightRightCornerRegion.height / 2;
  rectangle(debugFace,leftRightCornerRegion,200);
  rectangle(debugFace,leftLeftCornerRegion,200);
  rectangle(debugFace,rightLeftCornerRegion,200);
  rectangle(debugFace,rightRightCornerRegion,200);
  // change eye centers to face coordinates
  rightPupil.x += rightEyeRegion.x;
  rightPupil.y += rightEyeRegion.y;
  leftPupil.x += leftEyeRegion.x;
  leftPupil.y += leftEyeRegion.y;
  // draw eye centers
  circle(debugFace, rightPupil, 3, 1234);
  circle(debugFace, leftPupil, 3, 1234);

  //-- Find Eye Corners
  // Never called, kEnableEyeCorner set to false for now
  if (kEnableEyeCorner) {
    cv::Point2f leftRightCorner = findEyeCorner(faceROI(leftRightCornerRegion), true, false);
    leftRightCorner.x += leftRightCornerRegion.x;
    leftRightCorner.y += leftRightCornerRegion.y;
    cv::Point2f leftLeftCorner = findEyeCorner(faceROI(leftLeftCornerRegion), true, true);
    leftLeftCorner.x += leftLeftCornerRegion.x;
    leftLeftCorner.y += leftLeftCornerRegion.y;
    cv::Point2f rightLeftCorner = findEyeCorner(faceROI(rightLeftCornerRegion), false, true);
    rightLeftCorner.x += rightLeftCornerRegion.x;
    rightLeftCorner.y += rightLeftCornerRegion.y;
    cv::Point2f rightRightCorner = findEyeCorner(faceROI(rightRightCornerRegion), false, false);
    rightRightCorner.x += rightRightCornerRegion.x;
    rightRightCorner.y += rightRightCornerRegion.y;
    circle(faceROI, leftRightCorner, 3, 200);
    circle(faceROI, leftLeftCorner, 3, 200);
    circle(faceROI, rightLeftCorner, 3, 200);
    circle(faceROI, rightRightCorner, 3, 200);
  }

  //Debug output
  //std::cout << "Right pupil: (" << rightPupil.x << ", " << rightPupil.y << ")" << std::endl;

  int posY = (leftPupil.y + rightPupil.y)/2;
  int posX = (leftPupil.x + rightPupil.x)/2;

  m_pupilYValues[m_pupilIndex] = posY;
  m_pupilXValues[m_pupilIndex] = posX;

  ++m_pupilIndex;  

  if(!m_isPupilTableFilled && m_pupilIndex == NB_PUPIL_VALUES)
    m_isPupilTableFilled = true;
  m_pupilIndex %= NB_PUPIL_VALUES;

  if(m_isPupilTableFilled)
  {
    computePupilAverages();

    if(m_maxY == 0)
    {
      m_maxY = m_posYAverage;
      m_minY = m_posYAverage;
    }

    if(m_maxX == 0)
    {
      m_maxX = m_posXAverage;
      m_minX = m_posXAverage;
    }

    if(m_posYAverage > m_maxY)
      m_maxY = m_posYAverage;
    else if(m_posYAverage < m_minY)
      m_minY = m_posYAverage;

    if(m_posXAverage > m_maxX)
      m_maxX = m_posXAverage;
    else if(m_posXAverage < m_minX)
      m_minX = m_posXAverage;

    std::cout << "Y : " << m_posYAverage << " " << m_maxY << " " << m_minY << std::endl;
    std::cout << "X : " << m_posXAverage << " " << m_maxX << " " << m_minX << std::endl;
  }

}


cv::Mat EyeTracker::findSkin (cv::Mat &frame) {
  cv::Mat input;
  cv::Mat output = cv::Mat(frame.rows,frame.cols, CV_8U);

  cvtColor(frame, input, CV_BGR2YCrCb);

  for (int y = 0; y < input.rows; ++y) {
    const cv::Vec3b *Mr = input.ptr<cv::Vec3b>(y);
//    uchar *Or = output.ptr<uchar>(y);
    cv::Vec3b *Or = frame.ptr<cv::Vec3b>(y);
    for (int x = 0; x < input.cols; ++x) {
      cv::Vec3b ycrcb = Mr[x];
//      Or[x] = (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0) ? 255 : 0;
      if(m_skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) == 0) {
        Or[x] = cv::Vec3b(0,0,0);
      }
    }
  }
  return output;
}

/**
 * @function detectAndDisplay
 */
void EyeTracker::detectAndDisplay( cv::Mat frame ) {
  std::vector<cv::Rect> faces;
  //cv::Mat frameGray;

  std::vector<cv::Mat> rgbChannels(3);
  cv::split(frame, rgbChannels);
  cv::Mat frameGray = rgbChannels[2];

  //cvtColor( frame, frameGray, CV_BGR2GRAY );
  //equalizeHist( frameGray, frameGray );
  //cv::pow(frameGray, CV_64F, frameGray);
  //-- Detect faces
  m_faceCascade.detectMultiScale( frameGray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150) );
//  findSkin(debugImage);

  for( int i = 0; i < faces.size(); i++ )
  {
    rectangle(m_debugImage, faces[i], 1234);
  }
  //-- Show what you got
  if (faces.size() > 0)
  {
    if(!m_faceDetected)
      m_faceDetected = true;
    findEyes(frameGray, faces[0]);
  }
  else
  {
      if(m_faceDetected)
      {
        m_faceDetected = false;
        m_timerStart = time(NULL);
      }
      else
      {
        m_noFaceDuration = difftime(time(NULL), m_timerStart);
      }
  }
}