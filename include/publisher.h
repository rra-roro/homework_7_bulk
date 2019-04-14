#pragma once
#include <set>
#include <functional>
#include <type_traits>

namespace roro_lib
{
      namespace internal
      {
            inline static auto fn_comp = [](auto& fn1, auto& fn2) {
                  return &fn1 < &fn2;
            };
      }

      template <typename>
      class publisher_mixin;

      template <typename R, typename... Args>
      class publisher_mixin<R(Args...)>
      {
            std::set< std::function<R(Args...)>,
                      decltype(internal::fn_comp)> subscribers{ internal::fn_comp };

            template <std::size_t I = 0,
                      typename T,
                      typename F = R (T::*)(Args...),
                      std::size_t... PhNumber
            >
            constexpr void add_subscriber_internal(const T& obj, F fn)
            {
                  if constexpr (I < sizeof...(Args))
                  {
                        add_subscriber_internal<I + 1, T, F, PhNumber..., I>(obj, fn);
                  }
                  else
                  {
                        using namespace std::placeholders;
                        constexpr auto placeholders_tuple = std::make_tuple(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10);
                        subscribers.insert(std::bind(fn, obj, std::get<PhNumber>(placeholders_tuple)...));
                  }
            }

            template <typename, typename Facke = void>
            struct test_arg_subscriber : std::false_type
            {
            };

            template <typename Ret, typename... A>
            struct test_arg_subscriber<Ret (*)(A...),
                                       std::enable_if_t<std::is_same_v<std::tuple<Ret,A...>,
                                                                       std::tuple<R, Args...>>>> : std::true_type
            {
            };

            template <typename Ret, typename C, typename... A>
            struct test_arg_subscriber<Ret (C::*)(A...),
                                       std::enable_if_t<std::is_same_v<std::tuple<Ret,A...>,
                                                                       std::tuple<R, Args...>>>> : std::true_type
            {
            };

            template <class _Ty>
            static constexpr bool test_arg_subscriber_v = test_arg_subscriber<_Ty>::value;

        public:
            template <typename F,
                      typename std::enable_if_t<std::is_pointer_v<F> &&
                                                std::is_function_v<typename std::remove_pointer_t<F>>>* Facke = nullptr>
            void add_subscriber(F fn)
            {
                  static_assert(test_arg_subscriber_v<F>,
                                "the signature of the subscriber function must match the signature declared by the publisher");

                  subscribers.insert(fn);
            }

            template <typename T, typename MF,
                      typename std::enable_if_t<std::is_member_function_pointer_v<MF>>* Facke = nullptr>
            void add_subscriber(const T& obj, MF mfn)
            {
                  static_assert(test_arg_subscriber_v<MF>,
                                "the signature of the subscriber member function must match the signature declared by the publisher");

                  add_subscriber_internal(obj, mfn);
            }

            template <typename T,
                      typename std::enable_if_t<std::is_member_function_pointer_v<decltype(&T::operator())>>* Facke = nullptr>
            void add_subscriber(const T& obj)
            {
                  static_assert(test_arg_subscriber_v<decltype(&T::operator())>,
                                "the signature of the subscriber functor must match the signature declared by the publisher");

                  add_subscriber(obj, &T::operator());
            }


            void notify(Args... args)
            {
                  for (auto& subscriber : subscribers)
                  {
                        subscriber(args...);
                  }
            }
      };
}
