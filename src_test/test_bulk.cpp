#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

#include "lib_version.h"

#define PRIVATE_TEST 1
#include "publisher.h"

TEST(version, test1)
{
      ASSERT_TRUE(version() > 0);
}


struct publisher : public roro_lib::publisher_mixin<void(void)>
{
      void run()
      {
            notify();
      };
};


void subscriber_fn()
{
}

struct subscriber_functor
{
      void test(){};

      void operator()()
      {
      }
};

namespace roro_lib
{

      class PublisherMixinTest : public testing::Test
      {
        public:
            void SetUp() {}
            void TearDown() {}
      };

      TEST_F(PublisherMixinTest, UniqueAddSubscribers1)
      {
            publisher pbl;

            pbl.add_subscriber(subscriber_fn); // 1
            ASSERT_TRUE(pbl.subscribers.size() == 1);

            pbl.add_subscriber(subscriber_functor()); // 2  rvalue - добавляем
            ASSERT_TRUE(pbl.subscribers.size() == 2);

            subscriber_functor sf;

            pbl.add_subscriber(sf); // 3
            ASSERT_TRUE(pbl.subscribers.size() == 3);

            pbl.add_subscriber(sf, &subscriber_functor::test); // 4
            ASSERT_TRUE(pbl.subscribers.size() == 4);

            std::function<void(void)> fn1 = subscriber_fn;
            pbl.add_subscriber(fn1);
            ASSERT_TRUE(pbl.subscribers.size() == 5); // 5

            pbl.add_subscriber(std::function<void(void)>(subscriber_fn)); // 6 rvalue - добавляем
            ASSERT_TRUE(pbl.subscribers.size() == 6);


            // ----
            pbl.add_subscriber(subscriber_functor()); // 7  rvalue - добавляем повторно
            ASSERT_TRUE(pbl.subscribers.size() == 7);

            pbl.add_subscriber(std::function<void(void)>(subscriber_fn)); // 8 rvalue - добавляем повторно
            ASSERT_TRUE(pbl.subscribers.size() == 8);

            // ----
            pbl.add_subscriber(subscriber_fn);                 // Уже есть в подписчиках
            pbl.add_subscriber(sf);                            // Уже есть в подписчиках
            pbl.add_subscriber(sf, &subscriber_functor::test); // Уже есть в подписчиках

            std::function<void(void)> fn2;
            pbl.add_subscriber(fn1); // function == nullptr  - игнорируем
            pbl.add_subscriber(fn2); // Уже есть в подписчиках


            ASSERT_TRUE(pbl.subscribers.size() == 8);
      }
}

struct subscriber_base0
{
      int m[52];
      void test0(){};
};

struct subscriber_base1
{
      int m[30];
      void test0(){};
      void test1(){};
      void test2(){};
};

struct subscriber_base2 : subscriber_base1
{
      int m[256];
      void test2(){};
};

struct subscriber_d : subscriber_base2
{
      void test0(){};
};

struct subscriber_md : subscriber_base0, subscriber_base1
{
      void test0(){};
};

namespace roro_lib
{
      TEST_F(PublisherMixinTest, UniqueAddSubscribers2)
      {
            publisher pbl;
            subscriber_d sd;
            subscriber_md md;

            pbl.add_subscriber(sd, &subscriber_d::test0);
            ASSERT_TRUE(pbl.subscribers.size() == 1);
            pbl.add_subscriber(sd, &subscriber_d::test1);
            ASSERT_TRUE(pbl.subscribers.size() == 2);
            pbl.add_subscriber(sd, &subscriber_d::test2);
            ASSERT_TRUE(pbl.subscribers.size() == 3);

            pbl.add_subscriber(sd, &subscriber_d::subscriber_base1::test0);
            ASSERT_TRUE(pbl.subscribers.size() == 4);
            pbl.add_subscriber(sd, &subscriber_d::subscriber_base1::test2);
            ASSERT_TRUE(pbl.subscribers.size() == 5);

            pbl.add_subscriber(sd, &subscriber_d::test0);                   // Уже есть в подписчиках
            pbl.add_subscriber(sd, &subscriber_d::subscriber_base1::test2); // Уже есть в подписчиках

            ASSERT_TRUE(pbl.subscribers.size() == 5);

            // pbl.add_subscriber(md, &subscriber_md::test0);                // Подписчик напрямую не может быть типом с
            //                                                                  Множественным и/или виртуальным наследованием

            pbl.add_subscriber(std::bind(&subscriber_md::test0, md)); // Но через bind может
            ASSERT_TRUE(pbl.subscribers.size() == 6);

            pbl.add_subscriber(std::bind(&subscriber_md::test0, md)); // через bind подписываем многократно
            ASSERT_TRUE(pbl.subscribers.size() == 7);
      }
}

