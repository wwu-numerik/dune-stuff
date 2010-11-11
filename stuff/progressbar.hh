#ifndef __PROGRESSBAR_HH__
#define __PROGRESSBAR_HH__

#include <iostream>
#include <string>
#include <cc++/thread.h> //threading class

using namespace ost; //namespace for gnu common c++ library

namespace Stuff {

  /** @brief class realizing a threaded progress bar
   *
   *  Example of usage:
   *  @code
   *  #include <cmath> //for ceil function to round up the percentage
   *  int main() {
   *  progress pbar; //create a progressbar thread, it starts showing from now
   *  //now a time consuming job
   *  for(int i=1; i<100000000; i++) {
   *    pbar.setPercent( ceil((double)i/1000000) ); //set the  percent variable of the progressbar
   *   }
   *  pbar.join(); //wait till pbar thread ends, if it hasn't yet
   *  return 0;
   *  }
   *  @endcode  
   *
   *  @remark Source: http://www.dreamincode.net/code/snippet5166.htm
   *          You have to use -lccgnu2 if you want to link this with gcc.
   *
   */
  class ProgressBar : public Thread { //every threaded class must be inherited from Thread class
    
  public:
    /** @brief constructor that initializes the value for percent to zero.
     *
     *  @param[in] numBlocks The width of the progress bar, default is 100
     */
    ProgressBar(const int numBlocks=100) : percent(0),
                                          numBlocks_(numBlocks)
    {
      // start the thread now
      start(); 
    }

    /** @brief function responsible for displaying and
     *         updating the progress bar
     */
    void run() {
      std::string bar;
      while(percent <= 100) {
        for(int i=0; i < numBlocks_; i++) {
          //add enough = based on the percentage
          if(i <= percent*numBlocks_/100) 
            bar.replace(i, 1, "=");
          else
            //add the spaces for empty area
            bar.replace(i, 1, " "); 
        }
        //print the progress bar
        std::cout << "\r[" << bar << "]";
        /*output 4 characters, in the next output to cout,
          one for space, and three for the numbers*/
        std::cout.width(4); 
        std::cout << percent << "%" << std::flush;
        //break when reaching 100%
        if (percent==100) 
          break; 
        //interval between updates, in miliseconds
        Thread::sleep(500); 
      }
      std::cout << std::endl;
      //when the run function is ended, this thread is over
    }

    /** @brief member function responsible for
     *         setting value to percentage variable.
     *
     *  @param[in] pct The current percentage
     */
    void setPercent(const int& pct) {
      this->percent = pct;
    }
    
  private:
    //member variable holding the percentage, 0-100
    int       percent; 
    const int numBlocks_;
  };
 
} // namespace Stuff

#endif /* __PROGRESSBAR_HH__ */
