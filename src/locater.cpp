#include "locater.h"

Locater::Locater(const Mat &in)
{
	mat = in.clone();
	cplate_vec.reserve(16);
}

void Locater::setDisplay(string save_name)
{
	this->display = true;
	this->save_name = save_name;
}

void Locater::mserCharLocated()
{
	
	Mat gray_mat;
	cvtColor(mat, gray_mat, CV_BGR2GRAY);
	
	vector<vector<Point> > mser_blue_contour_vec;
	mser_blue_contour_vec.reserve(1024);

	vector<vector<Point> > mser_yellow_contour_vec;
	mser_yellow_contour_vec.reserve(1024);

	vector<Rect> mser_blue_rect_vec;
	mser_blue_rect_vec.reserve(1024);

	vector<Rect> mser_yellow_rect_vec;
	mser_yellow_rect_vec.reserve(1024);

	Mat formser_mat = gray_mat.clone();

	const int mser_delta = 1;
	const int mser_area = formser_mat.rows * formser_mat.cols; 
	const int mser_min_area = 30;
	const int mser_max_area = int(mser_area * 0.05);
	
	Ptr<MSER2> mser;
	mser = MSER2::create(mser_delta, mser_min_area, mser_max_area);
	mser->detectRegions(formser_mat, mser_blue_contour_vec, mser_blue_rect_vec, mser_yellow_contour_vec, mser_yellow_rect_vec);

	vector<vector<vector<Point> > > contour_vec_vec;
	contour_vec_vec.push_back(mser_blue_contour_vec);
	contour_vec_vec.push_back(mser_yellow_contour_vec);

	vector<vector<Rect> > rect_vec_vec;
	rect_vec_vec.push_back(mser_blue_rect_vec);
	rect_vec_vec.push_back(mser_yellow_rect_vec);

	vector<Color> color_vec;
	color_vec.push_back(BLUE);
	color_vec.push_back(YELLOW);

	for (int color_index = 0; color_index < color_vec.size(); color_index++)
	{
		Mat mask_mat = Mat::zeros(gray_mat.rows,gray_mat.cols,CV_8UC1);
		Mat fordisplay_mat;
		if(display)
		fordisplay_mat = mat.clone();

		vector<CChar> cchar_vec;
		cchar_vec.reserve(128);

		for (int i = 0; i < contour_vec_vec.at(color_index).size(); i++)
		{
			Rect mser_rect = rect_vec_vec.at(color_index)[i];
			vector<Point> &mser_contour = contour_vec_vec.at(color_index)[i];

			if (verifyCharSizes(mser_rect))
			{
				Mat cchar_mat = paintImageByPoints(mser_contour, mser_rect);
				Rect cchar_rect = mser_rect;

				cchar_mat = preprocessChar(cchar_mat, kCharSize);

				Point cchar_center_point(cchar_rect.tl().x + cchar_rect.width / 2, cchar_rect.tl().y + cchar_rect.height / 2);

				Mat temp_mat;
				float cchar_otsu_level = threshold(gray_mat(cchar_rect), temp_mat, 0, 255, CV_THRESH_BINARY | THRESH_OTSU);

				if (judgeMserCharDiffRatio(gray_mat, cchar_rect))
				{
					CChar cchar;
					cchar.setRect(cchar_rect);
					cchar.setMat(cchar_mat);
					cchar.setOtsuLevel(cchar_otsu_level);
					cchar.setCenterPoint(cchar_center_point);
					cchar_vec.push_back(cchar);
				}
			}
		}

		CharIdentifier::getInstance()->classify(cchar_vec);

		notMaxSuppression(cchar_vec, 0.6);

		vector<CChar> vp_cchar_vec;
		vp_cchar_vec.reserve(128);

		for (int i = 0; i < cchar_vec.size(); i++)
		{
			CChar cchar = cchar_vec.at(i);
			if (cchar.getIsStrong())
				vp_cchar_vec.push_back(cchar);
		}

		notMaxSuppression(vp_cchar_vec, 0.3f);

		vector<vector<CChar> > cchar_group_vec;
		mergeCharToGroup(vp_cchar_vec, cchar_group_vec);

		vector<CPlate> cplate_vec;
		cplate_vec.reserve(16);

		for (int i = 0; i < cchar_group_vec.size(); i++)
		{
			vector<CChar> rro_cchar_group;
			rro_cchar_group.reserve(16);
	
			removeRightOutliers(cchar_group_vec.at(i), rro_cchar_group, 0.3f, 0.5f);
			removeContainChar(rro_cchar_group, 0.1f);
			if(rro_cchar_group.size() > kPlateMaxCharNum)
			{
				int n = rro_cchar_group.size();
				for(int j = 0; j < n - kPlateMaxCharNum; j++)
					rro_cchar_group.erase(rro_cchar_group.end());
			}
	
			vector<Point> cchar_center_point_vec;
			cchar_center_point_vec.reserve(16);

			vector<CChar> mser_cchar_vec;
			mser_cchar_vec.reserve(16);

			float cchar_otsu_level_sum = 0.f;
			int cchar_max_area = 0;
			Rect cplate_rect = cchar_group_vec.at(i)[0].getRect();
			Point cplate_left_point(gray_mat.cols, 0);
			Point cplate_right_point(0, 0);
			Vec4f cplate_line_vec4f;
			Rect cplate_max_cchar_rect;

			for (int j = 0; j < rro_cchar_group.size(); j++)
			{
				CChar cchar = rro_cchar_group[j];
				Rect cchar_rect = cchar.getRect();

				cplate_rect |= cchar_rect;
				cchar_center_point_vec.push_back(cchar.getCenterPoint());

				mask_mat(cchar_rect) = 255;

				mser_cchar_vec.push_back(cchar);
				cchar_otsu_level_sum += cchar.getOtsuLevel();

				if (cchar_rect.area() > cchar_max_area)
				{
					cplate_max_cchar_rect = cchar_rect;
					cchar_max_area = cchar_rect.area();
				}

				if (cchar.getCenterPoint().x < cplate_left_point.x)
					cplate_left_point = cchar.getCenterPoint();
				if (cchar.getCenterPoint().x > cplate_right_point.x)
					cplate_right_point = cchar.getCenterPoint();
			}

			float cplate_otsu_level = cchar_otsu_level_sum / rro_cchar_group.size();
			float cplate_max_cchar_rect_ratio = float(cplate_max_cchar_rect.width) / float(cplate_max_cchar_rect.height);

			if (cchar_center_point_vec.size() >= 2 && cplate_max_cchar_rect_ratio >= 0.3)
			{
				fitLine(Mat(cchar_center_point_vec), cplate_line_vec4f, CV_DIST_L2, 0, 0.01, 0.01);

				Vec2i cplate_dist_vec2i;
				vector<Vec2i> dist_vec2i_vec;
				dist_vec2i_vec.reserve(16);

				sort(mser_cchar_vec.begin(), mser_cchar_vec.end(), compareCCharByCenterX);

				for (int j = 0; j + 1 < mser_cchar_vec.size(); j++)
				{
					Rect cchar_rect1 = mser_cchar_vec.at(j).getRect();
					Rect cchar_rect2 = mser_cchar_vec.at(j + 1).getRect();

					Vec2i dist_vec2i(cchar_rect2.x - cchar_rect1.x, cchar_rect2.y - cchar_rect2.y);
					dist_vec2i_vec.push_back(dist_vec2i);
				}

				sort(dist_vec2i_vec.begin(), dist_vec2i_vec.end(), compareVec2iByX);
				cplate_dist_vec2i = dist_vec2i_vec.at(int(float(dist_vec2i_vec.size() - 1) / 2.f));

				CPlate cplate;
				cplate.setLeftPoint(cplate_left_point);
				cplate.setRightPoint(cplate_right_point);
				cplate.setLineVec4f(cplate_line_vec4f);
				cplate.setDistVec2i(cplate_dist_vec2i);
				cplate.setOtsuLevel(cplate_otsu_level);
				cplate.setRect(cplate_rect);
				cplate.setMaxCCharRect(cplate_max_cchar_rect);
				cplate.setMserCCharVec(mser_cchar_vec);
				cplate.setColor(Color(color_index));
				cplate_vec.push_back(cplate);
			}
		}

		for (int i = 0; i < cplate_vec.size(); i++)
		{
			CPlate &cplate = cplate_vec.at(i);
			Vec2i cplate_dist_vec2i = cplate.getDistVec2i();
			vector<CChar> mser_cchar_vec = cplate.getCopyOfMserCCharVec();
			Vec4f cplate_line_vec4f = cplate.getLineVec4f();
			Point cplate_left_point = cplate.getLeftPoint();
			Point cplate_right_point = cplate.getRightPoint();
			Rect cplate_max_cchar_rect = cplate.getMaxCCharRect();
			Rect cplate_rect = cplate.getRect();
			float cplate_otsu_level = cplate.getOtsuLevel();

			const int LEFT = 0;
			const int RIGHT = 1;

			vector<CChar> left_axes_cchar_vec;
			left_axes_cchar_vec.reserve(16);

			vector<CChar> right_axes_cchar_vec;
			right_axes_cchar_vec.reserve(16);

			if (mser_cchar_vec.size() < kPlateMaxCharNum)
			{
				
				axesSearch(cplate_line_vec4f, cplate_left_point, cplate_right_point, cplate_max_cchar_rect, cplate_rect, cchar_vec, left_axes_cchar_vec, 0.2f, LEFT);
				for (int j = 0; j < left_axes_cchar_vec.size(); j++)
				{
					mser_cchar_vec.push_back(left_axes_cchar_vec[j]);
					mask_mat(left_axes_cchar_vec[j].getRect()) = 255;
				}
				if(mser_cchar_vec.size() < kPlateMaxCharNum)
				{
					axesSearch(cplate_line_vec4f, cplate_left_point, cplate_right_point, cplate_max_cchar_rect, cplate_rect, cchar_vec, right_axes_cchar_vec, 0.2f, RIGHT);
					for (int j = 0; j < right_axes_cchar_vec.size(); j++)
					{
						mser_cchar_vec.push_back(right_axes_cchar_vec[j]);
						mask_mat(right_axes_cchar_vec[j].getRect()) = 255;
					}
				}
			}
	
			combineRect(gray_mat, mser_cchar_vec, mser_cchar_vec, cplate_dist_vec2i, cplate_max_cchar_rect, 0.3f, 2.5f);

			vector<CChar> left_slide_cchar_vec;
			left_slide_cchar_vec.reserve(16);

			vector<CChar> right_slide_cchar_vec;
			right_slide_cchar_vec.reserve(16);

			sort(mser_cchar_vec.begin(), mser_cchar_vec.end(), compareCCharByRectTlX);
			
			enlargeCCharRect(mat,mser_cchar_vec[0],cplate_line_vec4f,cplate_max_cchar_rect);
			cplate_rect |= mser_cchar_vec[0].getRect();

			if (mser_cchar_vec.size() < kPlateMaxCharNum)
			{
				CChar &cchar = mser_cchar_vec[0];
				Rect cchar_rect = cchar.getRect();
	
				Mat cchar_mat;
				threshold(gray_mat(cchar_rect), cchar_mat, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
				cchar_mat = preprocessChar(cchar_mat,kCharSize);
	
				cchar.setMat(cchar_mat);

				CharIdentifier::getInstance()->isChinese(cchar, 0.9f);
			

				if (!cchar.getIsChinese())
				{	
					slideWindowSearch(gray_mat, cplate_line_vec4f,cplate_left_point, cplate_right_point, cplate_max_cchar_rect, cplate_rect, left_slide_cchar_vec, cplate_otsu_level, 0.4f, 0.8f, true, LEFT);
					for (int j = 0; j < left_slide_cchar_vec.size(); j++)
					{
						mser_cchar_vec.push_back(left_slide_cchar_vec[j]);
						mask_mat(left_slide_cchar_vec[j].getRect()) = 255;
					}
				}
				
				if (mser_cchar_vec.size() < kPlateMaxCharNum)
				{
					slideWindowSearch(gray_mat, cplate_line_vec4f,cplate_left_point, cplate_right_point, cplate_max_cchar_rect, cplate_rect, right_slide_cchar_vec, cplate_otsu_level, 0.4f, 0.8f, false, RIGHT);	
					for (int j = 0; j < right_slide_cchar_vec.size(); j++)
					{
						mser_cchar_vec.push_back(right_slide_cchar_vec[j]);
						mask_mat(right_slide_cchar_vec[j].getRect()) = 255;
					}
				}	
		
			}

			float cplate_angle = atan(cplate_line_vec4f[1] / cplate_line_vec4f[0]) * 180 / float(CV_PI);
			float cplate_width_enlarge_ratio = 1.1f;
			float cplate_height_enlarge_ratio = 1.1f;

			RotatedRect rrect(Point2f(float(cplate_rect.x) + cplate_rect.width / 2.f, float(cplate_rect.y) + cplate_rect.height / 2),
							  Size2f(cplate_rect.width * cplate_width_enlarge_ratio, cplate_rect.height * cplate_height_enlarge_ratio),
							  cplate_angle);

			cplate.setRect(cplate_rect);
			cplate.setLeftPoint(cplate_left_point);
			cplate.setRightPoint(cplate_right_point);
			cplate.setColor(Color(color_index));
			cplate.setRRect(rrect);
			cplate.setMserCCharVec(mser_cchar_vec);
			cplate.setBoundRect(getSafeRect(mat,rrect));
			cplate.setMaskMat(mask_mat(cplate.getBoundRect()));
			cplate.setBoundMat(mat(cplate.getBoundRect()));

			if(display)
			{
				RECTANGLE(fordisplay_mat,mser_cchar_vec,Scalar(0,255,255));
				RECTANGLE(fordisplay_mat,left_axes_cchar_vec,Scalar(255,255,255));
				RECTANGLE(fordisplay_mat,right_axes_cchar_vec,Scalar(255,0,255));
				RECTANGLE(fordisplay_mat,left_slide_cchar_vec,Scalar(0,0,255));
				RECTANGLE(fordisplay_mat,right_slide_cchar_vec,Scalar(0,0,0));
			}
		
		}

		for (int i = 0; i < cplate_vec.size(); i++)
		{
			if (verifyPlateSizes(cplate_vec[i].getRRect()))
			{
				this->cplate_vec.push_back(cplate_vec[i]);
				if (display)
					rectangle(fordisplay_mat, cplate_vec[i].getRect(), Scalar(0, 0, 255),2);
			}
		}
		
		if(display)
			saveImage(fordisplay_mat,kLocaterDisplayPath,save_name,color_index);

	}

	preprocessToSegment();
}

void Locater::preprocessToSegment()
{
	for(int i = 0; i < cplate_vec.size(); i++)
		deskew(cplate_vec[i]);
		
	 Judger::getInstance()->judgePlate(cplate_vec,0.5f); 

	if(display)
	{
		
		for(int i = 0; i < cplate_vec.size(); i++)
		{
			if(cplate_vec[i].getScore() < 0.5f)
				continue;
			saveImage(cplate_vec[i].getMat(),kDeskewDisplayPath,save_name,i);
		}
	}
}

