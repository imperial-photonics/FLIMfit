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

#include "MeasuredIrf.h"
#include "MeasuredIrfConvolver.h"

#include <algorithm>
#include <cmath>
#include <cassert>

MeasuredIrf::MeasuredIrf() :
   n_irf_rep(1),
   full_image_irf(false)
{
   irf = { 0.0, 1.0, 0.0, 0.0 };

   timebin_t0 = -1.0;
   timebin_width = 1.0;
}

std::shared_ptr<AbstractConvolver> MeasuredIrf::getConvolver(std::shared_ptr<TransformedDataParameters> dp)
{
   return std::make_shared<MeasuredIrfConvolver>(dp);
}




double MeasuredIrf::calculateMean()
{
   double mu = timebin_t0;
   for (int i = 0; i < n_irf; i++)
      mu += irf[i] * i;
   return timebin_t0 + mu * timebin_width;
}

void MeasuredIrf::setReferenceReconvolution(bool ref_reconvolution, double ref_lifetime_guess)
{
   // TODO
//   this->ref_reconvolution = ref_reconvolution;
//   this->ref_lifetime_guess = ref_lifetime_guess;
}

bool MeasuredIrf::isSpatiallyVariant()
{
   return InstrumentResponseFunction::isSpatiallyVariant() || full_image_irf;
}


bool MeasuredIrf::arePositionsEquivalent(PixelIndex idx1, PixelIndex idx2)
{
   return (!full_image_irf) && InstrumentResponseFunction::arePositionsEquivalent(idx1, idx2);
}



double_iterator MeasuredIrf::getIrf(PixelIndex irf_idx, double t0_shift, double_iterator storage)
{
   if (full_image_irf)
      return irf.begin() + irf_idx.pixel * n_irf * n_chan;

   t0_shift += getT0Shift(irf_idx);

   if (t0_shift == 0.0)
      return irf.begin();

   shiftIrf(t0_shift, storage);
   return storage;
}



void MeasuredIrf::shiftIrf(double shift, double_iterator storage)
{
   int i;

   shift = -shift;
   shift /= timebin_width;

   int c_shift = (int)floor(shift);
   double f_shift = shift - c_shift;

   int start = std::max(0, 1 - c_shift);
   int end = std::min(n_irf, n_irf - c_shift - 3);

   start = std::min(start, n_irf - 1);
   end = std::max(end, 1);

   for (int c = 0; c < n_chan; c++)
   {
      int offset = n_irf * c;
      for (i = 0; i < start; i++)
         storage[i + offset] = irf[offset];


      for (i = start; i < end; i++)
      {
         // will read y[0]...y[3]
         assert(i + c_shift - 1 < (n_irf - 3));
         assert(i + c_shift - 1 >= 0);
         storage[i + offset] = cubicInterpolate(irf.data() + i + c_shift - 1 + offset, f_shift);
      }

      for (i = end; i < n_irf; i++)
         storage[i + offset] = irf[n_irf - 1 + offset];
   }

}

void MeasuredIrf::allocateBuffer(int n_irf_raw)
{
   n_irf = (int)(ceil(n_irf_raw / 2.0) * 2);
   int irf_size = n_irf * n_chan * n_irf_rep;

   irf.resize(irf_size);
}


/**
 * Calculate g factor for polarisation resolved data
 *
 * g factor gives relative sensitivity of parallel and perpendicular channels,
 * and so can be determined from the ratio of the IRF's for the two channels
*/
/*
double InstrumentResponseFunction::calculateGFactor()
{

   if (n_chan == 2)
   {
      double perp = 0;
      double para = 0;
      for(int i=0; i<n_irf; i++)
      {
         para += irf[i];
         perp += irf[i+n_irf];
      }

      g_factor = para / perp;
   }
   else
   {
      g_factor = 1;
   }

   return g_factor;
}
*/

// http://paulbourke.net/miscellaneous/interpolation/
double MeasuredIrf::cubicInterpolate(double  y[], double mu)
{
   // mu - distance between y1 and y2
   double a0, a1, a2, a3, mu2;

   mu2 = mu * mu;
   a0 = -0.5*y[0] + 1.5*y[1] - 1.5*y[2] + 0.5*y[3];
   a1 = y[0] - 2.5*y[1] + 2 * y[2] - 0.5*y[3];
   a2 = -0.5*y[0] + 0.5*y[2];
   a3 = y[1];

   return(a0*mu*mu2 + a1 * mu2 + a2 * mu + a3);
}