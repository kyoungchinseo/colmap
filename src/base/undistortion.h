// COLMAP - Structure-from-Motion and Multi-View Stereo.
// Copyright (C) 2016  Johannes L. Schoenberger <jsch at inf.ethz.ch>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef COLMAP_SRC_BASE_UNDISTORTION_H_
#define COLMAP_SRC_BASE_UNDISTORTION_H_

#include "base/reconstruction.h"
#include "util/bitmap.h"
#include "util/threading.h"

namespace colmap {

struct UndistortCameraOptions {
  // The amount of blank pixels in the undistorted image in the range [0, 1].
  double blank_pixels = 0.0;

  // Minimum and maximum scale change of camera used to satisfy the blank
  // pixel constraint.
  double min_scale = 0.2;
  double max_scale = 2.0;

  // Maximum image size in terms of width or height of the undistorted camera.
  int max_image_size = -1;
};

// Undistort images and export undistorted cameras, as required by the
// mvs::PatchMatchController class.
class COLMAPUndistorter : public Thread {
 public:
  COLMAPUndistorter(const UndistortCameraOptions& options,
                    const Reconstruction& reconstruction,
                    const std::string& image_path,
                    const std::string& output_path);

 private:
  void Run();

  void Undistort(const size_t reg_image_idx) const;
  void WritePatchMatchConfig() const;
  void WriteFusionConfig() const;
  void WriteScript() const;

  UndistortCameraOptions options_;
  std::string image_path_;
  std::string output_path_;
  const Reconstruction& reconstruction_;
};

// Undistort images and prepare data for CMVS/PMVS.
class PMVSUndistorter : public Thread {
 public:
  PMVSUndistorter(const UndistortCameraOptions& options,
                  const Reconstruction& reconstruction,
                  const std::string& image_path,
                  const std::string& output_path);

 private:
  void Run();

  void Undistort(const size_t reg_image_idx) const;
  void WriteVisibilityData() const;
  void WriteOptionFile() const;

  UndistortCameraOptions options_;
  std::string image_path_;
  std::string output_path_;
  const Reconstruction& reconstruction_;
};

// Undistort images and prepare data for CMP-MVS.
class CMPMVSUndistorter : public Thread {
 public:
  CMPMVSUndistorter(const UndistortCameraOptions& options,
                    const Reconstruction& reconstruction,
                    const std::string& image_path,
                    const std::string& output_path);

 private:
  void Run();

  void Undistort(const size_t reg_image_idx) const;

  UndistortCameraOptions options_;
  std::string image_path_;
  std::string output_path_;
  const Reconstruction& reconstruction_;
};

// Undistort camera by resizing the image and shifting the principal point.
//
// The scaling factor is computed such that no blank pixels are in the
// undistorted image (blank_pixels=0) or all pixels in distorted image are
// contained in output image (blank_pixels=1).
//
// The focal length of the image is preserved and the dimensions of the
// undistorted pinhole camera are adjusted such that either all pixels in
// the undistorted image have a corresponding pixel in the distorted image
// (i.e. no blank pixels at the borders, for `blank_pixels=0`), or all pixels
// in the distorted image project have a corresponding pixel in the undistorted
// image (i.e. blank pixels at the borders, for `blank_pixels=1`). Intermediate
// states can be achieved by setting `blank_pixels` between 0 and 1.
//
// The relative location of the principal point of the distorted camera is
// preserved. The scaling of the image dimensions is subject to the `min_scale`,
// `max_scale`, and `max_image_size` constraints.
Camera UndistortCamera(const UndistortCameraOptions& options,
                       const Camera& camera);

// Undistort image such that the viewing geometry of the undistorted image
// follows a pinhole camera model. See `UndistortCamera` for more details
// on the undistortion conventions.
void UndistortImage(const UndistortCameraOptions& options,
                    const Bitmap& distorted_image,
                    const Camera& distorted_camera, Bitmap* undistorted_image,
                    Camera* undistorted_camera);

// Undistort all cameras in the reconstruction and accordingly all
// observations in their corresponding images.
void UndistortReconstruction(const UndistortCameraOptions& options,
                             Reconstruction* reconstruction);

}  // namespace colmap

#endif  // COLMAP_SRC_BASE_UNDISTORTION_H_
