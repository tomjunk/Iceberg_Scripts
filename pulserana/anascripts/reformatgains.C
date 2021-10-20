#include <iostream>
#include <fstream>

void printvals(float *gains, int offset, int nvals);
  
int main(int argc, char **argv)
{
  float gain[1280];

  for (int i=0; i<1280; ++i)
    {
      gain[i] = 0;
    }
  
  std::ifstream infile("gains.output",std::ifstream::in);
  while (1)
    {
      int ichan;
      float changain;
      infile >> ichan >> changain;
      if (!infile.good()) break;
      //std::cout << ichan << " " << changain << std::endl;
      if (changain<3000)  // put a cap on crazy values
        {
	  gain[ichan] = changain;
	}
    }

  //for (int i=0; i<1280; ++i)
  // {
  // std::cout << "chan, gain: " << i << " " << gain[i] << std::endl;
  //}

  printvals(gain,0,400);
  printvals(gain,400,400);
  printvals(gain,800,240);
  printvals(gain,1040,240);
  
}

void printvals(float *gain, int offset, int nvals)
{
  std::cout << "Offset: " << offset << std::endl;
  std::cout << "Values: [" << std::endl;
  for (int i=0; i<nvals; ++i)
    {
      float ginv = gain[i+offset];
      if (ginv != 0) ginv = 21.4/ginv;
      std::cout << " " << std::setw(11) << ginv;
      if (i<nvals-1) std::cout << ",";
      if ( (i+1) % 10 == 0 ) std::cout << std::endl;
    }
  std::cout << "]" << std::endl;

}
