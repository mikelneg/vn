#ifndef UUWOWFOBWSFF_VN_SCOPE_CALL_H_
#define UUWOWFOBWSFF_VN_SCOPE_CALL_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)                              

    namespace vn
         
    class scope_call<EnterFunc,ExitFunc>;
    scope_call<EnterFunc,ExitFunc> make_scope_call(EnterFunc&&, ExitFunc&&);
-----------------------------------------------------------------------------*/

namespace vn {

    template <typename EnterFunc, typename ExitFunc>
    class scope_call {
        EnterFunc on_enter;
        ExitFunc on_exit;   
    
        scope_call(EnterFunc on_enter, ExitFunc on_exit, int)
            : on_enter{std::move(on_enter)}, on_exit{std::move(on_exit)} {}

    public:
        // we use a deferred constructor to guarantee that the dtor is
        // called even if on_enter() throws an exception
        scope_call(EnterFunc on_enter, ExitFunc on_exit) 
            : scope_call(std::move(on_enter), std::move(on_exit), 0) 
        {
           on_enter();
        }

        ~scope_call() 
        {
           on_exit();
        }
    };

    template <typename N, typename X>
    scope_call<N,X> make_scope_call(N on_enter, X on_exit) 
    {
        return {std::move(on_enter), std::move(on_exit)};
    } 

} // namespace
#endif