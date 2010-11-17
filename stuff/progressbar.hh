#ifndef __PROGRESSBAR_HH__
#define __PROGRESSBAR_HH__

#include <iostream>
#include <string>
#include <cc++/thread.h> //threading class

#include "misc.hh"


namespace Stuff {

template < class OutputStream >
class SimpleProgressBar;


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
	template < class OutputStream = std::ostream >
  class ProgressBar : public ost::Thread { //every threaded class must be inherited from Thread class
    
  public:
    /** @brief constructor that initializes the value for percent to zero.
     *
     *  @param[in] numBlocks The width of the progress bar, default is 100
     */
    ProgressBar(OutputStream& stream = std::cout, const int numBlocks=100)
	    : percent(0),
	    numBlocks_(numBlocks),
	    output_stream_( stream )
    {
      // start the thread now
      start(); 
    }

	static const unsigned sleepTime = 500;

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
        output_stream_ << "\r[" << bar << "]";
        /*output 4 characters, in the next output to cout,
          one for space, and three for the numbers*/
        output_stream_ << std::setw(4) << percent << "%";
		output_stream_.flush();
        //break when reaching 100%
        if (percent==100) 
          break; 
        //interval between updates, in miliseconds
        Thread::sleep(sleepTime);
      }
      output_stream_ << std::endl;
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
	OutputStream& output_stream_;

	friend class SimpleProgressBar<OutputStream>;
  };

  /** \brief wrap a ProgessBar with discrete Increments
		Example usage: \code
			unsigned k = 10;
			Stuff::SimpleProgressBar<> pbar(k);
			for(int i=0; i<k+1; i++,++pbar) {
				sleep( 1 );
			}
			++pbar;
		\endcode
	**/
  template < class OutputStream = std::ostream >
  class SimpleProgressBar {
	  typedef SimpleProgressBar<OutputStream >
		ThisType;
	public:
	  /** \param[in]	increments maximum number of increments this bar will receive.
						Set it to number of entites in your gridwalk for example if you want  to map the gridwalk's progress.
		**/
	  SimpleProgressBar(const unsigned int increments, OutputStream& stream = std::cout, const int numBlocks=100 )
			: increments_(increments),
			progress_bar_(stream, numBlocks),
			current_step_(0),
			stream_( stream )
	  {}
	  ~SimpleProgressBar()
	  {
		  increment();
		  progress_bar_.sleep(ProgressBar<OutputStream>::sleepTime);
		  progress_bar_.terminate();
	  }

	  void increment() {
		  current_step_ = Stuff::clamp( ++current_step_, unsigned(0), increments_ );
		  set();
	  }

	  ThisType& operator++()
	  {
		  increment();
		  return *this;
	  }
	  private:
		void set() {
			unsigned pr = ceil( 100.0 * ( double(current_step_)/double(increments_) ) );
			progress_bar_.setPercent( pr );
		}

	private:
		const unsigned int increments_;
		ProgressBar<OutputStream> progress_bar_;
		unsigned int current_step_;
		OutputStream& stream_;
  };


 
} // namespace Stuff

#endif /* __PROGRESSBAR_HH__ */
