#pragma once
#include "ui_FittingWidget.h"

#include "MultiExponentialDecayGroup.h"
#include "FretDecayGroup.h"
#include "FittingParametersWidget.h"
#include "FLIMImageSet.h"
#include "FLIMImporter.h"
#include "FLIMGlobalFitController.h"
#include "TextReader.h"
#include "FLIMImageWidget.h"

#include <memory>
#include <fstream>

class FittingWidget : public QWidget, protected Ui::FittingWidget
{
public:
   FittingWidget(QWidget* parent) :
      QWidget(parent)
   {
      setupUi(this);

      connect(fit_button, &QPushButton::pressed, this, &FittingWidget::Fit);

      QString root = "/Users/sean/Documents/FLIMTestData";

//      QString irf_name = QString("%1%2").arg(root).arg("/acceptor-fret-irf.csv");
      QString irf_name = QString("%1%2").arg(root).arg("/2015-05-15 Dual FRET IRF.csv");

      auto irf = FLIMImporter::importIRF(irf_name);
      

      QString folder = QString("%1%2").arg(root).arg("/dual_data");
      images = FLIMImporter::importFromFolder(folder);
      
      auto acq = images->getAcquisitionParameters();
      acq->SetIRF(irf);
      decay_model = std::make_shared<QDecayModel>();
      
      transform = std::make_shared<QDataTransformationSettings>();
      
      auto dp = std::make_shared<TransformedDataParameters>(acq, *transform.get());

      decay_model->SetTransformedDataParameters(dp);
      //decay_model->AddDecayGroup(std::make_shared<QMultiExponentialDecayGroup>());
      
      
      auto fret_group = std::make_shared<QFretDecayGroup>();
      fret_group->SetIncludeAcceptor(false);

      std::vector<double> ch_donor = { 0.12, 0.64, 0.11, 0.60 };
      fret_group->SetChannelFactors(0, ch_donor);
      
//      std::vector<double> ch_acceptor = { 0.0, 1.0 };
//      fret_group->SetChannelFactors(1, ch_acceptor);
      
      decay_model->AddDecayGroup(fret_group);

      data_list->setModel(images.get());
      

      parameters_widget->SetDecayModel(decay_model);
      
      image_widget = new FLIMImageWidget(this);
      image_widget->setMinimumSize(500,500);
      mdi_area->addSubWindow(image_widget);

      connect(images.get(), &FLIMImageSet::currentImageChanged, image_widget, &FLIMImageWidget::setImage);
      image_widget->setImage(images->getCurrentImage());
      //Fit();
   }

   void FitSelected()
   {
      /*
      selected_fit_controller = std::make_shared<FLIMGlobalFitController>();
      selected_fit_controller->SetFitSettings(FitSettings(ALG_ML));
      
      
      auto image = images->getCurrentImage();
      cv::Mat selection_mask = image_widget->getSelectionMask();
      std::vector<std::vector<double>> decay;
      image->getDecay(selection_mask, decay);
      
      
      fit_controller->SetData(data);
      fit_controller->SetModel(decay_model);
      
      fit_controller->Init();
      fit_controller->RunWorkers();
      
      int mask = 0;
      int n_valid = 0;
      vector<double> fit(2000);
      fit_controller->GetFit(0, 1, &mask, fit.data(), n_valid);
      
      std::ofstream os("C:/Users/sean/Documents/FLIMTestData/results.csv");
      for (int i = 0; i < fit.size(); i++)
         os << fit[i] << "\n";
       */
      
   }
   
   void Fit()
   {
      
      try
      {
      
      fit_controller = std::make_shared<FLIMGlobalFitController>();
      fit_controller->SetFitSettings(FitSettings(ALG_LM, MODE_IMAGEWISE));
      
      auto data = std::make_shared<FLIMData>(images->getImages(), *transform.get());
         
      fit_controller->SetData(data);
      fit_controller->SetModel(decay_model);
      
      fit_controller->Init();
      fit_controller->RunWorkers();

      int mask = 0;
      int n_valid = 0;
      vector<double> fit(2000);
      fit_controller->GetFit(0, 1, &mask, fit.data(), n_valid);

      std::ofstream os("C:/Users/sean/Documents/FLIMTestData/results.csv");
      for (int i = 0; i < fit.size(); i++)
         os << fit[i] << "\n";
      }
      catch(std::exception e)
      {
         std::cout << "Exception occurred: " << e.what() << "\n";
      }
   }

protected:
   std::shared_ptr<FLIMImageSet> images;
   std::shared_ptr<FLIMGlobalFitController> fit_controller;
   std::shared_ptr<FLIMGlobalFitController> selected_fit_controller;
   std::shared_ptr<QDecayModel> decay_model;
   std::shared_ptr<QDataTransformationSettings> transform;
   FLIMImageWidget* image_widget;
};