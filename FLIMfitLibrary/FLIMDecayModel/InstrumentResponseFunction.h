//=========================================================================
//
// Copyright (C) 2013 Imperial College London.
// All rights reserved.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// This software tool was developed with support from the UK 
// Engineering and Physical Sciences Council 
// through  a studentship from the Institute of Chemical Biology 
// and The Wellcome Trust through a grant entitled 
// "The Open Microscopy Environment: Image Informatics for Biological Sciences" (Ref: 095931).
//
// Author : Sean Warren
//
//=========================================================================

#pragma once


#include "AlignedVectors.h"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/version.hpp>
#include <vector>
#include <cmath>
#include <opencv2/core/core.hpp>

class PixelIndex
{
public:

   PixelIndex(int pixel_ = 0, int image_ = 0)
   {
      pixel = pixel_;
      image = image_;
   }

   PixelIndex& operator=(int pixel_)
   {
      pixel = pixel_;
      return *this;
   }

   int image = 0;
   int pixel = 0;
};

class GaussianParameters
{
public:
   GaussianParameters(double mu = 1000, double sigma = 100, double offset = 0) :
      mu(mu), sigma(sigma), offset(offset)
   {}

   double mu;
   double sigma;
   double offset;

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & mu;
      ar & sigma;
      ar & offset;
   }
};

enum IRFType
{
   Scatter,
   Reference,
   Gaussian
};

class InstrumentResponseFunction
{
public:
   InstrumentResponseFunction();

   template<typename it>
   void setIRF(int n_t, int n_chan, double timebin_t0, double timebin_width, it irf);
   void setGaussianIRF(const std::vector<GaussianParameters>& gaussian_params);

   void setFrameT0(const std::vector<double>& frame_t0);
   void setSpatialT0(const cv::Mat& spatial_t0);

   bool isSpatiallyVariant();

   bool arePositionsEquivalent(PixelIndex idx1, PixelIndex idx2);

   void setImageIRF(int n_t, int n_chan, int n_irf_rep, double timebin_t0, double timebin_width, double_iterator irf); //TODO

   void setReferenceReconvolution(int ref_reconvolution, double ref_lifetime_guess);

   void setPolarisation(double g_factor, double polarisation_angle);
   void setGFactor(const std::vector<double>& g_factor);

   std::vector<double>& getGFactor() { return g_factor; };

   double_iterator getIRF(PixelIndex irf_idx, double t0_shift, double_iterator storage);
   double getT0Shift(PixelIndex irf_idx);

   double getT0() { return timebin_t0; }

   bool isGaussian() { return type == Gaussian; }

   int getNumChan();

   double timebin_width;
   double timebin_t0;

   int n_irf;
   int n_irf_rep;

   double polarisation_angle = 0.0;

   std::vector<GaussianParameters> gaussian_params;

   IRFType type;

private:
   template<typename it>
   void copyIRF(int n_irf_raw, it irf);

   void shiftIRF(double shift, double_iterator storage);
   //double calculateGFactor();

   void allocateBuffer(int n_irf_raw);

   static double cubicInterpolate(double  y[], double mu);

   aligned_vector<double> irf;

   std::vector<double> g_factor;

   bool full_image_irf;

   bool spatially_varying_t0;
   cv::Mat spatial_t0;

   bool frame_varying_t0;
   std::vector<double> frame_t0;

//   double t0;
   int n_chan;


   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
      ar & timebin_width;
      ar & timebin_t0;
      if (version < 5)
      {
         bool variable_irf;
         ar & variable_irf;
      }
      ar & n_irf;
      ar & n_chan;
      ar & n_irf_rep;

      if (version >= 4)
      {
         ar & g_factor;
      }
      else if (Archive::is_loading::value)
      {
         double g;
         ar & g;
         g_factor.resize(n_chan, 1.0);
      }

      ar & type;
      ar & irf;

      if (version >= 2)
         ar & gaussian_params;
      if (version >= 3)
         ar & polarisation_angle;

      if (version >= 5)
      {
         ar & frame_varying_t0;
         ar & frame_t0;
         ar & spatially_varying_t0;
         ar & spatial_t0;
      }
   }
   
   friend class boost::serialization::access;
   
};

template<typename it>
void InstrumentResponseFunction::setIRF(int n_t, int n_chan_, double timebin_t0_, double timebin_width_, it irf)
{
   n_chan = n_chan_;
   n_irf_rep = 1;
   full_image_irf = false;
   spatially_varying_t0 = false;
   frame_varying_t0 = false;

   timebin_t0 = timebin_t0_;
   timebin_width = timebin_width_;

   copyIRF(n_t, irf);

   // Check normalisation of IRF
   for (int i = 0; i < n_chan; i++)
   {
      double sum = 0;
      for (int j = 0; j < n_t; j++)
         sum += irf[n_t * i + j];
      if (std::fabs(sum - 1.0) > 0.1)
         throw std::runtime_error("IRF is not correctly normalised");
   }

   g_factor.assign(n_chan, 1.0);

   //calculateGFactor();
}

template<typename it>
void InstrumentResponseFunction::copyIRF(int n_irf_raw, it irf_)
{
   // Copy IRF, padding to ensure we have an even number of points so we can 
   // use SSE primatives in convolution
   //------------------------------
   allocateBuffer(n_irf_raw);

   for (int j = 0; j<n_irf_rep; j++)
   {
      int i;
      for (i = 0; i<n_irf_raw; i++)
         for (int k = 0; k<n_chan; k++)
            irf[(j*n_chan + k)*n_irf + i] = irf_[(j*n_chan + k)*n_irf_raw + i];
      for (; i<n_irf; i++)
         for (int k = 0; k<n_chan; k++)
            irf[(j*n_chan + k)*n_irf + i] = 0;
   }

}

BOOST_CLASS_VERSION(InstrumentResponseFunction, 5)
