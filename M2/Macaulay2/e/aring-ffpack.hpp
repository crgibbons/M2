// Copyright 2011 Michael E. Stillman

#ifndef _aring_ffpack_hpp_
#define _aring_ffpack_hpp_

#include "aring.hpp"
#include "buffer.hpp"
#include "ringelem.hpp"
#include <iostream>

//enable this lines to trick Kdevelop and do not forget to comment out them before committing. Other ideas ?
// #define HAVE_FFLAS_FFPACK 1 
// #define HAVE_GIVARO 1

#if not defined(HAVE_FFLAS_FFPACK)  
namespace M2 {
  
  class ARingZZpFFPACK : public DummyRing
  {
  public:
    static const RingID ringID = ring_FFPACK;
    
    typedef M2::ARingZZpFFPACK   ring_type;
    
    ARingZZpFFPACK(int charac) {}
  };
  
};
#else
#include <fflas-ffpack/field/modular-balanced.h>
#include <fflas-ffpack/field/modular-double.h>
#include <fflas-ffpack/ffpack/ffpack.h>
//#include <givaro/givgfq.h>

namespace M2 {

  /**
     @ingroup rings
     
     @brief wrapper for the FFPACK::ModularBalanced<double> field implementation
  */ 
  
  class ARingZZpFFPACK : public RingInterface
  {
  public:
    // @Jakob TODO: extract Signed_Trait from givaro.  Or use c++11.
    // Questions: why names are UTT, STT?
    // Also: whatever we call them, we want all aring classes to use them.
    // problem: givaro isn't necessarily defined here
    static const RingID ringID = ring_FFPACK;
    
    //typedef FFPACK::ModularBalanced<double> FieldType;
    typedef FFPACK::Modular<double> FieldType;

    typedef FieldType::Element ElementType;
    typedef ElementType elem;

    typedef  uint32_t UTT; ////// attention: depends on STT;currently manual update

    // see http://en.cppreference.com/w/cpp/types !
    typedef std::make_signed<UTT>::type STT;

    // if no givaro, use this:
    //typedef  int32_t STT; /// attention: depends on UTT; currently manual update

    // @todo: problem, wenn typ von cHarakteristif 
    ARingZZpFFPACK(UTT charac);

  public:
    // ring informational
    UTT characteristic() const { return mCharac; }

    UTT cardinality() const { return mCharac; }

    const ElementType& getGenerator() const
    {
      if (not mGeneratorComputed)
        {
          mGenerator = computeGenerator();
          mGeneratorComputed = true;
        }
      return mGenerator;
    }

    const FieldType field() const { return mFfpackField; }

    // @TODO: Jakob: what are the implications of using /// instead of /**.

  /** @name IO
  @{ 
  */
    void text_out(buffer &o) const { o << "ZZpFPACK(" << mCharac << "," << mDimension << ")"; }

    void elem_text_out
            (
              buffer &o, 
              const  ElementType a,
              bool p_one=true, 
              bool p_plus=false, 
              bool p_parens=false
            ) const;
  /** @} */

  /** @name properties
  @{     
  */
        
    bool is_unit(const ElementType f) const ;
    bool is_zero(const ElementType f) const ;

  /** @} */


  /** @name translation functions
      @{ */

    void to_ring_elem(ring_elem &result, const ElementType &a) const
    {
      // Note that the max modulus is small enough (about 70 million in 2013)
      // so that the coercion to an int will be correct.
      result.int_val = static_cast<int>(a);
    }
    
    void from_ring_elem(ElementType &result, const ring_elem &a) const
    {
      result = a.int_val;
    }

  /** @} */

  /** @name operators
      @{ */

    bool is_equal(const ElementType f,const ElementType g) const;
    int compare_elems(const ElementType f,const ElementType g) const;
  /** @} */

  /** @name init_set
  @{ */

    void init_set(ElementType &result, ElementType a) const { result = a; }
    
    void set(ElementType &result, ElementType a) const { result = a; }
    
    void init(ElementType &result) const ;
    
    void clear(ElementType &result) const ;
    
    void set_zero(ElementType &result) const ;
    
    void copy(ElementType &result,const ElementType a) const ;
    
    void set_from_int(ElementType &result, int a) const ;
    
    void set_from_mpz(ElementType &result,const mpz_ptr a) const ;
    
    void set_from_mpq(ElementType &result,const mpq_ptr a) const ;
    
    bool set_from_BigReal(ElementType &result, gmp_RR a) const { return false; }
    
    ElementType computeGenerator ( ) const; 
    
    void set_var(ElementType &result, int v) const         { result = getGenerator(); }
    
  /** @} */


  /** @name arithmetic
  @{ */
    void negate(ElementType &result,const ElementType a) const;
    
    void invert(ElementType &result,const ElementType a) const;

    void unsafeInvert(ElementType &result, const ElementType a) const;
    
    void add(ElementType &result, const ElementType a,const ElementType b) const;
    
    void subtract(ElementType &result,const  ElementType a,const  ElementType b) const;
    
    void subtract_multiple(ElementType &result,const  ElementType a,const  ElementType b) const;
    
    void mult(ElementType &result,const  ElementType a,const  ElementType b) const;
    
  ///@brief test doc
    void divide(ElementType &result,const  ElementType a,const  ElementType b) const;
    
    void power(ElementType &result,const  ElementType a,const  STT n) const;
    
    void power_mpz(ElementType &result,const  ElementType a,const  mpz_ptr n) const;
    
    void syzygy(const ElementType a, const ElementType b,
                ElementType &x, ElementType &y) const;
  /** @} */


  /** @name misc
  @{ */
    void swap(ElementType &a, ElementType &b) const;
    
    void random(ElementType &result) const;

    //TODO: Mike.  If possible, move promote, lift, eval to 
    // their own classes, or templated functions
    bool promote(const Ring *Rf, const ring_elem f, ElementType &result) const;
    
    bool lift(const Ring *Rg, const ElementType f, ring_elem &result) const;
    
    // map : this --> target(map)
    //       primelem --> map->elem(first_var)
    // evaluate map(f)
    void eval(const RingMap *map, const ElementType f, int first_var, ring_elem &result) const;
    
    static inline double getMaxModulus() 
    {
      if  (std::is_same<FFPACK::Modular<double> , FieldType>::value)
      {
	    return FieldType::getMaxModulus()/2;
      }
      return FieldType::getMaxModulus();
    }
  /** @} */

  private:
    const FieldType mFfpackField;
    mutable FieldType::RandIter   mFfpackRandomIterator;
    
    UTT mCharac;
    UTT mDimension; ///< same as extensionDegree
    
    /// use getGenerator() to access it since generator is cached and not 
    /// computed if not required.
    mutable ElementType mGenerator;

    mutable bool mGeneratorComputed;
  };

};

#endif  // HAVE_FFLAS_FFPACK
#endif

// Local Variables:
// compile-command: "make -C $M2BUILDDIR/Macaulay2/e  "
// indent-tabs-mode: nil
// End:
