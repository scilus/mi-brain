#ifndef _MITK_FiberBundle_H
#define _MITK_FiberBundle_H

//includes for MITK datastructure
#include <mitkBaseData.h>
#include "FiberBundleExports.h"
#include <mitkImage.h>
#include <mitkDataStorage.h>
#include <mitkPixelTypeTraits.h>

//includes storing fiberdata
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkTransform.h>
#include <vtkFloatArray.h>
#include <itkScalableAffineTransform.h>

namespace mitk {

/**
   * \brief Base Class for Fiber Bundles;   */
class FiberBundle_EXPORT FiberBundle : public BaseData
{
public:

    typedef itk::Image<unsigned char, 3> ItkUcharImgType;

    // fiber colorcodings
    static const char* FIBER_ID_ARRAY;

    void UpdateOutputInformation() override;
    void SetRequestedRegionToLargestPossibleRegion() override;
    bool RequestedRegionIsOutsideOfTheBufferedRegion() override;
    bool VerifyRequestedRegion() override;
    void SetRequestedRegion(const itk::DataObject*) override;

    mitkClassMacro( FiberBundle, BaseData )
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)
    mitkNewMacro1Param(Self, vtkSmartPointer<vtkPolyData>) // custom constructor

    // colorcoding related methods
    static vtkSmartPointer<vtkUnsignedCharArray> GetNewColorArray(const unsigned int);
    bool IsInOrientationColoring() const { return m_IsInOrientationColoring; }
    virtual void ColorFibersByCurvature(const bool minMaxNorm = true);
    void ColorFibersByLength(const bool opacity, const bool normalize);
    void ColorFibersByScalarMap(mitk::Image::Pointer, const bool opacity, const bool normalize);
    template <typename TPixel>
    void ColorFibersByScalarMap(const mitk::PixelType pixelType, mitk::Image::Pointer, const bool opacity, const bool normalize);
    virtual void ColorFibersByOrientation();
    void SetFiberOpacity(vtkDoubleArray *FAValArray);
    void ResetFiberOpacity();
    virtual void SetFiberColors(vtkSmartPointer<vtkUnsignedCharArray> fiberColors);
    virtual void SetFiberColors(float r, float g, float b, float alpha = 255);
    vtkSmartPointer<vtkUnsignedCharArray> GetFiberColors() const { return m_FiberColors; }

    // fiber resampling
    void ResampleSpline(float pointDistance = 1);
    void ResampleSpline(float pointDistance, double tension, double continuity, double bias);
    void ResampleLinear(double pointDistance = 1);

    virtual void MirrorFibers(unsigned int axis);

    // get/set data
    float GetFiberLength(int index) const { return m_FiberLengths.at(index); }
    void SetFiberPolyData(vtkSmartPointer<vtkPolyData>, bool updateGeometry = true);
    vtkSmartPointer<vtkPolyData> GetFiberPolyData() const;
    itkGetConstMacro( NumFibers, int)
    itkGetConstMacro( MinFiberLength, float )
    itkGetConstMacro( MaxFiberLength, float )
    itkGetConstMacro( MeanFiberLength, float )
    itkGetConstMacro( MedianFiberLength, float )
    itkGetConstMacro( LengthStDev, float )
    itkGetConstMacro( UpdateTime2D, itk::TimeStamp )
    itkGetConstMacro( UpdateTime3D, itk::TimeStamp )
    void RequestUpdate2D(){ m_UpdateTime2D.Modified(); }
    void RequestUpdate3D(){ m_UpdateTime3D.Modified(); }
    void RequestUpdate(){ m_UpdateTime2D.Modified(); m_UpdateTime3D.Modified(); }

    unsigned long GetNumberOfPoints() const;
    const std::vector<float> &GetFiberLengths() const { return m_FiberLengths; }

    // copy fiber bundle
    mitk::FiberBundle::Pointer GetDeepCopy();

    // compare fiber bundles
    bool Equals(FiberBundle* fib, double eps=0.01);

    itkSetMacro( ReferenceGeometry, mitk::BaseGeometry::Pointer )
    itkGetConstMacro( ReferenceGeometry, mitk::BaseGeometry::Pointer )

    vtkSmartPointer<vtkPolyData>    GeneratePolyDataByIds(std::vector<long> fiberIds);

protected:

    FiberBundle( vtkPolyData* fiberPolyData = nullptr );
    ~FiberBundle() override;

    itk::Point<float, 3>            GetItkPoint(double point[3]);
    void                            UpdateFiberGeometry();
    void                    PrintSelf(std::ostream &os, itk::Indent indent) const override;

    // actual fiber container
    vtkSmartPointer<vtkPolyData>  m_FiberPolyData;

    int   m_NumFibers;

    vtkSmartPointer<vtkUnsignedCharArray> m_FiberColors;
    std::vector< float > m_FiberLengths;
    float   m_MinFiberLength;
    float   m_MaxFiberLength;
    float   m_MeanFiberLength;
    float   m_MedianFiberLength;
    float   m_LengthStDev;
    bool m_IsInOrientationColoring;
    itk::TimeStamp m_UpdateTime2D;
    itk::TimeStamp m_UpdateTime3D;
    mitk::BaseGeometry::Pointer m_ReferenceGeometry;
};

} // namespace mitk

#endif /*  _MITK_FiberBundle_H */
