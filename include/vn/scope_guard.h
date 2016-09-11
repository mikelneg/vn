/*-------------------------------------------------------------

Copyright (c) 2016 Mikel Negugogor (http://github.com/mikelneg)
MIT license. See LICENSE.txt in project root for details.

---------------------------------------------------------------*/

#ifndef UUWOWFOBWSFF_VN_SCOPE_GUARD_H_
#define UUWOWFOBWSFF_VN_SCOPE_GUARD_H_

namespace vn {

template <typename EnterFunc, typename ExitFunc>
class scope_guard {
    EnterFunc on_enter;
    ExitFunc on_exit;

    scope_guard(EnterFunc on_enter, ExitFunc on_exit, int) noexcept
        : on_enter{std::move(on_enter)}, on_exit{std::move(on_exit)}
    {
    }

public:
    scope_guard(scope_guard&&) = delete;
    scope_guard& operator=(scope_guard&&) = delete;

    // we use a deferred constructor to guarantee that the dtor is
    // called even if on_enter() throws an exception
    scope_guard(EnterFunc on_enter, ExitFunc on_exit)
        : scope_guard(std::move(on_enter), std::move(on_exit), 0)
    {
        on_enter();
    }

    ~scope_guard()
    {
        on_exit();
    }
};

template <typename T, typename U>
scope_guard<T, U> make_scope_guard(T on_enter, U on_exit)
{
    return {std::move(on_enter), std::move(on_exit)};
}

} // namespace
#endif