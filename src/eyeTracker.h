/*
Copyright (C) 2013 Tristan Hume

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef EYE_TRACKER_H
#define EYE_TRACKER_H

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <queue>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "constants.h"
#include "findEyeCenter.h"
#include "findEyeCorner.h"

#define NB_PUPIL_VALUES 5
#define DELAY_REFRESH 30

class EyeTracker
{
public:
	EyeTracker();
	~EyeTracker();

	void setDelay(int delay);
	bool isAbsent();
	bool isScreenLocked;

	bool isWatchingBottom();
	bool isWatchingRight();
	bool isWatchingLeft();
	void computePupilAverages();

	bool update();
	void findEyes(cv::Mat frameGray, cv::Rect face);
	cv::Mat findSkin(cv::Mat &frame);
	void detectAndDisplay( cv::Mat frame );

private:
	cv::String m_faceCascadeName;
	cv::CascadeClassifier m_faceCascade;
	cv::RNG m_rng;
	cv::Mat m_debugImage;
	cv::Mat m_skinCrCbHist;
	CvCapture* m_capture;

	time_t m_timerStart;
	bool m_faceDetected;
	double m_noFaceDuration;
	int m_delay;

	time_t m_timerRefresh;

	bool m_isPupilTableFilled;
	int m_pupilIndex;
	// In Y
	int m_pupilYValues[NB_PUPIL_VALUES];
	int m_posYAverage;
	int m_maxY;
	int m_minY;
	// In X
	int m_pupilXValues[NB_PUPIL_VALUES];
	int m_posXAverage;
	int m_maxX;
	int m_minX;
};

#endif
