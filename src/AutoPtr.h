// AutoPtr.h: interface for the AutoPtr class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOPTR_H__405B5055_DBA2_4A05_B74E_69125C005689__INCLUDED_)
#define AFX_AUTOPTR_H__405B5055_DBA2_4A05_B74E_69125C005689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////
template<class C> class AutoPtr   {   
private:
    AutoPtr& operator =( const AutoPtr& aptr) {};  // forbid copy
    AutoPtr(const AutoPtr& aptr) {};               // forbid copy-constructing

protected: 
    C*  m_pointer;   // pointer itself is protected

public:
    AutoPtr(C* pointer): m_pointer(pointer) 
        {
        }      

    AutoPtr(): m_pointer(NULL) 
        {
        }

    virtual ~AutoPtr()       
        { 
        Remove(); 
        }
    
    void operator= (const C* pointer)             // pointer assignment (deletes previous)
        { 
        Remove(); 
        m_pointer = (C*)pointer;  
        } 

    virtual void Remove()  
        { 
        if (m_pointer != NULL) 
            delete m_pointer; 
        m_pointer = NULL; 
        }
              
    //  compare with pointer
    bool  operator==(const C* pointer) 
        { 
        return m_pointer == pointer; 
        }    
    bool  operator!=(const C* pointer) 
        { 
        return m_pointer != pointer; 
        }     
    
    //  compare with AutoPtr
    bool  operator==(const AutoPtr<C>& aptr) { return m_pointer == aptr.m_pointer; }    
    bool  operator!=(const AutoPtr<C>& aptr) { return m_pointer != aptr.m_pointer; }     
    
    C* operator->()
        { 
        return m_pointer; 
        }
    
    C** operator& ()
        { 
        return &m_pointer; 
        }
          
    operator C* ()                                                  // conversion to pointer
        { 
        return m_pointer; 
        }
          
    C&     operator* ()
        { 
        return *m_pointer; 
        }

    };

/////////////////////////////////////
template<class C> class AutoIPtr : public AutoPtr<C>
    {   
public:
    AutoIPtr<C>(): AutoPtr<C>() {}
    AutoIPtr<C>(C* pointer): AutoPtr<C>(pointer) {}      
    
    virtual ~AutoIPtr<C>()       
        { 
        Remove(); 
        }
    void operator= (const C* pointer)
        { 
        Remove(); 
        m_pointer = (C*)pointer;  
        } 
    virtual void Remove()  
        { 
        if (m_pointer != NULL) 
            m_pointer->Release(); 
        m_pointer = NULL; 
        }
    };
    
#endif // !defined(AFX_AUTOPTR_H__405B5055_DBA2_4A05_B74E_69125C005689__INCLUDED_)
