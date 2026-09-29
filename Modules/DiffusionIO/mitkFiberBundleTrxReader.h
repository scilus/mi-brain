namespace mitk
{

  /** \brief
  */

  class FiberBundleTrxReader : public AbstractFileReader
  {
  public:

    FiberBundleTrxReader();
    ~FiberBundleTrxReader() override{}
    FiberBundleTrxReader(const FiberBundleTrxReader& other);
    FiberBundleTrxReader * Clone() const override;

    std::vector<itk::SmartPointer<BaseData> > DoRead() override;

  private:

    us::ServiceRegistration<mitk::IFileReader> m_ServiceReg;
  };

} //namespace MITK

#endif // __mitkFiberBundleReader_h