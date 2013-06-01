#include "Vision/Reconstruction/BundleAdjuster.h"

#include <opencv2/contrib/contrib.hpp>

#include <sba.h>

#include "Vision/Core/Feature.h"
#include "Vision/Core/FeatureSet.h"
#include "Vision/Core/PointOfView.h"

namespace Xu
{
    namespace Vision
    {
        namespace Reconstruction
        {
            BundleAdjuster::BundleAdjuster()
            {
            }

            BundleAdjuster::BundleAdjuster(const std::shared_ptr<Core::FeatureSet> &featureSet)
                : features(featureSet),
                  adjustedPoints(0),
                  adjustedCameras(0)
            {
            }

            void BundleAdjuster::EstimateCameraPose(std::shared_ptr<Core::PointOfView> &pointOfView)
            {
                SbaParameters params = PrepareData(MOTION_ONLY);
                params.pointsOfView.push_back(pointOfView);

                Run(0, params.pointsOfView.size() - 1, params);

//                for (int i = 0; i < features->Size(); i++)
//                {
//                    std::shared_ptr<Core::Feature> feature = features->GetFeature(i);
//                    if (feature->IsTriangulated())
//                    {
//                        pointsx.push_back(feature);
//                    }
//                }
//                int pointCount = pointsx.size();

//                std::vector<std::shared_ptr<Core::PointOfView> > pointsOfView;
//                {
//                    std::vector<std::shared_ptr<Core::PointOfView> > existingPointsOfView = features->GetPointsOfView();
//                    for (int i = 0; i < existingPointsOfView.size(); i++)
//                    {
//                        std::shared_ptr<Core::PointOfView> pointOfView = existingPointsOfView.at(i);
//                        if (pointOfView->GetCameraParameters().IsPoseDetermined())
//                        {
//                            pointsOfView.push_back(pointOfView);
//                        }
//                    }
//                }
//                int cameraCount = pointsOfView.size();


//                char *visibility = new char[pointCount * (cameraCount + 1)];
//                double *projections = new double[2 * pointCount * (cameraCount + 1)];

//                for (int i = 0; i < pointCount * (cameraCount + 1); i++)
//                {
//                    visibility[i] = 0;

//                    projections[2 * i + 0] = 0;
//                    projections[2 * i + 1] = 0;
//                }

//                for (int i = 0; i < cameraCount; i++)
//                {
//                    std::shared_ptr<Core::PointOfView> donePointOfView = pointsOfView.at(i);
//                    for (int j = 0; j < pointCount; j++)
//                    {
//                        std::shared_ptr<Core::Feature> feature = pointsx.at(j);
//                        if (feature->HasCorrespondenceInView(donePointOfView))
//                        {
//                            visibility[j * cameraCount + i] = 1;
//                            cv::Point2d point = feature->GetPointInView(donePointOfView);
//                            projections[2 * j * cameraCount + 2 * i + 0] = point.x;
//                            projections[2 * j * cameraCount + 2 * i + 1] = point.y;
//                        }
//                    }
//                }

//                for (int j = 0; j < pointCount; j++)
//                {
//                    std::shared_ptr<Core::Feature> feature = pointsx.at(j);
//                    if (feature->HasCorrespondenceInView(pointOfView))
//                    {
//                        visibility[j * cameraCount + cameraCount] = 1;
//                        cv::Point2d point = feature->GetPointInView(pointOfView);
//                        projections[2 * j * cameraCount + 2 * cameraCount + 0] = point.x;
//                        projections[2 * j * cameraCount + 2 * cameraCount + 1] = point.y;
//                    }
//                }

//                int cameraParametersCount = 6;
//                int pointParametersCount = 3;
//                int parametersCount = (cameraCount + 1) * cameraParametersCount + pointCount * pointParametersCount;

//                double *parameters = new double[parametersCount];

//                for (int i = 0; i < cameraCount; i++)
//                {
//                    std::shared_ptr<Core::PointOfView> donePointOfView = pointsOfView.at(i);
//                    cv::Mat rotationMatrix = donePointOfView->GetCameraParameters().GetRotationMatrix();
//                    cv::Mat rotation; cv::Rodrigues(rotationMatrix, rotation);
//                    cv::Mat translation = donePointOfView->GetCameraParameters().GetTranslationMatrix();

//                    parameters[i * cameraParametersCount + 0] = translation.at<double>(0);
//                    parameters[i * cameraParametersCount + 1] = translation.at<double>(1);
//                    parameters[i * cameraParametersCount + 2] = translation.at<double>(2);

//                    parameters[i * cameraParametersCount + 3] = rotation.at<double>(0);
//                    parameters[i * cameraParametersCount + 4] = rotation.at<double>(1);
//                    parameters[i * cameraParametersCount + 5] = rotation.at<double>(2);
//                }

//                // TODO rename
//                cv::Mat rotationMatrixx = pointOfView->GetCameraParameters().GetRotationMatrix();
//                cv::Mat rotationx; cv::Rodrigues(rotationMatrixx, rotationx);
//                cv::Mat translation = pointOfView->GetCameraParameters().GetTranslationMatrix();

//                parameters[cameraCount * cameraParametersCount + 0] = translation.at<double>(0);
//                parameters[cameraCount * cameraParametersCount + 1] = translation.at<double>(1);
//                parameters[cameraCount * cameraParametersCount + 2] = translation.at<double>(2);

//                parameters[cameraCount * cameraParametersCount + 3] = rotationx.at<double>(0);
//                parameters[cameraCount * cameraParametersCount + 4] = rotationx.at<double>(1);
//                parameters[cameraCount * cameraParametersCount + 5] = rotationx.at<double>(2);

//                int pointsStartIndex = (cameraCount + 1) * cameraParametersCount;

//                for (int i = 0; i < pointCount; i++)
//                {
//                    std::shared_ptr<Core::Feature> feature = pointsx.at(i);

//                    parameters[pointsStartIndex + i * pointParametersCount + 0] = feature->GetX();
//                    parameters[pointsStartIndex + i * pointParametersCount + 1] = feature->GetY();
//                    parameters[pointsStartIndex + i * pointParametersCount + 2] = feature->GetZ();
//                }

//                double options[SBA_OPTSSZ];
//                options[0] = SBA_INIT_MU;
//                options[1] = SBA_STOP_THRESH;
//                options[2] = SBA_STOP_THRESH;
//                options[3] = SBA_STOP_THRESH;
//                options[4] = 0.0;
//                double info[SBA_INFOSZ];

//                sba_mot_levmar(pointCount,
//                               cameraCount + 1,
//                               cameraCount,
//                               visibility,
//                               parameters,
//                               cameraParametersCount,
//                               projections,
//                               NULL, // covariance matrix
//                               2,
//                               ProjectPointMotionOnly,
//                               NULL, // jacobian calculation function
//                               (void *) this, // additional data
//                               150, // max iterations
//                               3, // verbosity level
//                               options,
//                               info);

//                delete[] visibility;
//                delete[] projections;

//                cv::Mat translationMatrix(3, 1, CV_64FC1);
//                translationMatrix.at<double>(0) = parameters[cameraCount * cameraParametersCount + 0];
//                translationMatrix.at<double>(1) = parameters[cameraCount * cameraParametersCount + 1];
//                translationMatrix.at<double>(2) = parameters[cameraCount * cameraParametersCount + 2];

//                cv::Mat rotation(3, 1, CV_64FC1);
//                rotation.at<double>(0) = parameters[cameraCount * cameraParametersCount + 3];
//                rotation.at<double>(1) = parameters[cameraCount * cameraParametersCount + 4];
//                rotation.at<double>(2) = parameters[cameraCount * cameraParametersCount + 5];

//                cv::Mat rotationMatrix(3, 3, CV_64FC1);
//                cv::Rodrigues(rotation, rotationMatrix);

//                pointOfView->GetCameraParameters().SetRotationMatrix(rotationMatrix);
//                pointOfView->GetCameraParameters().SetTranslationMatrix(translationMatrix);


//                delete[] parameters;
            }

