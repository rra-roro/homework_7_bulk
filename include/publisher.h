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
                  using Facke = struct
                  {
                  };
                  using fn_mem_t = void (Facke::*)(void);

                  std::pair<void*, fn_mem_t> key_value;
                  bool rvalue;

                  key() = delete;

#if __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
                  template <typename T, typename FM, typename FMx = void (Facke::*)(void),
                      typename std::enable_if_t<std::is_rvalue_reference_v<T&&>>* tmp = nullptr>
                  key(T&& obj, FM fn) : key_value{ static_cast<void*>(&obj),
                                              reinterpret_cast<FMx>(fn) },
                                        rvalue(true)
                  {
                  }

                  template <typename T, typename FM, typename FMx = void (Facke::*)(void),
                      typename std::enable_if_t<!std::is_rvalue_reference_v<T&&>>* tmp = nullptr>
                  key(T&& obj, FM fn) : key_value{ static_cast<void*>(&obj),
                                              reinterpret_cast<FMx>(fn) },
                                        rvalue(false)
                  {
                  }

#if __GNUG__
#pragma GCC diagnostic pop
#endif
                  template <typename T>
                  key(const T& obj) : key_value{ static_cast<void*>(const_cast<T*>(&obj)), nullptr },
                                      rvalue(false)
                  {
                  }

                  key(const key& key_arg) : key_value(key_arg.key_value), rvalue(key_arg.rvalue)
                  {
                  }

                  key(key&& key_arg) : rvalue(key_arg.rvalue)
                  {
                        key_value.swap(key_arg.key_value);
                  }
            };

            bool operator==(const key& arg1, const key& arg2)
            {
                  if (arg1.rvalue == true && arg2.rvalue == true)
                  {
                        return false;
                  }
                  else
                        return (arg1.key_value == arg2.key_value);
            }
      }
}

namespace std
{
      /*!   \brief  Это специализация std::hash для нашей структуры key.

      */
      template <>
      struct hash<roro_lib::internal::key>
      {
            using argument_t = roro_lib::internal::key;
            using result_t = std::intptr_t;

            result_t operator()(const argument_t& key_arg) const noexcept
            {
                  return std::hash<std::size_t>{}(reinterpret_cast<std::intptr_t>(const_cast<void*>(key_arg.key_value.first)));
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
            static_assert(sizeof...(Args) < 20,
                "the subscriber must have less 20 arguments");

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
            void add_subscriber(T&& obj, MF mfn)
            {
                  static_assert(test_arg_subscriber_v<MF>,
                      "the signature of the subscriber member function must match the signature declared by the publisher");

                  add_subscriber_internal(std::forward<T>(obj), mfn);
            }

            template <typename Ref_,
                typename T = std::remove_reference_t<Ref_>,
                typename std::enable_if_t<std::is_member_function_pointer_v<decltype(&T::operator())>>* Facke = nullptr>
            void add_subscriber(Ref_&& obj)
            {
                  if constexpr (std::is_same_v<T, std::function<R(Args...)>>)
                  {
                        if (obj != nullptr) // function != nullptr
                              add_subscriber_internal(std::forward<Ref_>(obj));
                  }
                  else
                  {
                        static_assert(test_arg_subscriber_v<decltype(&T::operator())>,
                            "the signature of the subscriber functor must match the signature declared by the publisher");

                        add_subscriber(std::forward<Ref_>(obj), &T::operator());
                  }
            }

            template <typename T,
                      typename std::enable_if_t<std::is_bind_expression_v<T>>* Facke = nullptr>
            void add_subscriber(const T& obj)
            {
                  add_subscriber(std::function<R(Args...)>(obj));
            }

        protected:
            void notify(Args... args)
            {
                  for (auto& subscriber : subscribers)
                  {
                        subscriber.second(args...);
                  }
            }

            // non-virtual protected destructor special for mixin
            ~publisher_mixin(){};

        private:
            std::unordered_map<internal::key, std::function<R(Args...)>> subscribers;

            // добавляем указатель на ф-ию
            template <typename T>
            constexpr void add_subscriber_internal(const T& fn)
            {
                  subscribers.insert({ { fn }, fn });
            }

            // добавляем function
            template <typename Ref_,
                typename T = std::remove_reference_t<Ref_>,
                typename std::enable_if_t<std::is_same_v<T, std::function<R(Args...)>>>* Facke = nullptr>
            constexpr void add_subscriber_internal(Ref_&& fn)
            {
                  internal::key fn_key(fn);

                  if constexpr (std::is_rvalue_reference_v<Ref_&&>)
                        fn_key.rvalue = true;
                  else
                        fn_key.rvalue = false;

                  subscribers.insert({ fn_key, fn });
            }

            // добавляем прочие функторы
            template <std::size_t I = 0,
                typename T,
                typename F = R (T::*)(Args...),
                std::size_t... PhNumber>
            constexpr void add_subscriber_internal(T&& obj, F fn)
            {
                  if constexpr (I < sizeof...(Args))
                  {
                        add_subscriber_internal<I + 1, T, F, PhNumber..., I>(obj, fn);
                  }
                  else if constexpr (sizeof...(Args) == 0)
                  {
                        if constexpr (std::is_rvalue_reference_v<T&&>)
                              subscribers.insert({ { std::forward<T>(obj), fn }, std::bind(fn, obj) });
                        else
                              subscribers.insert({ { std::forward<T>(obj), fn }, std::bind(fn, std::ref(obj)) });
                  }
                  else
                  {
                        using namespace std::placeholders;
                        constexpr auto placeholders_tuple = std::make_tuple(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,
                            _11, _12, _13, _14, _15, _16, _17, _18, _19, _20);

                        if constexpr (std::is_rvalue_reference_v<T&&>)
                              subscribers.insert({ { std::forward<T>(obj), fn },
                                  std::bind(fn, obj, std::get<PhNumber>(placeholders_tuple)...) });
                        else
                              subscribers.insert({ { std::forward<T>(obj), fn },
                                  std::bind(fn, std::ref(obj), std::get<PhNumber>(placeholders_tuple)...) });
                  }
            }

            // проверка соответствия сигнатуры подписчиков
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
            FRIEND_TEST(PublisherMixinTest, copy);
#endif
      };
}
