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
            int index = 0; // The beginning index of the open sentinel
            int open_sent_value, closing_sent_value;            
            while (index < N) {
                assert (index>=0);
				open_sent_value =  view(a[index]);
                closing_sent_value =  view(a[index+ sizeof(int)  + abs(open_sent_value)]);
                if (open_sent_value != closing_sent_value)
                    return false;

                index = index + 2 * sizeof(int) + abs(open_sent_value); 

            }
            return true;}

        // ------------
        // View 
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */

        int& view (char& c) const {
            return *reinterpret_cast<int*>(&c); }
        
        int view (const char& c) const {
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
            if (N < 2* sizeof(int)) throw bad_alloc ();
			view(a[0]) = N- 2* sizeof(int);
            view(a[N-sizeof(int)]) = N- 2* sizeof(int);
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
          
		  if (n<=0 || n >=N) return 0;	
		  int index = 0;
          int sentinel_value, space_needed;           
          space_needed = n * sizeof(value_type) ; // we need not fit a couple sentinels in new block
            
          while (index < N )  {   // looking up space until the end of the array
            sentinel_value = view(a[index]);
                
            if (sentinel_value > (signed)(space_needed +2*sizeof(int)) ) {   // found a spot, enough space leftover for another block
                
              view(a[index+ sizeof(int) + sentinel_value]) -= (space_needed+2*sizeof(int)) ;  // set remain space's end sentinel
			  view(a[index+ 2*sizeof(int) + space_needed]) = view(a[index+ sizeof(int) + sentinel_value]); //set its beginning's sentnel 
              
		      view(a[index]) = -1 * space_needed; //update the space found 
			  view(a[index + space_needed + sizeof(int)]) = -1 * space_needed;

              return (pointer) &a[index+sizeof(int)];
		    } else if (sentinel_value >= space_needed)  {
			  view(a[index]) = -1 * view(a[index]);
			  view(a[index+ sentinel_value+sizeof(int)]) = view(a[index]);			
		
              return (pointer) &a[index+sizeof(int)];
			}
			assert ( view(a[index]) == view(a[index+ sizeof(int)  + abs(view(a[index]) )]));
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
			char * right_sentinel = (char_ptr_p + abs(view(*left_sentinel)));    
			assert ( view(*right_sentinel) == view(*left_sentinel));		// sanity check
			assert ( view(*right_sentinel) < 0);
			int new_sentinel_value =  abs( view(*left_sentinel));		
			
			view(*left_sentinel) = new_sentinel_value;
			view(*right_sentinel) = new_sentinel_value; 

			if ( (left_sentinel  == a) && ((right_sentinel + sizeof(int))  == (N+a)))  {
				// Entire space deallocated, no further actions necessary
			}else if ( left_sentinel == a) {				// Space deallocated start at beginning of array, but stop in middle
				if ( view(*(right_sentinel + sizeof(int))) > 0 ) {   // adjacent block on the right is positive
					coalesce (left_sentinel, right_sentinel + sizeof(int));
					assert(valid());
				} 
			}else if ((right_sentinel + sizeof(int))  == (N+a) ) {		// Space deallocated ends at end of array, but part
				int neighbor_value = view(*(left_sentinel - sizeof(int)));
				if ( neighbor_value > 0 ) {
					coalesce (left_sentinel - 2* sizeof(int) - abs(neighbor_value), left_sentinel);
					assert(valid());
				} 
			}else {					// the block freed is right in the middle of the array
				int right_neighbor_value = view(*(right_sentinel + sizeof(int)));
				int left_neighbor_value = view(*(left_sentinel - sizeof(int)));
					
				if (right_neighbor_value > 0 && left_neighbor_value > 0 ) {
					coalesce(left_sentinel - 2* sizeof(int) - abs(left_neighbor_value), left_sentinel, right_sentinel + sizeof(int));
				
				} else if (right_neighbor_value > 0 ) {
					
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
			int updated_size = view(*beg_p) + view(*beg_q) + 2* sizeof(int);
			int old_left_sent= view (*beg_q);
			//cout << endl << "coalescing to size " << updated_size << endl;
			
			// updating left-most sentinel
			view( *beg_p) = updated_size;
			
			// updating right-most sentinel
			view ( *(beg_q+ sizeof(int) + old_left_sent)) = updated_size;
			
			assert(valid());
		}

		void coalesce (char* beg_p, char* beg_q, char* beg_r) {
			int updated_size = view(*beg_p) + view(*beg_q) + view(*beg_r) + 4 *sizeof(int) ;
			int r_old_sent_value = view(*beg_r);
			
			//updating left-most sentinel
			view( *beg_p) = updated_size;

			//updating right-most sentinel
			view (*(beg_r + sizeof(int) + r_old_sent_value)) = updated_size;
			
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