            void BundleAdjuster::RunOnNewData()
            {
                SbaParameters params = PrepareData(STRUCTURE_AND_MOTION);
                Run(adjustedPoints, adjustedCameras, params);
            }

            void BundleAdjuster::RunOnAllData()
            {
                SbaParameters params = PrepareData(STRUCTURE_AND_MOTION);
                Run(0, 0, params);
            }

            void BundleAdjuster::Reset(const std::shared_ptr<Core::FeatureSet> &featureSet)
            {
                this->features = featureSet;
            }

            BundleAdjuster::SbaParameters BundleAdjuster::PrepareData(SbaMode mode)
            {
                SbaParameters params(this, mode);

                for (int i = 0; i < features->Size(); i++)
                {
                    std::shared_ptr<Core::Feature> feature = features->GetFeature(i);
                    if (feature->IsTriangulated())
                    {
                        params.triangulatedPoints.push_back(feature);
                    }
                }

                std::vector<std::shared_ptr<Core::PointOfView> > existingPointsOfView = features->GetPointsOfView();
                for (int i = 0; i < existingPointsOfView.size(); i++)
                {
                    std::shared_ptr<Core::PointOfView> pointOfView = existingPointsOfView.at(i);
                    if (pointOfView->GetCameraParameters().IsPoseDetermined())
                    {
                        params.pointsOfView.push_back(pointOfView);
                    }
                }

                return params;
            }

