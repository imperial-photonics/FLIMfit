#pragma once 

#include "AbstractDecayGroup.h"
#include "IRFConvolution.h"

template<class T> // TODO: make this part of class
int getBeta(const vector<shared_ptr<FittingParameter>>& beta_parameters, double fixed_beta, int n_beta_free, const T* alf, T beta[])
{
   alf2beta(n_beta_free, alf, beta);

   int idx = 0;
   for (int i = 0; i < beta_parameters.size(); i++)
   {
      if (!beta_parameters[i]->isFixed())
         beta[i] = beta[idx++] * (1 - fixed_beta);
   }

   for (int i = 0; i < beta_parameters.size(); i++)
   {
      if (beta_parameters[i]->isFixed())
         beta[i] = beta_parameters[i]->initial_value;
   }

   return n_beta_free - 1;
}
class MultiExponentialDecayGroupPrivate : public AbstractDecayGroup
{
   Q_OBJECT

public:

   MultiExponentialDecayGroupPrivate(int n_exponential_ = 1, bool contributions_global_ = false, const QString& name = "Multi-Exponential Decay");

   MultiExponentialDecayGroupPrivate(const MultiExponentialDecayGroupPrivate& obj);

   virtual void setNumExponential(int n_exponential);
   void setContributionsGlobal(bool contributions_global);

   virtual const vector<double>& getChannelFactors(int index);
   virtual void setChannelFactors(int index, const vector<double>& channel_factors);

   virtual int setVariables(const double* variables);
   virtual int calculateModel(double* a, int adim, vector<double>& kap, int bin_shift = 0);
   virtual int calculateDerivatives(double* b, int bdim, vector<double>& kap);
   virtual int getNonlinearOutputs(float* nonlin_variables, float* output, int& nonlin_idx);
   virtual int getLinearOutputs(float* lin_variables, float* output, int& lin_idx);
   virtual int setupIncMatrix(std::vector<int>& inc, int& row, int& col);
   virtual void getLinearOutputParamNames(vector<string>& names);

protected:
  
   virtual void init();
   void setupParametersMultiExponential();

   void resizeLifetimeParameters(std::vector<std::shared_ptr<FittingParameter>>& params, int new_size, const std::string& name_prefix);

   int addDecayGroup(const vector<ExponentialPrecomputationBuffer>& buffers, double* a, int adim, vector<double>& kap, int bin_shift = 0);
   int addLifetimeDerivative(int idx, double* b, int bdim, vector<double>& kap);
   int addContributionDerivatives(double* b, int bdim, vector<double>& kap);
   int normaliseLinearParameters(float* lin_variables, int n, float* output, int& lin_idx);

   vector<shared_ptr<FittingParameter>> tau_parameters;
   vector<shared_ptr<FittingParameter>> beta_parameters;

   int n_exponential;
   bool contributions_global;

   vector<double> tau;
   vector<double> beta;
   vector<ExponentialPrecomputationBuffer> buffer;
   vector<double> channel_factors;

private:
   template<class Archive>
   void serialize(Archive & ar, const unsigned int version);
   
   const double* beta_param_values = nullptr;
   int n_beta_free;
   double fixed_beta;
   friend class boost::serialization::access;
   
};

template<class Archive>
void MultiExponentialDecayGroupPrivate::serialize(Archive & ar, const unsigned int version)
{
   ar & tau_parameters;
   ar & beta_parameters;
   ar & n_exponential;
   ar & contributions_global;
   ar & channel_factors;
   ar & boost::serialization::base_object<AbstractDecayGroup>(*this);
};

class MultiExponentialDecayGroup : public MultiExponentialDecayGroupPrivate
{
   Q_OBJECT

public:

   MultiExponentialDecayGroup(int n_exponential_ = 1, bool contributions_global_ = false, const QString& name = "Multi-Exponential Decay") :
      MultiExponentialDecayGroupPrivate(n_exponential_, contributions_global_, name)
   {
   }


   MultiExponentialDecayGroup(const MultiExponentialDecayGroup& obj) :
      MultiExponentialDecayGroupPrivate(obj)
   {
      setupParametersMultiExponential();
      init();
   }

   AbstractDecayGroup* clone() const { return new MultiExponentialDecayGroup(*this); }

   Q_PROPERTY(int n_exponential MEMBER n_exponential WRITE setNumExponential USER true);
   Q_PROPERTY(bool contributions_global MEMBER contributions_global WRITE setContributionsGlobal USER true);

};

/*
class QMultiExponentialDecayGroup : public QAbstractDecayGroup, virtual public MultiExponentialDecayGroup
{
   Q_OBJECT

public:

   QMultiExponentialDecayGroup(const QString& name = "Multi Exponential Decay", QObject* parent = 0) :
      QAbstractDecayGroup(name, parent) {};

   Q_PROPERTY(int n_exponential MEMBER n_exponential WRITE SetNumExponential USER true);
   Q_PROPERTY(bool contributions_global MEMBER contributions_global WRITE SetContributionsGlobal USER true);

private:
   template<class Archive>
   void serialize(Archive & ar, const unsigned int version);
   
   friend class boost::serialization::access;
   
};

template<class Archive>
void QMultiExponentialDecayGroup::serialize(Archive & ar, const unsigned int version)
{
   ar & boost::serialization::base_object<MultiExponentialDecayGroup>(*this);
};
*/

BOOST_CLASS_TRACKING(MultiExponentialDecayGroup, track_always)

//BOOST_CLASS_TRACKING(QMultiExponentialDecayGroup, track_always)