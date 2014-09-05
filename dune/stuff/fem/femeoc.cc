// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include <config.h>

#include <boost/numeric/conversion/cast.hpp>

#include <dune/stuff/fem/namespace.hh>

#include "femeoc.hh"

#if HAVE_DUNE_FEM

namespace Dune {
namespace Stuff {
namespace Fem {

FemEoc::FemEoc()
  : outputFile_()
    , level_(0)
    , prevError_(0)
    , error_(0)
    , description_(0)
    , prevh_(0)
    , initial_(true)
    , pos_(0)
{}

FemEoc::~FemEoc() {
  outputFile_.close();
}

void FemEoc::init(const std::string& path,
          const std::string& name, const std::string& descript, const std::string& inputFile) {
  if (!Dune::Fem::directoryExists(path) && !Dune::Fem::createDirectory(path))
    DUNE_THROW( Dune::IOError, (boost::format("couldn't create directory") % path).str() );
  init(path + "/" + name, descript, path + "/" + inputFile);
}

void FemEoc::init(const std::string& name, const std::string& descript, const std::string& inputFile) {
  if ( !outputFile_.is_open() )
  {
    std::ofstream main( (name + "_main.tex").c_str() );
    std::ifstream input( inputFile.c_str() );
    if (!main)
    {
      std::cerr << "Could not open file : "
                << (name + "_main.tex").c_str()
                << " ... ABORTING" << std::endl;
      abort();
    }

    std::ostringstream filestreamBody;
    filestreamBody << name << "_body.tex";
    outputFile_.open(filestreamBody.str().c_str(), std::ios::out);
    std::string bodyfile = filestreamBody.str().substr(filestreamBody.str().find(
                                                         '/') != std::string::npos ? filestreamBody.str().find(
                                                         '/') + 1 : 0);

    if (!input)
    {
      std::cerr << "Could not open file : "
                << inputFile
                << " ... using default template" << std::endl;
      main << "\\documentclass[10pt,english]{article}\n"
           << "\\usepackage{fontenc}\n"
           << "\\usepackage{vmargin}\n"
           << "\\usepackage{longtable}\n"
           << "\\setpapersize[landscape]{A4}\n"
           << "\\usepackage[latin1]{inputenc}\n"
           << "\\usepackage{setspace}\n"
           << "\\onehalfspacing\n"
           << "\\makeatletter\n"
           << "\\providecommand{\\boldsymbol}[1]{\\mbox{\\boldmath $#1$}}\n"
           << "\\providecommand{\\tabularnewline}{\\\\}\n"
           << "\\usepackage{babel}\n"
           << "\\makeatother\n"
           << "\\begin{document}\n"
           << "\\begin{center}\\large\n"
           << descript
           << "\n\\end{center}\n\n"
           << "\\input{"
           << bodyfile
           << "}\n"
           << "\\end{document}\n" << std::endl;
    } else {
      std::stringstream inputTex;
      while ( input.good() )
      {
        inputTex << (char) input.get();
      }
      std::string input_str = inputTex.str();
      int pos = boost::numeric_cast< int >(input_str.find("DESCRIPTION", 0));
      input_str.replace(pos, 11, "");
      input_str.insert(pos, descript);

      pos = boost::numeric_cast< int >(input_str.find("BODYFILE", 0));
      input_str.replace(pos, 8, "");
      input_str.insert(pos, bodyfile);

      main << input_str;
    }
    main.close();
  } else {
    DUNE_THROW(Dune::InvalidStateException, "");
  }
} // init


size_t FemEoc::addentry(const std::string& descript) {
  if (!initial_)
    DUNE_THROW(Dune::InvalidStateException, "");
  pos_.push_back(boost::numeric_cast< int >(error_.size()));
  error_.push_back(0);
  prevError_.push_back(0);
  description_.push_back(descript);
  return pos_.size() - 1;
} // addentry

void FemEoc::seterrors(size_t id, const double& err) {
  int pos = pos_[id];

  error_[pos] = err;
}

void FemEoc::writeerr(double h, double size, double time, int counter) {
  if (initial_)
  {
    outputFile_ << "\\begin{tabular}{|c|c|c|c|c|";
    for (unsigned int i = 0; i < error_.size(); i++)
    {
      outputFile_ << "|cc|";
    }
    outputFile_ << "}\n"
                << "\\hline \n"
                << "level & h & size & CPU-time & counter";
    for (unsigned int i = 0; i < error_.size(); i++)
    {
      outputFile_ << " & " << description_[i]
                  << " & EOC ";
    }
    outputFile_ << "\n \\tabularnewline\n"
                << "\\hline\n"
                << "\\hline\n";
  }
  outputFile_ << "\\hline \n"
              << level_ << " & "
              << h << " & "
              << size << " & "
              << time << " & "
              << counter;
  for (unsigned int i = 0; i < error_.size(); ++i)
  {
    outputFile_ << " & " << error_[i] << " & ";
    if (initial_)
    {
      outputFile_ << " --- ";
    } else {
      double factor = prevh_ / h;
      outputFile_ << log(prevError_[i] / error_[i]) / log(factor);
    }
    prevError_[i] = error_[i];
    error_[i] = -1;  // uninitialized
  }
  outputFile_ << "\n"
              << "\\tabularnewline\n"
              << "\\hline \n";
  outputFile_.flush();
  prevh_ = h;
  level_++;
  initial_ = false;
} // writeerr


} // namespace Stuff
} // namespace Fem
} // namespace Dune

#endif // HAVE_DUNE_FEM
