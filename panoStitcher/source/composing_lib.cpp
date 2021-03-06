/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//
//M*/

#include <iostream>
#include <fstream>
#include <string>
#include "opencv2/opencv_modules.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching/detail/autocalib.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/timelapsers.hpp"
#include "opencv2/stitching/detail/camera.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/warpers.hpp"
#include "opencv2/stitching/warpers.hpp"
#include "composing_lib.h"

#define ENABLE_LOG 1
#define LOG(msg) std::cout << msg
#define LOGLN(msg) std::cout << msg << std::endl

using namespace std;
using namespace cv;
using namespace cv::detail;
//
//static void printUsage()
//{
//    cout <<
//        "Rotation model images stitcher.\n\n"
//        "stitching_detailed img1 img2 [...imgN] [flags]\n\n"
//        "Flags:\n"
//        "  --preview\n"
//        "      Run stitching in the preview mode. Works faster than usual mode,\n"
//        "      but output image will have lower resolution.\n"
//        "  --try_cuda (yes|no)\n"
//        "      Try to use CUDA. The default value is 'no'. All default values\n"
//        "      are for CPU mode.\n"
//        "\nMotion Estimation Flags:\n"
//        "  --work_megapix <float>\n"
//        "      Resolution for image registration step. The default is 0.6 Mpx.\n"
//        "  --features (surf|orb)\n"
//        "      Type of features used for images matching. The default is surf.\n"
//        "  --matcher (homography|affine)\n"
//        "      Matcher used for pairwise image matching.\n"
//        "  --estimator (homography|affine)\n"
//        "      Type of estimator used for transformation estimation.\n"
//        "  --match_conf <float>\n"
//        "      Confidence for feature matching step. The default is 0.65 for surf and 0.3 for orb.\n"
//        "  --conf_thresh <float>\n"
//        "      Threshold for two images are from the same panorama confidence.\n"
//        "      The default is 1.0.\n"
//        "  --ba (no|reproj|ray|affine)\n"
//        "      Bundle adjustment cost function. The default is ray.\n"
//        "  --ba_refine_mask (mask)\n"
//        "      Set refinement mask for bundle adjustment. It looks like 'x_xxx',\n"
//        "      where 'x' means refine respective parameter and '_' means don't\n"
//        "      refine one, and has the following format:\n"
//        "      <fx><skew><ppx><aspect><ppy>. The default mask is 'xxxxx'. If bundle\n"
//        "      adjustment doesn't support estimation of selected parameter then\n"
//        "      the respective flag is ignored.\n"
//        "  --wave_correct (no|horiz|vert)\n"
//        "      Perform wave effect correction. The default is 'horiz'.\n"
//        "  --save_graph <file_name>\n"
//        "      Save matches graph represented in DOT language to <file_name> file.\n"
//        "      Labels description: Nm is number of matches, Ni is number of inliers,\n"
//        "      C is confidence.\n"
//        "\nCompositing Flags:\n"
//        "  --warp (affine|plane|cylindrical|spherical|fisheye|stereographic|compressedPlaneA2B1|compressedPlaneA1.5B1|compressedPlanePortraitA2B1|compressedPlanePortraitA1.5B1|paniniA2B1|paniniA1.5B1|paniniPortraitA2B1|paniniPortraitA1.5B1|mercator|transverseMercator)\n"
//        "      Warp surface type. The default is 'spherical'.\n"
//        "  --seam_megapix <float>\n"
//        "      Resolution for seam estimation step. The default is 0.1 Mpx.\n"
//        "  --seam (no|voronoi|gc_color|gc_colorgrad)\n"
//        "      Seam estimation method. The default is 'gc_color'.\n"
//        "  --compose_megapix <float>\n"
//        "      Resolution for compositing step. Use -1 for original resolution.\n"
//        "      The default is -1.\n"
//        "  --expos_comp (no|gain|gain_blocks)\n"
//        "      Exposure compensation method. The default is 'gain_blocks'.\n"
//        "  --blend (no|feather|multiband)\n"
//        "      Blending method. The default is 'multiband'.\n"
//        "  --blend_strength <float>\n"
//        "      Blending strength from [0,100] range. The default is 5.\n"
//        "  --output <result_img>\n"
//        "      The default is 'result.jpg'.\n"
//        "  --timelapse (as_is|crop) \n"
//        "      Output warped images separately as frames of a time lapse movie, with 'fixed_' prepended to input file names.\n"
//        "  --rangewidth <int>\n"
//        "      uses range_width to limit number of images to match with.\n";
//}


