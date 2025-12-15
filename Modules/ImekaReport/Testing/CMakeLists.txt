MITK_CREATE_MODULE_TESTS(LABELS ReportTests)

mitkAddCustomModuleTest(CSVWriterTest1 CSVWriterTest
  ${PLITK_DATA_DIR}/groundtruth)
