/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef MITKCONNECTOMICSHISTOGRAMCACHE_H
#define MITKCONNECTOMICSHISTOGRAMCACHE_H

#include "mitkSimpleHistogram.h"
#include "mitkConnectomicsNetwork.h"

#include <mitkConnectomicsDegreeHistogram.h>
#include <mitkConnectomicsBetweennessHistogram.h>
#include <mitkConnectomicsShortestPathHistogram.h>

#include "MitkDiffusionImagingExports.h"

namespace mitk {

  //##Documentation
  //## @brief Provides a method to cache network histograms

  class ConnectomicsHistogramsContainer 
  {
  public:

    void ComputeFromBaseData(BaseData* baseData)
    {
      m_BetweennessHistogram.ComputeFromBaseData(baseData);
      m_DegreeHistogram.ComputeFromBaseData(baseData);
      m_ShortestPathHistogram.ComputeFromBaseData(baseData);
    }

    ConnectomicsBetweennessHistogram* GetBetweennessHistogram( )
    {
      return &m_BetweennessHistogram;
    }

    ConnectomicsDegreeHistogram* GetDegreeHistogram( )
    {
      return &m_DegreeHistogram;
    }

    ConnectomicsShortestPathHistogram* GetShortestPathHistogram( )
    {
      return &m_ShortestPathHistogram;
    }

    ConnectomicsBetweennessHistogram  m_BetweennessHistogram;
    ConnectomicsDegreeHistogram       m_DegreeHistogram;
    ConnectomicsShortestPathHistogram m_ShortestPathHistogram;
  };

  class MitkDiffusionImaging_EXPORT ConnectomicsHistogramCache : public SimpleHistogramCache
  {
  public:

    ConnectomicsHistogramCache();
    ~ConnectomicsHistogramCache();

    ConnectomicsHistogramsContainer *operator[]( ConnectomicsNetwork::Pointer sp_NetworkData);


  protected:

    // purposely not implemented
    SimpleHistogram *operator[](BaseData::Pointer sp_BaseData); 

  };



  class ConnectomicsHistogramsCacheElement : public ConnectomicsHistogramCache::Element
  {
  public:

    void ComputeFromBaseData(BaseData* baseData)
    {
      m_Container.ComputeFromBaseData(baseData);
    }

    ConnectomicsHistogramsContainer* GetHistograms()
    {
      return &m_Container;
    }

    ConnectomicsHistogramsContainer m_Container;

  private:
    ConnectomicsHistogramBase* GetHistogram()
    {
      return NULL;
    }
  };



}

#endif // MITKCONNECTOMICSHISTOGRAMCACHE_H
