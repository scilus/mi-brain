
#ifndef IMEKA_PROPERTY_MATRIX_3D_HPP_INCLUDED
#define IMEKA_PROPERTY_MATRIX_3D_HPP_INCLUDED

#include <mitkGenericProperty.h>
#include <nlohmann/json.hpp>

#include "ImekaCommonExports.h"

namespace mitk
{
  inline void to_json(nlohmann::json& j, const Matrix3D& m)
  {
    j = nlohmann::json::array();
    for (int i = 0; i < 3; ++i)
      for (int k = 0; k < 3; ++k)
        j.push_back(m(i, k));
  }

  inline void from_json(const nlohmann::json& j, Matrix3D& m)
  {
    if (j.is_array() && j.size() == 9)
    {
      for (int i = 0; i < 3; ++i)
        for (int k = 0; k < 3; ++k)
          m(i, k) = j.at(i * 3 + k).get<double>();
    }
  }

  mitkDeclareGenericProperty(Matrix3DProperty, Matrix3D, ImekaCommon_EXPORT);
}

#endif // IMEKA_PROPERTY_MATRIX_3D_HPP_INCLUDED