            void BundleAdjuster::Run(int startingPoint, int startingCamera, SbaParameters &params)
            {
                int pointCount = params.triangulatedPoints.size();

                if (startingPoint > pointCount)
                {
                    startingPoint = 0;
                }

                int cameraCount = params.pointsOfView.size();

                if (startingCamera > cameraCount)
                {
                    startingCamera = 0;
                }

                char *visibility = new char[pointCount * cameraCount];
                double *projections = new double[2 * pointCount * cameraCount];

                for (int i = 0; i < pointCount * cameraCount; i++)
                {
                    visibility[i] = 0;

                    projections[2 * i + 0] = 0;
                    projections[2 * i + 1] = 0;
                }

                for (int i = 0; i < cameraCount; i++)
                {
                    std::shared_ptr<Core::PointOfView> pointOfView = params.pointsOfView.at(i);
                    for (int j = 0; j < pointCount; j++)
                    {
                        std::shared_ptr<Core::Feature> feature = params.triangulatedPoints.at(j);
                        if (feature->HasCorrespondenceInView(pointOfView))
                        {
                            visibility[j * cameraCount + i] = 1;
                            cv::Point2d point = feature->GetPointInView(pointOfView);
                            projections[2 * j * cameraCount + 2 * i + 0] = point.x;
                            projections[2 * j * cameraCount + 2 * i + 1] = point.y;
                        }
                    }
                }

                const int cameraParametersCount = 7;
                const int pointParametersCount = 3;
                const int parametersCount = cameraCount * cameraParametersCount + pointCount * pointParametersCount;

                double *parameters = new double[parametersCount];

                for (int i = 0; i < cameraCount; i++)
                {
                    std::shared_ptr<Core::PointOfView> pointOfView = params.pointsOfView.at(i);
                    cv::Mat rotationMatrix = pointOfView->GetCameraParameters().GetRotationMatrix();
                    cv::Mat rotation; cv::Rodrigues(rotationMatrix, rotation);
                    cv::Mat translation = pointOfView->GetCameraParameters().GetTranslationMatrix();

                    parameters[i * cameraParametersCount + 0] = translation.at<double>(0);
                    parameters[i * cameraParametersCount + 1] = translation.at<double>(1);
                    parameters[i * cameraParametersCount + 2] = translation.at<double>(2);

                    parameters[i * cameraParametersCount + 3] = rotation.at<double>(0);
                    parameters[i * cameraParametersCount + 4] = rotation.at<double>(1);
                    parameters[i * cameraParametersCount + 5] = rotation.at<double>(2);

                    parameters[i * cameraParametersCount + 6] = pointOfView->GetCameraParameters().GetFocalLength();
                }

                int pointsStartIndex = cameraCount * cameraParametersCount;

                for (int i = 0; i < pointCount; i++)
                {
                    std::shared_ptr<Core::Feature> feature = params.triangulatedPoints.at(i);

                    parameters[pointsStartIndex + i * pointParametersCount + 0] = feature->GetX();
                    parameters[pointsStartIndex + i * pointParametersCount + 1] = feature->GetY();
                    parameters[pointsStartIndex + i * pointParametersCount + 2] = feature->GetZ();
                }

                double options[SBA_OPTSSZ];
                options[0] = SBA_INIT_MU;
                options[1] = SBA_STOP_THRESH;
                options[2] = SBA_STOP_THRESH;
                options[3] = SBA_STOP_THRESH;
                options[4] = 0.0;
                double info[SBA_INFOSZ];

                switch (params.mode) {
                    case STRUCTURE_AND_MOTION:
                        sba_motstr_levmar(pointCount,
                                          startingPoint,
                                          cameraCount,
                                          startingCamera,
                                          visibility,
                                          parameters,
                                          cameraParametersCount,
                                          pointParametersCount,
                                          projections,
                                          NULL, // covariance matrix
                                          2,
                                          ProjectPoint,
                                          NULL, // jacobian calculation function
                                          &params, // additional data
                                          150, // max iterations
                                          3, // verbosity level
                                          options,
                                          info);
                        break;
                    case STRUCTURE_ONLY:
                        sba_str_levmar(pointCount,
                                       startingPoint,
                                       cameraCount,
                                       visibility,
                                       parameters,
                                       pointParametersCount,
                                       projections,
                                       NULL, // covariance matrix
                                       2,
                                       ProjectPointStructureOnly,
                                       NULL, // jacobian calculation function
                                       &params, // additional data
                                       150, // max iterations
                                       3, // verbosity level
                                       options,
                                       info);
                        break;
                    case MOTION_ONLY:
                        sba_mot_levmar(pointCount,
                                       cameraCount,
                                       startingCamera,
                                       visibility,
                                       parameters,
                                       cameraParametersCount,
                                       projections,
                                       NULL, // covariance matrix
                                       2,
                                       ProjectPointMotionOnly,
                                       NULL, // jacobian calculation function
                                       &params, // additional data
                                       150, // max iterations
                                       3, // verbosity level
                                       options,
                                       info);
                        break;
                }

                delete[] visibility;
                delete[] projections;

                for (int i = startingCamera; i < cameraCount; i++)
                {
                    std::shared_ptr<Core::PointOfView> pointOfView = params.pointsOfView.at(i);

                    cv::Mat translationMatrix(3, 1, CV_64FC1);
                    translationMatrix.at<double>(0) = parameters[i * cameraParametersCount + 0];
                    translationMatrix.at<double>(1) = parameters[i * cameraParametersCount + 1];
                    translationMatrix.at<double>(2) = parameters[i * cameraParametersCount + 2];

                    cv::Mat rotation(3, 1, CV_64FC1);
                    rotation.at<double>(0) = parameters[i * cameraParametersCount + 3];
                    rotation.at<double>(1) = parameters[i * cameraParametersCount + 4];
                    rotation.at<double>(2) = parameters[i * cameraParametersCount + 5];

                    cv::Mat rotationMatrix(3, 3, CV_64FC1);
                    cv::Rodrigues(rotation, rotationMatrix);

                    double focalLength = parameters[i * cameraParametersCount + 6];
                    std::cout << "Focal length estimate for camera " << i << ": " << focalLength << std::endl;

                    pointOfView->GetCameraParameters().SetRotationMatrix(rotationMatrix);
                    pointOfView->GetCameraParameters().SetTranslationMatrix(translationMatrix);

                    pointOfView->GetCameraParameters().SetFocalLength(focalLength);
                }

                for (int i = startingPoint; i < pointCount; i++)
                {
                    std::shared_ptr<Core::Feature> feature = params.triangulatedPoints.at(i);

                    double x = parameters[pointsStartIndex + i * pointParametersCount + 0];
                    double y = parameters[pointsStartIndex + i * pointParametersCount + 1];
                    double z = parameters[pointsStartIndex + i * pointParametersCount + 2];

                    feature->SetPosition(x, y, z);
                }

                delete[] parameters;

                adjustedCameras = cameraCount;
                adjustedPoints = pointCount;
            }