struct i_subscriber
{
      virtual void test0() = 0;
      virtual void test1() = 0;
};

struct subscriber_base : i_subscriber
{
      void test0(){};
      void test1(){};
      virtual void test2(){};
};

struct subscriber_d2 : subscriber_base
{
      void test2(){};
};

namespace roro_lib
{
      TEST_F(PublisherMixinTest, UniqueAddSubscribers3)
      {
            publisher pbl;

            subscriber_d2 sd1;
            subscriber_d2 sd2;


            //i_subscriber& is = sd2;
            subscriber_base& sb = sd2;

            pbl.add_subscriber(sd1, &subscriber_d2::test0);
            ASSERT_TRUE(pbl.subscribers.size() == 1);
            pbl.add_subscriber(sd1, &subscriber_d2::test1);
            ASSERT_TRUE(pbl.subscribers.size() == 2);
            pbl.add_subscriber(sd1, &subscriber_d2::test2);
            ASSERT_TRUE(pbl.subscribers.size() == 3);

            //pbl.add_subscriber(is, &i_subscriber::test0);   // is  - нельзя инстанцировать
            //pbl.add_subscriber(is, &i_subscriber::test1);
            pbl.add_subscriber(sb, &subscriber_d2::test0);
            ASSERT_TRUE(pbl.subscribers.size() == 4);
            pbl.add_subscriber(sb, &subscriber_d2::test1);
            ASSERT_TRUE(pbl.subscribers.size() == 5);
            //pbl.add_subscriber(sb, &subscriber_d2::test2);  // &subscriber_d2::test2 - обращение к ф-ии что нет в subscriber_base
      }
}

struct subscriber_state
{
      int st = 0;
      void test() { ++st; };
      void operator()() { ++st; };
};

namespace roro_lib
{
      TEST_F(PublisherMixinTest, StoreSubscribersByRef)
      {
            publisher pbl;

            subscriber_state sscr;

            pbl.add_subscriber(sscr);
            pbl.add_subscriber(sscr, &subscriber_state::test);
            pbl.run();

            ASSERT_TRUE(sscr.st == 2);
      }

      TEST_F(PublisherMixinTest, StoreSubscribersByValue)
      {
            publisher pbl;

            subscriber_state sscr;

            pbl.add_subscriber(std::move(sscr));
            pbl.add_subscriber(std::move(sscr), &subscriber_state::test);

            pbl.add_subscriber(subscriber_state());
            pbl.add_subscriber(subscriber_state(), &subscriber_state::test);

            pbl.run();

            ASSERT_TRUE(sscr.st == 0);
      }

      TEST_F(PublisherMixinTest, copy)
      {
            std::function<void(void)> fn1 = subscriber_fn;
            subscriber_functor sf;

            publisher pbl1;
            pbl1.add_subscriber(subscriber_fn); 
            pbl1.add_subscriber(subscriber_functor()); 
            pbl1.add_subscriber(sf); 
            pbl1.add_subscriber(sf, &subscriber_functor::test);
            pbl1.add_subscriber(fn1);
            pbl1.add_subscriber(std::function<void(void)>(subscriber_fn));

            ASSERT_TRUE(pbl1.subscribers.size() == 6);

            publisher pbl2(pbl1);
            ASSERT_TRUE(pbl2.subscribers.size() == 6);

            publisher pbl3;
            pbl3 = pbl1;
            ASSERT_TRUE(pbl3.subscribers.size() == 6);
      }
}
