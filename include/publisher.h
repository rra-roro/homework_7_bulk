#pragma once
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace roro_lib
{
      namespace internal
      {
            struct key
            {
                  using Facke = struct{};
                  using fn_mem_t = void (Facke::*)(void);

                  std::pair<void*, fn_mem_t> key_value;

                  key() = delete;

#if __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
                  template <typename T, typename FM, typename FMx = void (Facke::*)(void)>
                  key(const T& obj, FM fn) : key_value{ static_cast<void*>(const_cast<T*>(&obj)),
                                                        reinterpret_cast<FMx>(fn) }
                  {
                  }   
#if __GNUG__
#pragma GCC diagnostic pop
#endif
                  template <typename T>
                  key(const T& obj) : key_value{ static_cast<void*>(const_cast<T*>(&obj)), nullptr}
                  {
                  }

                  key(const key& key_arg) : key_value(key_arg.key_value)
                  {
                  }

                  key(key&& key_arg)
                  {
                        key_value.swap(key_arg.key_value);
                  }                  
            };

            bool operator==(const key& arg1, const key& arg2)
            {
                  return (arg1.key_value == arg2.key_value);
            }
      }
}

namespace std
{
      /*!   \brief  Это специализация std::hash для нашей структуры key.

      */
      template<>
      struct hash<roro_lib::internal::key>
      {
            using argument_t = roro_lib::internal::key;
            using result_t = std::intptr_t;

            result_t operator()(const argument_t& key_arg) const noexcept
            {
                  return std::hash<std::size_t>{}
                               (reinterpret_cast< std::intptr_t > (const_cast<void*>(key_arg.key_value.first)));
            }
      };
}


namespace roro_lib
{

      template <typename>
      class publisher_mixin;

      template <typename R, typename... Args>
      class publisher_mixin<R(Args...)>
      {
        public:
            template <typename F,
                typename std::enable_if_t<std::is_pointer_v<F> &&
                                          std::is_function_v<typename std::remove_pointer_t<F>>>* Facke = nullptr>
            void add_subscriber(F fn)
            {
                  static_assert(test_arg_subscriber_v<F>,
                      "the signature of the subscriber function must match the signature declared by the publisher");

                  add_subscriber_internal(fn);
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

        protected:
            void notify(Args... args)
            {
                  for (auto& subscriber : subscribers)
                  {
                        subscriber.second(args...);
                  }
            }

        private:
            
            std::unordered_map<internal::key, std::function<R(Args...)>> subscribers;

            template <typename T>
            constexpr void add_subscriber_internal(const T& fn)
            {
                  subscribers.insert({{fn},fn});
            }

            template <std::size_t I = 0,
                typename T,
                typename F = R (T::*)(Args...),
                std::size_t... PhNumber>
            constexpr void add_subscriber_internal(const T& obj, F fn)
            {
                  if constexpr (I < sizeof...(Args))
                  {
                        add_subscriber_internal<I + 1, T, F, PhNumber..., I>(obj, fn);
                  }
                  else if constexpr (sizeof...(Args) == 0)
                  {
                        subscribers.insert({ { obj, fn }, std::bind(fn, obj) });
                  }
                  else
                  {
                        using namespace std::placeholders;
                        constexpr auto placeholders_tuple = std::make_tuple(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10);
                        subscribers.insert({ { obj, fn }, std::bind(fn, obj, std::get<PhNumber>(placeholders_tuple)...) });
                  }
            }

            template <typename, typename Facke = void>
            struct test_arg_subscriber : std::false_type
            {
            };

            template <typename Ret, typename... A>
            struct test_arg_subscriber<Ret (*)(A...),
                                       std::enable_if_t<std::is_same_v<std::tuple<Ret, A...>,
                                                        std::tuple<R, Args...>>>> : std::true_type
            {
            };

            template <typename Ret, typename C, typename... A>
            struct test_arg_subscriber<Ret (C::*)(A...),
                std::enable_if_t<std::is_same_v<std::tuple<Ret, A...>,
                    std::tuple<R, Args...>>>> : std::true_type
            {
            };

            template <class _Ty>
            static constexpr bool test_arg_subscriber_v = test_arg_subscriber<_Ty>::value;

#ifdef PRIVATE_TEST
            FRIEND_TEST(PublisherMixinTest, UniqueAddSubscribers1);
            FRIEND_TEST(PublisherMixinTest, UniqueAddSubscribers2);
            FRIEND_TEST(PublisherMixinTest, UniqueAddSubscribers3);
#endif
      };
}
