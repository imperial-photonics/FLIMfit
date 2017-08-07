#pragma once

#include "AbstractDecayGroup.h"

class Pattern
{
public:

   Pattern(const std::vector<double>& params)
   {
      if (params.size() % 2 == 0)
         throw std::runtime_error("Params should take form tau_1 beta_1 ... tau_n beta_n offset");

      int n_exp = (params.size() - 1) / 2;
      tau.resize(n_exp);
      beta.resize(n_exp);

      for (int i = 0; i < n_exp; i++)
      {
         tau[i] = params[i * 2];
         beta[i] = params[i * 2 + 1];
      }

      offset = params[n_exp * 2];
   }

   Pattern(const std::vector<double>& tau, const std::vector<double>& beta, double offset) :
      tau(tau), beta(beta), offset(offset)
   {
      if (tau.size() != beta.size())
         throw std::runtime_error("Expected tau and beta to be the same size");
   }

   std::vector<double> tau;
   std::vector<double> beta;
   double offset;
};

class PatternDecayGroup : public AbstractDecayGroup
{
   Q_OBJECT

public:

   PatternDecayGroup(const std::vector<Pattern> pattern, const QString& name = "Pattern");
   PatternDecayGroup(const PatternDecayGroup& obj);

   int setVariables(const double* variables);
   int calculateModel(double* a, int adim, double& kap, int bin_shift = 0);
   int calculateDerivatives(double* b, int bdim, double kap_derv[]);
   void addConstantContribution(float* a);

   void setupIncMatrix(std::vector<int>& inc, int& row, int& col);
   int getNonlinearOutputs(float* nonlin_variables, float* output, int& nonlin_idx);
   int getLinearOutputs(float* lin_variables, float* output, int& lin_idx);

   void getNonlinearOutputParamNames(vector<string>& names);
   void getLinearOutputParamNames(vector<string>& names);

   const vector<double>& getChannelFactors(int index);
   void setChannelFactors(int index, const vector<double>& channel_factors);


   AbstractDecayGroup* clone() const { return new PatternDecayGroup(*this); }

protected:

   void init();

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version);

   friend class boost::serialization::access;

   std::vector<Pattern> pattern;
   std::vector<double> channel_factors;
   std::vector<double> decay;
};

template<class Archive>
void PatternDecayGroup::serialize(Archive & ar, const unsigned int version)
{
   ar & boost::serialization::base_object<MultiExponentialDecayGroupPrivate>(*this);
   ar & pattern;
};


BOOST_CLASS_TRACKING(PatternDecayGroup, track_always)