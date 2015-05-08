/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/


#ifndef mitkAbstractClassifier_h
#define mitkAbstractClassifier_h

#include <MitkClassificationCoreExports.h>

#include <mitkBaseData.h>

// Eigen
#include <Eigen/Dense>

// STD Includes

// MITK includes
#include <mitkConfigurationHolder.h>

namespace mitk
{
class MITKCLASSIFICATIONCORE_EXPORT AbstractClassifier : public BaseData
{
public:

  ///
  /// @brief Build a forest of trees from the training set (X, y).
  /// @param X, The training input samples. Matrix of shape = [n_samples, n_features]
  /// @param Y, The target values (class labels in classification, real numbers in regression). Matrix of shape = [n_samples, 1]
  ///
  virtual void Train(const Eigen::MatrixXd &X, const Eigen::MatrixXi &Y) = 0;

  ///
  /// @brief Predict class for X.
  /// @param X, The input samples.
  /// @return The predicted classes. Y matrix of shape = [n_samples, 1]
  ///
  virtual Eigen::MatrixXi Predict(const Eigen::MatrixXd &X) = 0;

  ///
  /// @brief GetPointWiseWeightCopy
  /// @return return label matrix of shape = [n_samples , 1]
  ///
  Eigen::MatrixXi & GetLabels()
  {
    return m_OutLabel;
  }

protected:
  Eigen::MatrixXi m_OutLabel;


public:
  // * --------------- *
  // PointWiseWeight
  // * --------------- *

  ///
  /// @brief SupportsPointWiseWeight
  /// @return True if the classifier supports pointwise weighting else false
  ///
  virtual bool SupportsPointWiseWeight() = 0;

  ///
  /// @brief GetPointWiseWeightCopy
  /// @return Create and return a copy of W
  ///
  Eigen::MatrixXd & GetPointWiseWeight()
  {
    return m_PointWiseWeight;
  }

  ///
  /// @brief SetPointWiseWeight
  /// @param W, The pointwise weights. W matrix of shape = [n_samples, 1]
  ///
  void SetPointWiseWeight(const Eigen::MatrixXd& W)
  {
    this->m_PointWiseWeight = W;
  }

  ///
  /// @brief UsePointWiseWeight
  /// @param toggle weighting on/off
  ///
  void UsePointWiseWeight(bool value)
  {
    this->m_IsUsingPointWiseWeight = value;
  }

  ///
  /// @brief IsUsingPointWiseWeight
  /// @return true if pointewise weighting is enabled.
  ///
  bool IsUsingPointWiseWeight()
  {
    return this->m_IsUsingPointWiseWeight;
  }

protected:
  Eigen::MatrixXd m_PointWiseWeight;
  bool m_IsUsingPointWiseWeight;

  // * --------------- *
  // PointWiseProbabilities
  // * --------------- *

public:
  ///
  /// @brief SupportsPointWiseProbability
  /// @return True if the classifier supports pointwise class probability calculation else false
  ///
  virtual bool SupportsPointWiseProbability() = 0;

  ///
  /// @brief GetPointWiseWeightCopy
  /// @return return probability matrix
  ///
  Eigen::MatrixXd & GetPointWiseProbabilities()
  {
    return m_OutProbability;
  }

  ///
  /// \brief UsePointWiseProbabilities
  /// \param value
  ///
  void UsePointWiseProbability(bool value)
  {
    m_IsUsingPointWiseProbability = value;
  }

  ///
  /// \brief IsUsingPointWiseProbabilities
  /// \return
  ///
  bool IsUsingPointWiseProbability()
  {
    return m_IsUsingPointWiseProbability;
  }

protected:
  Eigen::MatrixXd m_OutProbability;
  bool m_IsUsingPointWiseProbability;

public:

#ifndef DOXYGEN_SKIP

  virtual void SetRequestedRegionToLargestPossibleRegion(){}
  virtual bool RequestedRegionIsOutsideOfTheBufferedRegion(){return true;}
  virtual bool VerifyRequestedRegion(){return false;}
  virtual void SetRequestedRegion(const itk::DataObject */*data*/){}

  // Override
  virtual bool IsEmpty() const override
  {
    if(IsInitialized() == false)
      return true;
    const TimeGeometry* timeGeometry = const_cast<AbstractClassifier*>(this)->GetUpdatedTimeGeometry();
    if(timeGeometry == NULL)
      return true;
    return false;
  }

#endif // Skip Doxygen

};
}

#endif //mitkAbstractClassifier_h