            void BundleAdjuster::ProjectPoint(int j, int i, double *cameraParams, double *pointParams, double *projection, void *additionalData)
            {
                cv::Mat translation(3, 1, CV_64FC1);
                translation.at<double>(0) = cameraParams[0];
                translation.at<double>(1) = cameraParams[1];
                translation.at<double>(2) = cameraParams[2];

                cv::Mat rotation(3, 1, CV_64FC1);
                rotation.at<double>(0) = cameraParams[3];
                rotation.at<double>(1) = cameraParams[4];
                rotation.at<double>(2) = cameraParams[5];

                cv::Mat rotationMatrix(3, 3, CV_64FC1);
                cv::Rodrigues(rotation, rotationMatrix);

                double focalLength = cameraParams[6];

                double x = pointParams[0];
                double y = pointParams[1];
                double z = pointParams[2];

                cv::Mat point(3, 1, CV_64FC1);
                point.at<double>(0) = x - translation.at<double>(0);
                point.at<double>(1) = y - translation.at<double>(1);
                point.at<double>(2) = z - translation.at<double>(2);
                point = rotationMatrix * point;

                projection[0] = -point.at<double>(0) * focalLength / point.at<double>(2);
                projection[1] = -point.at<double>(1) * focalLength / point.at<double>(2);
            }

