#ifndef DUNE_STUFF_FILE_IO_HH
#define DUNE_STUFF_FILE_IO_HH

#ifdef HAVE_CMAKE_CONFIG
 #include "cmake_config.h"
#else
 #include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#include <string>
#include <fstream>

#include <dune/grid/common/grid.hh>
#include <dune/stuff/common/string.hh>
#include <dune/fem/gridpart/common/gridpart.hh>
#include <dune/fem/io/file/vtkio.hh>
#include <dune/common/deprecated.hh>
#include <iostream>

namespace Dune {
namespace Stuff {
namespace Fem {

/** \todo FELIX needs to doc me **/
template< class DiscreteFunctionType>
int saveDiscreteFunction(const DiscreteFunctionType& discreteFunction,
                         const std::string gridType,
                         const int refineLevel,
                         const std::string dgfFilename,
                         const std::string saveToFilenamePrefix,
                         std::ostream& error) DUNE_DEPRECATED_MSG("To be removed by 2012's end")
{
  int errorState(0);

  error.flush();

  if ( !( discreteFunction.write_ascii( saveToFilenamePrefix + std::string(".ddf") ) ) )
  {
    ++errorState;
    error << "Error in saveDiscreteFunction(): could not write to " << saveToFilenamePrefix + std::string(".ddf")
          << "!" << std::endl;
    return errorState;
  }

  // save dgf file with information
  std::ifstream readFile( dgfFilename.c_str() );
  if ( readFile.is_open() )
  {
    std::ofstream writeFile( std::string( saveToFilenamePrefix + std::string(".dgf") ).c_str() );
    if ( writeFile.is_open() )
    {
      // shift until "DGF"
      while ( !( readFile.eof() ) )
      {
        std::string line("");
        std::getline(readFile, line);
        if ( line.size() )
        {
          // remove lines with redundant information == "# ddf_"
          if ( !( (line[0] == '#') && (line[1] == ' ') && (line[2] == 'd') && (line[3] == 'd') && (line[4] == 'f')
                  && (line[5] == '_') ) )
          {
            writeFile << line << std::endl;
          }
          if ( (line[0] == 'D') && (line[1] == 'G') && (line[2] == 'F') )
          {
            break;
          }
        }
      }
      // insert informations
      writeFile << "# ddf_gridtype: " << gridType << std::endl;
      writeFile << "# ddf_refine_level: " << refineLevel << std::endl;
      // rest of dgf file
      while ( !( readFile.eof() ) )
      {
        std::string line("");
        std::getline(readFile, line);
        if ( line.size() )
        {
          // remove lines with redundant information == "# ddf_"
          if ( !( (line[0] == '#') && (line[1] == ' ') && (line[2] == 'd') && (line[3] == 'd') && (line[4] == 'f')
                  && (line[5] == '_') ) )
          {
            writeFile << line << std::endl;
          }
        }
      }
      writeFile.flush();
      writeFile.close();
      readFile.close();
    } else {
      ++errorState;
      error << "Error in saveDiscreteFunction(): could not write to " << saveToFilenamePrefix + std::string(".dgf")
            << "!" << std::endl;
      return errorState;
    }
  } else {
    ++errorState;
    error << "Error in saveDiscreteFunction(): could not open " << dgfFilename << "!" << std::endl;
    return errorState;
  }
  return errorState;
} // saveDiscreteFunction

/** \todo FELIX needs to doc me **/
template< class DiscreteFunctionType >
int loadDiscreteFunction(const std::string loadFromfilenamePrefix,
                         std::ostream& error) DUNE_DEPRECATED_MSG("To be removed by 2012's end")
{
  int errorState(0);

  error.flush();

  std::string gridType("");
  int refineLevel(0);

  bool gridTypeRead(false);
  bool refineLevelRead(false);

  std::ifstream readFile( std::string( loadFromfilenamePrefix + std::string(".dgf") ).c_str() );
  if ( readFile.is_open() )
  {
    while ( !( readFile.eof() ) )
    {
      if ( !(gridTypeRead && refineLevelRead) )
      {
        std::string line("");
        std::getline(readFile, line);
        if ( line.size() )
        {
          if (line.substr(0, 6) == "# ddf_")
          {
            unsigned int key_start = 6;
            unsigned int key_end = key_start;
            for ( ; key_end < line.size(); ++key_end)
            {
              const char& c = line[key_end];
              if ( (c == ' ') || (c == '\t') || (c == ':') )
              {
                break;
              }
            }
            std::string key = line.substr(key_start, key_end - key_start);
            unsigned int value_start = key_end;
            for ( ; value_start < line.size(); ++value_start)
            {
              if (line[value_start] == ':')
              {
                break;
              }
            }
            ++value_start;
            for ( ; value_start < line.size(); ++value_start)
            {
              if ( (line[value_start] != ' ') && (line[value_start] != '\t') )
              {
                break;
              }
            }
            if ( value_start >= line.size() )
            {
              ++errorState;
            }
            std::string value = line.substr(value_start, line.size() - value_start);
            if (key == "gridtype")
            {
              gridType = value;
              gridTypeRead = true;
            } else if (key == "refine_level") {
              refineLevel = Dune::Stuff::Common::fromString< int >(value);
              refineLevelRead = true;
            }
          }
        }
      } else {
        break;
      }
    }
  } else {
    ++errorState;
    error << "Error: could not open "
          << std::string( loadFromfilenamePrefix + std::string(".dgf") ) << "!" << std::endl;
    return errorState;
  }

  if (gridTypeRead && refineLevelRead)
  {
    typedef typename DiscreteFunctionType::FunctionSpaceType
    DiscreteFunctionSpaceType;

    typedef typename DiscreteFunctionSpaceType::GridPartType
    GridPartType;

    typedef typename GridPartType::GridType
    GridType;

    typedef Dune::GridPtr< GridType >
    GridPointerType;

    GridPointerType gridPointer( std::string( loadFromfilenamePrefix + std::string(".dgf") ) );
    gridPointer->globalRefine(refineLevel);
    GridPartType gridPart(*gridPointer);

    DiscreteFunctionSpaceType discreteFunctionSpace(gridPart);

    DiscreteFunctionType discreteFunction(loadFromfilenamePrefix, discreteFunctionSpace);

    const bool readFromAscii = discreteFunction.read_ascii( std::string( loadFromfilenamePrefix + std::string(".ddf") ) );

    if (readFromAscii)
    {
      Dune::VTKIO< GridPartType > vtkWriter(gridPart);
      const std::string vtkWriterFilename = std::string("data/saved_") + loadFromfilenamePrefix;
      vtkWriter.addVectorVertexData(discreteFunction);
      vtkWriter.write( vtkWriterFilename.c_str() );
      vtkWriter.clear();
    } else {
      ++errorState;
      error << "Error: could not read from "
            << std::string( loadFromfilenamePrefix + std::string(".ddf") ) << "!" << std::endl;
      return errorState;
    }
  } else {
    ++errorState;
    error << "Error: one of the following lines is missing in "
          << std::string( loadFromfilenamePrefix + std::string(".dgf") ) << "!" << std::endl;
    error << "\t# ddf_gridtype: GRIDTYPE" << std::endl;
    error << "\t# ddf_refine_level: REFINELEVEL" << std::endl;
    return errorState;
  }
  return errorState;
} // loadDiscreteFunction

/** \todo FELIX needs to doc me **/
int readRefineLevelFromDGF(const std::string filename) {
  std::string gridType("");
  int refineLevel(0);

  bool gridTypeRead(false);
  bool refineLevelRead(false);

  std::ifstream readFile( filename.c_str() );

  if ( readFile.is_open() )
  {
    while ( !( readFile.eof() ) )
    {
      if ( !(gridTypeRead && refineLevelRead) )
      {
        std::string line("");
        std::getline(readFile, line);
        if ( line.size() )
        {
          if (line.substr(0, 6) == "# ddf_")
          {
            unsigned int key_start = 6;
            unsigned int key_end = key_start;
            for ( ; key_end < line.size(); ++key_end)
            {
              const char& c = line[key_end];
              if ( (c == ' ') || (c == '\t') || (c == ':') )
              {
                break;
              }
            }
            std::string key = line.substr(key_start, key_end - key_start);
            unsigned int value_start = key_end;
            for ( ; value_start < line.size(); ++value_start)
            {
              if (line[value_start] == ':')
              {
                break;
              }
            }
            ++value_start;
            for ( ; value_start < line.size(); ++value_start)
            {
              if ( (line[value_start] != ' ') && (line[value_start] != '\t') )
              {
                break;
              }
            }
            if ( value_start >= line.size() )
            {
              --refineLevel;
            }
            std::string value = line.substr(value_start, line.size() - value_start);
            if (key == "gridtype")
            {
              gridType = value;
              gridTypeRead = true;
            } else if (key == "refine_level") {
              refineLevel = Dune::Stuff::Common::fromString< int >(value);
              refineLevelRead = true;
              return refineLevel;
            }
          }
        }
      } else {
        break;
      }
    }
  } else {
    --refineLevel;
    std::cerr << "Error: could not open " << filename << "!" << std::endl;
    return refineLevel;
  }
  return refineLevel;
} // readRefineLevelFromDGF


std::string readGridTypeFromDGF(const std::string filename) DUNE_DEPRECATED_MSG(
  "will be removed with dune-stuff 2.3");

std::string readGridTypeFromDGF(const std::string filename) {
  std::string gridType("no_gridtype_found_in " + filename);
  int refineLevel(0);

  bool gridTypeRead(false);
  bool refineLevelRead(false);

  std::ifstream readFile( filename.c_str() );

  if ( readFile.is_open() )
  {
    while ( !( readFile.eof() ) )
    {
      if ( !(gridTypeRead && refineLevelRead) )
      {
        std::string line("");
        std::getline(readFile, line);
        if ( line.size() )
        {
          if (line.substr(0, 6) == "# ddf_")
          {
            unsigned int key_start = 6;
            unsigned int key_end = key_start;
            for ( ; key_end < line.size(); ++key_end)
            {
              const char& c = line[key_end];
              if ( (c == ' ') || (c == '\t') || (c == ':') )
              {
                break;
              }
            }
            std::string key = line.substr(key_start, key_end - key_start);
            unsigned int value_start = key_end;
            for ( ; value_start < line.size(); ++value_start)
            {
              if (line[value_start] == ':')
              {
                break;
              }
            }
            ++value_start;
            for ( ; value_start < line.size(); ++value_start)
            {
              if ( (line[value_start] != ' ') && (line[value_start] != '\t') )
              {
                break;
              }
            }
            if ( value_start >= line.size() )
            {
              return gridType;
            }
            std::string value = line.substr(value_start, line.size() - value_start);
            if (key == "gridtype")
            {
              gridType = value;
              gridTypeRead = true;
            } else if (key == "refine_level") {
              refineLevel = Dune::Stuff::Common::fromString< int >(value);
              refineLevelRead = true;
            }
          }
        }
      } else {
        break;
      }
    }
  } else {
    std::cerr << "Error: could not open " << filename << "!" << std::endl;
    return gridType;
  }
  return gridType;
} // readGridTypeFromDGF

} // end namespace Fem
} // end namespace Stuff
} // end namespace Dune

/** Copyright (c) 2012, Felix Albrecht
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/


#endif // FILE_IO_HH
