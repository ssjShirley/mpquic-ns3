/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 University of Washington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mitch Watrous (watrous@u.washington.edu)
 */

#include <fstream>
#include <iostream>
#include <regex>

#include "ns3/gnuplot.h"

using namespace ns3;

namespace {

//===========================================================================
// Function: Create2DPlotFile
//
//
// This function creates a 2-D plot file.
//===========================================================================


void Create2DPlotFile ()
{
  std::string fileNameWithNoExtension = "rtt-cpr";
  std::string graphicsFileName        = fileNameWithNoExtension + ".png";
  std::string plotFileName            = fileNameWithNoExtension + ".plt";
  std::string plotTitle               = " ";
  std::string dataTitle0               = "subflow 0";
  std::string dataTitle1               = "subflow 1";
  // std::string dataTitle2               = "MPQUIC-OLIA";
  // std::string dataTitle3               = "olia-path 1";
  // std::string dataTitle4               = "BBR";

  // Instantiate the plot and set its title.
  Gnuplot plot (graphicsFileName);
  plot.SetTitle (plotTitle);

  // Make the graphics file, which the plot file will create when it
  // is used with Gnuplot, be a PNG file.
  plot.SetTerminal ("png");

  // Set the labels for each axis.
  plot.SetLegend ("Time (s)", "RTT (s)");
  // plot.AppendExtra ("set yrange [0.025:+0.04]");
  // Set the range for the x axis.
  // plot.AppendExtra ("set xrange [0:+10000]");

  // Instantiate the dataset, set its title, and make the points be
  // plotted along with connecting lines.
  Gnuplot2dDataset dataset;
  dataset.SetTitle (dataTitle0);
  dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);
  // Gnuplot2dDataset dataset1;
  // dataset1.SetTitle (dataTitle1);
  // dataset1.SetStyle (Gnuplot2dDataset::LINES_POINTS);


  std::string line;
  // std::ifstream myfile ("clientQUIC-cwnd-change-p0-63.txt");
  std::ifstream myfile ("clientQUIC-rtt-change-p0.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      std::regex regex{R"([\s,]+)"}; // split on space and comma
      std::sregex_token_iterator it{line.begin(), line.end(), regex, -1};
      std::vector<std::string> words{it, {}};
      dataset.Add (atof(words[0].c_str()), atof(words[1].c_str()));
      // dataset1.Add (atof(words[0].c_str()), );
    }
    myfile.close();
  }

  else std::cout << "Unable to open file"; 


  Gnuplot2dDataset dataset1;
  dataset1.SetTitle (dataTitle1);
  dataset1.SetStyle (Gnuplot2dDataset::LINES_POINTS);
  // Gnuplot2dDataset dataset3;
  // dataset3.SetTitle (dataTitle3);
  // dataset3.SetStyle (Gnuplot2dDataset::LINES_POINTS);

  // std::ifstream myfile1 ("clientQUIC-cwnd-change-p1-63.txt");
  std::ifstream myfile1 ("clientQUIC-rtt-change-p1.txt");
  if (myfile1.is_open())
  {
    while ( getline (myfile1,line) )
    {
      std::regex regex{R"([\s,]+)"}; // split on space and comma
      std::sregex_token_iterator it{line.begin(), line.end(), regex, -1};
      std::vector<std::string> words{it, {}};
      dataset1.Add (atof(words[0].c_str()), atof(words[1].c_str()));
      // dataset3.Add (atof(words[0].c_str())-1, );
    }
    myfile1.close();
  }

  else std::cout << "Unable to open file"; 


  // Gnuplot2dDataset dataset4;
  // dataset4.SetTitle (dataTitle4);
  // dataset4.SetStyle (Gnuplot2dDataset::LINES_POINTS);


  // std::ifstream myfile2 ("bbr-thp.txt");
  // if (myfile2.is_open())
  // {
  //   while ( getline (myfile2,line) )
  //   {
  //     std::regex regex{R"([\s,]+)"}; // split on space and comma
  //     std::sregex_token_iterator it{line.begin(), line.end(), regex, -1};
  //     std::vector<std::string> words{it, {}};
   
  //     dataset4.Add (atof(words[0].c_str())-1, atof(words[1].c_str()));

  //   }
  //   myfile2.close();
  // }

  // else std::cout << "Unable to open file"; 


  // Add the dataset to the plot.
  plot.AddDataset (dataset);
  plot.AddDataset (dataset1);
  // plot.AddDataset (dataset2);
  // plot.AddDataset (dataset3);
  // plot.AddDataset (dataset4);

  // Open the plot file.
  std::ofstream plotFile (plotFileName.c_str());

  // Write the plot file.
  plot.GenerateOutput (plotFile);

  // Close the plot file.
  plotFile.close ();
}

}  // unnamed namespace


int main (int argc, char *argv[])
{
  // Create a 2-D plot file.
  Create2DPlotFile();
  return 0;
}