// Default command line args
vector<String> img_names;
bool preview = false;
bool try_cuda = false;
double work_megapix = 0.6;
double seam_megapix = 0.08;
double compose_megapix = -1;
float conf_thresh = 1.f;
string features_type = "surf";
string matcher_type = "homography";
string estimator_type = "homography";
string ba_cost_func = "ray";
string ba_refine_mask = "xxxxx";
bool do_wave_correct = true;
WaveCorrectKind wave_correct = detail::WAVE_CORRECT_HORIZ;
bool save_graph = false;
std::string save_graph_to;
string warp_type = "cylindrical";
int expos_comp_type = ExposureCompensator::GAIN_BLOCKS;
float match_conf = 0.3f;
string seam_find_type = "dp_colorgrad";
int blend_type = Blender::MULTI_BAND;
int timelapse_type = Timelapser::AS_IS;
float blend_strength = 5;
string result_name = "result.jpg";
bool timelapse = false;
int range_width = -1;



PANOCOMPOSER_EXP int ComposePanorama(float** Rmatrices, float** Kmatrices, void** inputImages, int numOfImages, int byteType, int imageHeight, int imageWidth, int numChannels)
{
	// TODO - Break ComposePanoram into (1) Prepare - calculate the size of the output based on Transforms (2) Process - actual warp and compose and put the output to a user provided BUFFER
	// TODO - populate the Rmatrices as vector<MAT> R and the Kmatrices as vector<MAT> K upon entry
	// set SEAM_RESIZE_VALUE
#if ENABLE_LOG
	int64 app_start_time = getTickCount();
	int64 t;
#endif

#if 0
	cv::setBreakOnError(true);
#endif


	// Check if have enough images

	if (numOfImages < 2)
	{
		LOGLN("Need more images");
		return NOT_ENOUGH_IMAGES;
	}

	double work_scale = 1, seam_scale = 1, compose_scale = 1;
	bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;

	Mat full_img, img;
	vector<Mat> images(numOfImages);
	vector<Size> full_img_sizes(numOfImages);
	double seam_work_aspect = 1;
	int CV_BYTE_TYPE = -1;

	switch (byteType)
	{
	case UINT8_GRAY:
		CV_BYTE_TYPE = CV_8U;
	case UINT8_RGB:
	case UINT8_BGR:
		CV_BYTE_TYPE = CV_8UC3;
	case UINT16_GRAY:
		CV_BYTE_TYPE = CV_16U;
	case UINT16_RGB:
	case UINT16_BGR:
		CV_BYTE_TYPE = CV_16UC3;

	}


    for (int i = 0; i < numOfImages; ++i)
    {
        images[i] = Mat(imageHeight,imageWidth,CV_BYTE_TYPE, inputImages[i]);
    }

    vector<CameraParams> cameras;

    for (size_t i = 0; i < cameras.size(); ++i)
    {
		Mat R(3, 3, CV_32F, Rmatrices[i]);
		Mat K(3, 3, CV_32F, Kmatrices[i]);
        // cameras[i].R.convertTo(R, CV_32F);
        cameras[i].R = R;
		// cameras[i].K = K; can't edit K because it is defined as constant by OPENCV
		Mat_<float> K;
		Mat Kdata(3, 3, CV_32F, Kmatrices[i]);
		K = (Mat_<float>)Kdata;
        // ("Initial camera intrinsics #" << i+1 << ":\nK:\n" << Kdata() << "\nR:\n" << cameras[i].R);
    }

    LOGLN("Warping images (auxiliary)... ");
#if ENABLE_LOG
    t = getTickCount();
#endif

    vector<Point> corners(numOfImages);
    vector<UMat> masks_warped(numOfImages);
    vector<UMat> images_warped(numOfImages);
    vector<Size> sizes(numOfImages);
    vector<UMat> masks(numOfImages);

    // Preapre images masks
    for (int i = 0; i < numOfImages; ++i)
    {
        masks[i].create(images[i].size(), CV_8U);
        masks[i].setTo(Scalar::all(255));
    }

    // Warp images and their masks

    Ptr<WarperCreator> warper_creator;
#ifdef HAVE_OPENCV_CUDAWARPING
    if (try_cuda && cuda::getCudaEnabledDeviceCount() > 0)
    {
        if (warp_type == "plane")
            warper_creator = makePtr<cv::PlaneWarperGpu>();
        else if (warp_type == "cylindrical")
            warper_creator = makePtr<cv::CylindricalWarperGpu>();
        else if (warp_type == "spherical")
            warper_creator = makePtr<cv::SphericalWarperGpu>();
    }
    else
#endif
    {
        if (warp_type == "plane")
            warper_creator = makePtr<cv::PlaneWarper>();
        else if (warp_type == "affine")
            warper_creator = makePtr<cv::AffineWarper>();
        else if (warp_type == "cylindrical")
            warper_creator = makePtr<cv::CylindricalWarper>();
        else if (warp_type == "spherical")
            warper_creator = makePtr<cv::SphericalWarper>();
        else if (warp_type == "fisheye")
            warper_creator = makePtr<cv::FisheyeWarper>();
        else if (warp_type == "stereographic")
            warper_creator = makePtr<cv::StereographicWarper>();
        else if (warp_type == "compressedPlaneA2B1")
            warper_creator = makePtr<cv::CompressedRectilinearWarper>(2.0f, 1.0f);
        else if (warp_type == "compressedPlaneA1.5B1")
            warper_creator = makePtr<cv::CompressedRectilinearWarper>(1.5f, 1.0f);
        else if (warp_type == "compressedPlanePortraitA2B1")
            warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(2.0f, 1.0f);
        else if (warp_type == "compressedPlanePortraitA1.5B1")
            warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(1.5f, 1.0f);
        else if (warp_type == "paniniA2B1")
            warper_creator = makePtr<cv::PaniniWarper>(2.0f, 1.0f);
        else if (warp_type == "paniniA1.5B1")
            warper_creator = makePtr<cv::PaniniWarper>(1.5f, 1.0f);
        else if (warp_type == "paniniPortraitA2B1")
            warper_creator = makePtr<cv::PaniniPortraitWarper>(2.0f, 1.0f);
        else if (warp_type == "paniniPortraitA1.5B1")
            warper_creator = makePtr<cv::PaniniPortraitWarper>(1.5f, 1.0f);
        else if (warp_type == "mercator")
            warper_creator = makePtr<cv::MercatorWarper>();
        else if (warp_type == "transverseMercator")
            warper_creator = makePtr<cv::TransverseMercatorWarper>();
    }

    if (!warper_creator)
    {
        cout << "Can't create the following warper '" << warp_type << "'\n";
        return 1;
    }

    Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));
    vector<UMat> images_warped_f(numOfImages);
    #pragma omp parallel for
    for (int i = 0; i < numOfImages; ++i)
    {
		Mat_<float> K;
		Mat Kdata(3, 3, CV_32F, Kmatrices[i]);
		K = (Mat_<float>)Kdata;
		// cameras[i].K().convertTo(K, CV_32F);
        float swa = (float)seam_work_aspect;
        K(0,0) *= swa; K(0,2) *= swa;
        K(1,1) *= swa; K(1,2) *= swa;

        corners[i] = warper->warp(images[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);
        sizes[i] = images_warped[i].size();
        warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]);
        images_warped[i].convertTo(images_warped_f[i], CV_32F);
    }        

    LOGLN("Warping images, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");

    Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(expos_comp_type);
    compensator->feed(corners, images_warped, masks_warped);

    Ptr<SeamFinder> seam_finder;
    if (seam_find_type == "no")
        seam_finder = makePtr<detail::NoSeamFinder>();
    else if (seam_find_type == "voronoi")
        seam_finder = makePtr<detail::VoronoiSeamFinder>();
    else if (seam_find_type == "gc_color")
    {
#ifdef HAVE_OPENCV_CUDALEGACY
        if (try_cuda && cuda::getCudaEnabledDeviceCount() > 0)
            seam_finder = makePtr<detail::GraphCutSeamFinderGpu>(GraphCutSeamFinderBase::COST_COLOR);
        else
#endif
            seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR);
    }
    else if (seam_find_type == "gc_colorgrad")
    {
#ifdef HAVE_OPENCV_CUDALEGACY
        if (try_cuda && cuda::getCudaEnabledDeviceCount() > 0)
            seam_finder = makePtr<detail::GraphCutSeamFinderGpu>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
        else
#endif
            seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
    }
    else if (seam_find_type == "dp_color")
        seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR);
    else if (seam_find_type == "dp_colorgrad")
        seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR_GRAD);
    if (!seam_finder)
    {
        cout << "Can't create the following seam finder '" << seam_find_type << "'\n";
        return 1;
    }

    seam_finder->find(images_warped_f, corners, masks_warped);

    // Release unused memory
    images.clear();
    images_warped.clear();
    images_warped_f.clear();
    masks.clear();

    LOGLN("Compositing...");