            void BundleAdjuster::ProjectPointStructureOnly(int j, int i, double *pointParams, double *projection, void *additionalData)
            {
                SbaParameters *params = (SbaParameters *) additionalData;

                double cameraParams[7];
                cameraParams[0] = params->pointsOfView.at(j)->GetCameraParameters().GetTranslationMatrix().at<double>(0);
                cameraParams[1] = params->pointsOfView.at(j)->GetCameraParameters().GetTranslationMatrix().at<double>(1);
                cameraParams[2] = params->pointsOfView.at(j)->GetCameraParameters().GetTranslationMatrix().at<double>(2);

                cv::Mat rotationMatrix = params->pointsOfView.at(j)->GetCameraParameters().GetRotationMatrix();
                cv::Mat rotation; cv::Rodrigues(rotationMatrix, rotation);

                cameraParams[3] = rotation.at<double>(0);
                cameraParams[4] = rotation.at<double>(1);
                cameraParams[5] = rotation.at<double>(2);

                cameraParams[6] = params->pointsOfView.at(j)->GetCameraParameters().GetFocalLength();

                ProjectPoint(j, i, cameraParams, pointParams, projection, additionalData);
            }

            void BundleAdjuster::ProjectPointMotionOnly(int j, int i, double *cameraParams, double *projection, void *additionalData)
            {
                SbaParameters *params = (SbaParameters *) additionalData;

                double pointParams[3];
                pointParams[0] = params->triangulatedPoints.at(i)->GetX();
                pointParams[1] = params->triangulatedPoints.at(i)->GetY();
                pointParams[2] = params->triangulatedPoints.at(i)->GetZ();

                ProjectPoint(j, i, cameraParams, pointParams, projection, additionalData);
            }
        }
    }
}