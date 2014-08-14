// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include "tree.hh"

namespace Dune {
namespace Stuff {
namespace Common {

ExtendedParameterTree::ExtendedParameterTree()
{}

ExtendedParameterTree::ExtendedParameterTree(const std::string filename)
  : BaseType(init(filename))
{}

ExtendedParameterTree::ExtendedParameterTree(int argc, char** argv, std::string filename)
  : ExtendedParameterTree::BaseType(init(argc, argv, filename))
{}

ExtendedParameterTree::ExtendedParameterTree(const Dune::ParameterTree& other)
  : ExtendedParameterTree::BaseType(other)
{}

ExtendedParameterTree& ExtendedParameterTree::operator=(const Dune::ParameterTree& other)
{
  if (this != &other) {
    BaseType::operator=(other);
  }
  return *this;
} // ExtendedParameterTree& operator=(const Dune::ParameterTree& other)

/**
 *  \brief adds another Dune::ParameterTree
 */
void ExtendedParameterTree::add(const Dune::ParameterTree& _other, const std::string _subName /*= ""*/, bool overwrite /*= false*/)
{
  if (_subName.empty()) {
    // copy each key/value pair and append subName
    const Dune::ParameterTree::KeyVector& keyVector = _other.getValueKeys();
    for (size_t ii = 0; ii < keyVector.size(); ++ii) {
      const std::string key = keyVector[ii];
      if (BaseType::hasKey(key) && !overwrite && (BaseType::operator[](key) != _other[key]))
        DUNE_THROW(Dune::InvalidStateException,
                   "\n" << DSC::colorStringRed("ERROR:")
                   << " key '" << key << "' already exists in the following Dune::ParameterTree:\n"
                   << reportString("  "));
      BaseType::operator[](key) = _other.get< std::string >(key);
    }
  } else {
      if (BaseType::hasSub(_subName)) {
      ExtendedParameterTree _sub = BaseType::sub(_subName);
      _sub.add(_other, "", overwrite);
      BaseType::sub(_subName) = _sub;
    } else
      BaseType::sub(_subName) = _other;
  }
} // ... add(...)

ExtendedParameterTree ExtendedParameterTree::sub(const std::string& _sub) const
{
  if (!hasSub(_sub))
      DUNE_THROW(Dune::RangeError,
                 "\n" << DSC::colorStringRed("ERROR:")
                 << " sub '" << _sub << "' missing in the following Dune::ParameterTree:\n" << reportString("  "));
  return ExtendedParameterTree(BaseType::sub(_sub));
}

void ExtendedParameterTree::report(std::ostream& stream /*= std::cout*/, const std::string& prefix /*= ""*/) const
{
  reportAsSub(stream, prefix, "");
} // void report(std::ostream& stream = std::cout, const std::string& prefix = "") const

void ExtendedParameterTree::reportNicely(std::ostream& stream /*= std::cout*/) const
{
  if (valueKeys.size() == 0 || subKeys.size() == 0) {
    const std::string commonPrefix = findCommonPrefix(*this);
    if (!commonPrefix.empty()) {
      stream << "[" << commonPrefix << "]" << std::endl;
      const ExtendedParameterTree& commonSub = sub(commonPrefix);
      reportFlatly(commonSub, "", stream);
    }
  } else
    reportAsSub(stream, "", "");
}

std::string ExtendedParameterTree::reportString(const std::string& prefix /*= ""*/) const
{
  std::stringstream stream;
  report(stream, prefix);
  return stream.str();
} // std::stringstream reportString(const std::string& prefix = "") const

bool ExtendedParameterTree::hasVector(const std::string& key) const
{
  if (hasKey(key)) {
    const std::string str = BaseType::get< std::string >(key, "meaningless_default_value");
    return (str.substr(0, 1) ==  "["
        && str.substr(str.size() - 1, 1) == "]");
  }
  return false;
} // bool hasVector(const std::string& vector) const

void ExtendedParameterTree::assertKey(const std::string& key) const
{
  if (!BaseType::hasKey(key))
    DUNE_THROW(Dune::RangeError,
               "\n" << DSC::colorStringRed("ERROR:")
               << " key '" << key << "' missing  in the following Dune::ParameterTree:\n" << reportString("  "));
}

void ExtendedParameterTree::assertSub(const std::string& _sub) const
{
  if (_sub.empty())
    DUNE_THROW_COLORFULLY(Exceptions::configuration_error, "Given sub_id must not be empty!");
  if (!BaseType::hasSub(_sub))
    DUNE_THROW(Dune::RangeError,
               "\n" << DSC::colorStringRed("ERROR:")
               << " sub '" << _sub << "' missing  in the following Dune::ParameterTree:\n" << reportString("  "));
}

/**
  \brief      Fills a Dune::ParameterTree given a parameter file or command line arguments.
  \param[in]  argc
              From \c main()
  \param[in]  argv
              From \c main()
  \return     The Dune::ParameterTree that is to be filled.
  **/
ParameterTree ExtendedParameterTree::init(int argc, char** argv, std::string filename)
{
  Dune::ParameterTree paramTree;
  if (argc == 1) {
    Dune::ParameterTreeParser::readINITree(filename, paramTree);
  } else if (argc == 2) {
    Dune::ParameterTreeParser::readINITree(argv[1], paramTree);
  } else {
    Dune::ParameterTreeParser::readOptions(argc, argv, paramTree);
  }
  if (paramTree.hasKey("paramfile")) {
    Dune::ParameterTreeParser::readINITree(paramTree.get< std::string >("paramfile"), paramTree, false);
  }
  return paramTree;
} // static ExtendedParameterTree init(...)

ParameterTree ExtendedParameterTree::init(const std::string filename)
{
  Dune::ParameterTree paramTree;
  Dune::ParameterTreeParser::readINITree(filename, paramTree);
  return paramTree;
} // static ExtendedParameterTree init(...)

void ExtendedParameterTree::reportAsSub(std::ostream& stream, const std::string& prefix, const std::string& subPath) const
{
  for (auto pair : values)
    stream << prefix << pair.first << " = " << pair.second << std::endl;
//      stream << prefix << pair.first << " = \"" << pair.second << "\"" << std::endl;
  for (auto pair : subs) {
    ExtendedParameterTree subTree(pair.second);
    if (subTree.getValueKeys().size())
      stream << prefix << "[" << subPath << pair.first << "]" << std::endl;
    subTree.reportAsSub(stream, prefix, subPath + pair.first + ".");
  }
} // void reportAsSub(std::ostream& stream = std::cout, const std::string& prefix = "") const

std::string ExtendedParameterTree::findCommonPrefix(const BaseType& subtree, const std::string previousPrefix /*= ""*/) const
{
  const auto& valuekeys = subtree.getValueKeys();
  const auto& subkeys = subtree.getSubKeys();
  if (valuekeys.size() == 0 && subkeys.size() == 1) {
    // we append the subs name
    if (previousPrefix.empty())
      return findCommonPrefix(subtree.sub(subkeys[0]), subkeys[0]);
    else
      return findCommonPrefix(subtree.sub(subkeys[0]), previousPrefix + "." + subkeys[0]);
  } else {
    // end of the recursion, return the previous prefix
    return previousPrefix;
  }
}

void ExtendedParameterTree::reportFlatly(const BaseType& subtree, const std::string& prefix /*= ""*/,
                                         std::ostream& stream /*= std::cout*/) const
{
  // report all the keys
  for (auto key : subtree.getValueKeys())
    stream << prefix << key << " = " << subtree[key] << std::endl;
  // report all the subs
  for (auto subkey : subtree.getSubKeys()) {
    if (prefix.empty())
      reportFlatly(subtree.sub(subkey), subkey + ".", stream);
    else
      reportFlatly(subtree.sub(subkey), prefix + subkey+ "." , stream);
  }
}

} // namespace Common
} // namespace Stuff
} // namespace Dune
