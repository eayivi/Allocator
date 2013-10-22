// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert> // assert
#include <cstddef> // ptrdiff_t, size_t
#include <new>     // new
//#include <cmath>    // abs
#define DISABLE_PRINTF

#ifdef DISABLE_PRINTF
    #define printf(fmt, ...) (0)
#endif

using namespace std;
// ---------
// Allocator
// ---------

template <typename T, int N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        typedef       value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef       value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}                                              // this is correct

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}

    private:
        // ----
        // data
        // ----

        char a[N];

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         */
        bool valid () const {
            // <your code>
            //cout << endl << "Checking block validity, standby..." ;
            int index = 0; // The beginning index of the open sentinel
            int open_sent_value, closing_sent_value;            
            while (index < N) {
                assert (index>=0);
                //cout << endl << "Index is " << index;
				open_sent_value =  my_view(a[index]);
                closing_sent_value =  my_view(a[index+ sizeof(int)  + abs(open_sent_value)]);
                //cout << endl << "opening_sentinel value is " << open_sent_value ;
                //cout << ", closing_sentinel value is " << closing_sent_value << endl; 
                if (open_sent_value != closing_sent_value)
                    return false;

                index = index + 2 * sizeof(int) + abs(open_sent_value); 
                //cout << "num _location is " << index;

            }
            //cout << endl << "$$Exiting valid() " << endl;
            return true;}

        // ------------
        // View 
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */

        int& my_view (char& c) const {
            return *reinterpret_cast<int*>(&c); }
        
        int my_view (const char& c) const {
        return *reinterpret_cast<const int*>(&c); }



    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        Allocator () {
            // <your code>
            my_view(a[0]) = N- 2* sizeof(int);
            my_view(a[N-sizeof(int)]) = N- 2* sizeof(int);
            assert(valid());
        }
        

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);
        

        // --------
        // allocate
        // --------

       /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         */
        pointer allocate (size_type n) {
            // <your code>
          //cout << endl << "$$$$$In allocate" << endl;
          //cout << "allocating " << n << " elements" << endl;
          
		  if (n<=0 || n >=N) return 0;	
		  int index = 0;
          int sentinel_value, space_needed;           
          //space_needed = n * sizeof(value_type) + 2*sizeof(int);  // we need to fit a couple sentinels in new block
          space_needed = n * sizeof(value_type) ; // we need not fit a couple sentinels in new block
            
          while (index < N )  {   // looking up space until the end of the array
            sentinel_value = my_view(a[index]);
                
            if (sentinel_value > (signed)(space_needed +2*sizeof(int)) ) {   // found a spot, enough space leftover for another block
                
              //cout << endl << "$$Found a spot at index " << index << " , which shows " << sentinel_value ;
              //cout << " available, we need " << space_needed << ", excluding sentinels" ;
			  //cout << endl << "index of leftover's last sentinel is " << index+ sizeof(int) + sentinel_value;
              my_view(a[index+ sizeof(int) + sentinel_value]) -= (space_needed+2*sizeof(int)) ;  // set remain space's end sentinel
			  //cout << endl << "index of leftover's first sentinel is " << index+ sizeof(int) + space_needed;
			  my_view(a[index+ 2*sizeof(int) + space_needed]) = my_view(a[index+ sizeof(int) + sentinel_value]); //set its beginning's sentnel 
                 //cout << endl << "There was more space than needed, so we set the next sentinels" << endl;
              
		      my_view(a[index]) = -1 * space_needed; //update the space found 
			  my_view(a[index + space_needed + sizeof(int)]) = -1 * space_needed;

              return (pointer) &a[index+sizeof(int)];
		    } else if (sentinel_value >= space_needed)  {
			  my_view(a[index]) = -1 * my_view(a[index]);
			  my_view(a[index+ sentinel_value+sizeof(int)]) = my_view(a[index]);			
		
              return (pointer) &a[index+sizeof(int)];
			}
			assert ( my_view(a[index]) == my_view(a[index+ sizeof(int)  + abs(my_view(a[index]) )]));
            assert (valid());
            index += 2*sizeof(int) + abs(sentinel_value);
          }
          assert(valid());
          return 0;}                   // replace!

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                               // this is correct and exempt
            assert(valid());}                           // from the prohibition of new

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * (1) in time
         * <your documentation>
         * after deallocation adjacent free blocks must be coalesced
         */
        void deallocate (pointer p, size_type) {
            // <your code>
			
			if (p <= 0) return;
			
			assert(valid());
			char * char_ptr_p = reinterpret_cast<char*>(p);     // Casting a pointer to value_type, into a char *
			char * left_sentinel = (char_ptr_p - sizeof(int));					// address of beginning of left sentinel
			char * right_sentinel = (char_ptr_p + abs(my_view(*left_sentinel)));    
			assert ( my_view(*right_sentinel) == my_view(*left_sentinel));		// sanity check
			assert ( my_view(*right_sentinel) < 0);
			int new_sentinel_value = -1 * my_view(*left_sentinel);		
			
			//cout << endl << "In deallocate, previous sentinel is " << my_view(*left_sentinel);
			//cout << " matching sentinel is " << my_view(*right_sentinel) << endl;
						
			my_view(*left_sentinel) = new_sentinel_value;
			my_view(*right_sentinel) = new_sentinel_value; 
		    //cout << "value left of sentinel is"  << static_cast<void *> (left_sentinel );
		    //cout << endl << "value of a is " << static_cast<void *> (a) ;
			//cout << "value right  of sentinel is"  << static_cast<void *> (right_sentinel + sizeof(int) );
		    //cout << endl << "value of a is " << static_cast<void *> (a) ;
			

			if ( (left_sentinel  == a) && ((right_sentinel + sizeof(int))  == (N+a)))  {
				//cout << endl << "Full space alloc'ed" << endl;   // Entire space deallocated, no further actions necessary
			}else if ( left_sentinel == a) {				// Space deallocated start at beginning of array, but stop in middle
				if ( my_view(*(right_sentinel + sizeof(int))) > 0 ) {   // adjacent block on the right is positive
					//cout << endl << "Coalescing with right neighbor, no left neighbor..." ;
					coalesce (left_sentinel, right_sentinel + sizeof(int));
					assert(valid());
				} 
			}else if ((right_sentinel + sizeof(int))  == (N+a) ) {		// Space deallocated ends at end of array, but part
				int neighbor_value = my_view(*(left_sentinel - sizeof(int)));
				if ( neighbor_value > 0 ) {
					//cout << endl << "Coalescing with left neighbor, no right neighbor...";
					coalesce (left_sentinel - 2* sizeof(int) - abs(neighbor_value), left_sentinel);
					assert(valid());
				} 
			}else {					// the block freed is right in the middle of the array
				int right_neighbor_value = my_view(*(right_sentinel + sizeof(int)));
				int left_neighbor_value = my_view(*(left_sentinel - sizeof(int)));
					
				if (right_neighbor_value > 0 && left_neighbor_value > 0 ) {
					//cout << endl << "Coalescing with left and right neighbors... " << endl;
					coalesce(left_sentinel - 2* sizeof(int) - abs(left_neighbor_value), left_sentinel, right_sentinel + sizeof(int));
				
				} else if (right_neighbor_value > 0 ) {
					//cout << endl << "Coalescing with right neighbor... " << endl;
					coalesce(left_sentinel,right_sentinel + sizeof(int));
				
				} else if (left_neighbor_value >0 ) {

				} 				
			} 						
            assert(valid());}
		
		// ---------
		// coaslesce
		//---------
		/**
		*
		* coalesce two or three adjacent locations, the order of argument matters 
		* beg_p points to the beginning of the first location
		* beg_q points to the beginning of the second location
		*/
		
		void coalesce (char* beg_p, char* beg_q) {
			int updated_size = my_view(*beg_p) + my_view(*beg_q) + 2* sizeof(int);
			int old_left_sent= my_view (*beg_q);
			//cout << endl << "coalescing to size " << updated_size << endl;
			
			// updating left-most sentinel
			my_view( *beg_p) = updated_size;
			
			// updating right-most sentinel
			my_view ( *(beg_q+ sizeof(int) + old_left_sent)) = updated_size;
			
			assert(valid());
		}

		void coalesce (char* beg_p, char* beg_q, char* beg_r) {
			int updated_size = my_view(*beg_p) + my_view(*beg_q) + my_view(*beg_r) + 4 *sizeof(int) ;
			int r_old_sent_value = my_view(*beg_r);
			//cout << endl << "coalescing to size " << updated_size << endl;
			
			//updating left-most sentinel
			my_view( *beg_p) = updated_size;

			//updating right-most sentinel
			my_view (*(beg_r + sizeof(int) + r_old_sent_value)) = updated_size;
			
			assert (valid()); 
			
		}  	
		
        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}};

#endif // Allocator_h
