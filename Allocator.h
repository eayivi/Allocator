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
#include <cmath>    // abs
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
                open_sent_value =  view(a[index]);
                closing_sent_value =  view(a[index+ sizeof(int)  + abs(open_sent_value)]);
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
          //cout << endl << "$$$$$In allocate" << endl;
          //cout << "allocating " << n << " elements" << endl;
          int index = 0;
          int sentinel_value, space_needed;           
          space_needed = n * sizeof(value_type) + 2*sizeof(int);  // we need to fit a couple sentinels in new block
            
          while (index < N )  {   // looking up space until the end of the array
            sentinel_value = view(a[index]);
                
            if (sentinel_value >= space_needed) {   // found a spot
                
              //cout << endl << "$$Found a spot at index " << index << " , which shows " << sentinel_value ;
              //cout << " available, we need " << space_needed << ", including sentinels" ;
              if (sentinel_value > space_needed) {  // Enough space leftover for a future block?
                 view(a[index+ sizeof(int) + sentinel_value]) -= space_needed ;  // set leftover space's end sentinel
                 view(a[index+ space_needed]) = view(a[index+ sizeof(int) + sentinel_value]); //set its beginning's sentinel 
                 //cout << endl << "There was more space than needed, so we set the next sentinels" << endl;
              }
                    
              view(a[index]) = -1 * n* sizeof (value_type);       //update the space found 
              view(a[index + space_needed - sizeof(int)]) = -1 * n * sizeof(value_type);

              assert (valid());
              return (pointer) &a[index+sizeof(int)];

              }
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
         * O(1) in time
         * <your documentation>
         * after deallocation adjacent free blocks must be coalesced
         */
        void deallocate (pointer p, size_type) {
            // <your code>
            assert(valid());}

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
