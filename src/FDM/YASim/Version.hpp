#ifndef _VERSION_HPP
#define _VERSION_HPP

#include <string>
#include <type_traits>

namespace yasim {

enum class YASIM_VERSION : int {
  ORIGINAL = 0,
  V_32,
  V_2017_2,
  V_2018_1,
  CURRENT = V_2018_1
};

class Version {
public:
  Version() : _version(YASIM_VERSION::ORIGINAL) {}
  virtual ~Version() {}

  void setVersion( const char * version );
  std::underlying_type<YASIM_VERSION>::type getVersion() const;
  bool isVersion( YASIM_VERSION version ) const;
  bool isVersionOrNewer( YASIM_VERSION version ) const;
  static YASIM_VERSION getByName(const std::string& name);
  static std::string getName(YASIM_VERSION v);
private:
  using versionUnderlyingType = std::underlying_type<YASIM_VERSION>::type;
  YASIM_VERSION _version;
};

inline bool Version::isVersion( YASIM_VERSION version ) const
{
  return _version == version;
}

inline bool Version::isVersionOrNewer( YASIM_VERSION version ) const
{
  return _version >= version;
}

std::ostream& operator<<(std::ostream& os, const YASIM_VERSION& version);

}; // namespace yasim
#endif // _WING_HPP