#if ENABLE_LOG
    t = getTickCount();
#endif

    Mat img_warped, img_warped_s;
    Mat dilated_mask, seam_mask, mask, mask_warped;
    Ptr<Blender> blender;
    Ptr<Timelapser> timelapser;
    //double compose_seam_aspect = 1;
    double compose_work_aspect = 1;

    for (int img_idx = 0; img_idx < numOfImages; ++img_idx)
    {
        LOGLN("Compositing image #" << img_idx+1);

        // Read image and resize it if necessary
        full_img = imread(img_names[img_idx]);
        if (!is_compose_scale_set)
        {
            if (compose_megapix > 0)
                compose_scale = min(1.0, sqrt(compose_megapix * 1e6 / full_img.size().area()));
            is_compose_scale_set = true;

            // Compute relative scales
            //compose_seam_aspect = compose_scale / seam_scale;
            compose_work_aspect = compose_scale / work_scale;

            // Update warped image scale
            warped_image_scale *= static_cast<float>(compose_work_aspect);
            warper = warper_creator->create(warped_image_scale);

            // Update corners and sizes
            for (int i = 0; i < numOfImages; ++i)
            {
                // Update intrinsics
                cameras[i].focal *= compose_work_aspect;
                cameras[i].ppx *= compose_work_aspect;
                cameras[i].ppy *= compose_work_aspect;

                // Update corner and size
                Size sz = full_img_sizes[i];
                if (std::abs(compose_scale - 1) > 1e-1)
                {
                    sz.width = cvRound(full_img_sizes[i].width * compose_scale);
                    sz.height = cvRound(full_img_sizes[i].height * compose_scale);
                }

				Mat_<float> K;
				Mat Kdata(3, 3, CV_32F, Kmatrices[i]);
				K = (Mat_<float>)Kdata;
                Rect roi = warper->warpRoi(sz, K, cameras[i].R);
                corners[i] = roi.tl();
                sizes[i] = roi.size();
            }
        }
        if (abs(compose_scale - 1) > 1e-1)
            resize(full_img, img, Size(), compose_scale, compose_scale);
        else
            img = full_img;
        full_img.release();
        Size img_size = img.size();

        Mat K;
        cameras[img_idx].K().convertTo(K, CV_32F);

        // Warp the current image
        warper->warp(img, K, cameras[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);

        // Warp the current image mask
        mask.create(img_size, CV_8U);
        mask.setTo(Scalar::all(255));
        warper->warp(mask, K, cameras[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);

        // Compensate exposure
        compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);

        img_warped.convertTo(img_warped_s, CV_16S);
        img_warped.release();
        img.release();
        mask.release();

        dilate(masks_warped[img_idx], dilated_mask, Mat());
        resize(dilated_mask, seam_mask, mask_warped.size());
        mask_warped = seam_mask & mask_warped;

        if (!blender && !timelapse)
        {
            blender = Blender::createDefault(blend_type, try_cuda);
            Size dst_sz = resultRoi(corners, sizes).size();
            float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
            if (blend_width < 1.f)
                blender = Blender::createDefault(Blender::NO, try_cuda);
            else if (blend_type == Blender::MULTI_BAND)
            {
                MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(blender.get());
                mb->setNumBands(static_cast<int>(ceil(log(blend_width)/log(2.)) - 1.));
                LOGLN("Multi-band blender, number of bands: " << mb->numBands());
            }
            else if (blend_type == Blender::FEATHER)
            {
                FeatherBlender* fb = dynamic_cast<FeatherBlender*>(blender.get());
                fb->setSharpness(1.f/blend_width);
                LOGLN("Feather blender, sharpness: " << fb->sharpness());
            }
            blender->prepare(corners, sizes);
        }
        else if (!timelapser && timelapse)
        {
            timelapser = Timelapser::createDefault(timelapse_type);
            timelapser->initialize(corners, sizes);
        }

        // Blend the current image
        if (timelapse)
        {
            timelapser->process(img_warped_s, Mat::ones(img_warped_s.size(), CV_8UC1), corners[img_idx]);
            String fixedFileName;
            size_t pos_s = String(img_names[img_idx]).find_last_of("/\\");
            if (pos_s == String::npos)
            {
                fixedFileName = "fixed_" + img_names[img_idx];
            }
            else
            {
                fixedFileName = "fixed_" + String(img_names[img_idx]).substr(pos_s + 1, String(img_names[img_idx]).length() - pos_s);
            }
            imwrite(fixedFileName, timelapser->getDst());
        }
        else
        {
            blender->feed(img_warped_s, mask_warped, corners[img_idx]);
        }
    }

    if (!timelapse)
    {
        Mat result, result_mask;
        blender->blend(result, result_mask);

        LOGLN("Compositing, time: " << ((getTickCount() - t) / getTickFrequency()) << " sec");

        imwrite(result_name, result);
    }

    LOGLN("Finished, total time: " << ((getTickCount() - app_start_time) / getTickFrequency()) << " sec");
    return 0;
}
